/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <com/sun/star/io/BufferSizeExceededException.hpp>
#include <com/sun/star/io/NotConnectedException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/packages/NoEncryptionException.hpp>
#include <com/sun/star/packages/WrongPasswordException.hpp>
#include <com/sun/star/packages/zip/ZipConstants.hpp>
#include <com/sun/star/packages/zip/ZipException.hpp>
#include <com/sun/star/packages/zip/ZipIOException.hpp>
#include <com/sun/star/xml/crypto/XCipherContext.hpp>
#include <com/sun/star/xml/crypto/XDigestContext.hpp>
#include <com/sun/star/xml/crypto/CipherID.hpp>
#include <com/sun/star/xml/crypto/DigestID.hpp>
#include <com/sun/star/xml/crypto/NSSInitializer.hpp>

#include <comphelper/bytereader.hxx>
#include <comphelper/storagehelper.hxx>
#include <comphelper/processfactory.hxx>
#include <rtl/digest.h>
#include <sal/log.hxx>
#include <o3tl/safeint.hxx>
#include <osl/diagnose.h>

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include "blowfishcontext.hxx"
#include "sha1context.hxx"
#include <ZipFile.hxx>
#include <ZipEnumeration.hxx>
#include "XUnbufferedStream.hxx"
#include "XBufferedThreadedStream.hxx"
#include <PackageConstants.hxx>
#include <EncryptedDataHeader.hxx>
#include <EncryptionData.hxx>
#include "MemoryByteGrabber.hxx"

#include <CRC32.hxx>

using namespace com::sun::star;
using namespace com::sun::star::io;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::packages;
using namespace com::sun::star::packages::zip;
using namespace com::sun::star::packages::zip::ZipConstants;

using ZipUtils::Inflater;

#if OSL_DEBUG_LEVEL > 0
#define THROW_WHERE SAL_WHERE
#else
#define THROW_WHERE ""
#endif

/** This class is used to read entries from a zip file
 */
ZipFile::ZipFile( rtl::Reference<comphelper::RefCountedMutex> aMutexHolder,
                  uno::Reference < XInputStream > const &xInput,
                  uno::Reference < XComponentContext > xContext,
                  bool bInitialise )
: m_aMutexHolder(std::move( aMutexHolder ))
, aGrabber( xInput )
, aInflater( true )
, xStream(xInput)
, m_xContext (std::move( xContext ))
, bRecoveryMode( false )
{
    if (bInitialise && readCEN() == -1 )
    {
        aEntries.clear();
        throw ZipException( "stream data looks to be broken" );
    }
}

ZipFile::ZipFile( rtl::Reference< comphelper::RefCountedMutex > aMutexHolder,
                  uno::Reference < XInputStream > const &xInput,
                  uno::Reference < XComponentContext > xContext,
                  bool bInitialise, bool bForceRecovery)
: m_aMutexHolder(std::move( aMutexHolder ))
, aGrabber( xInput )
, aInflater( true )
, xStream(xInput)
, m_xContext (std::move( xContext ))
, bRecoveryMode( bForceRecovery )
{
    if (bInitialise)
    {
        if ( bForceRecovery )
        {
            recover();
        }
        else if ( readCEN() == -1 )
        {
            aEntries.clear();
            throw ZipException("stream data looks to be broken" );
        }
    }
}

ZipFile::~ZipFile()
{
    aEntries.clear();
}

void ZipFile::setInputStream ( const uno::Reference < XInputStream >& xNewStream )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    xStream = xNewStream;
    aGrabber.setInputStream ( xStream );
}

uno::Reference< xml::crypto::XDigestContext > ZipFile::StaticGetDigestContextForChecksum( const uno::Reference< uno::XComponentContext >& xArgContext, const ::rtl::Reference< EncryptionData >& xEncryptionData )
{
    uno::Reference< xml::crypto::XDigestContext > xDigestContext;
    if ( xEncryptionData->m_nCheckAlg == xml::crypto::DigestID::SHA256_1K )
    {
        uno::Reference< uno::XComponentContext > xContext = xArgContext;
        if ( !xContext.is() )
            xContext = comphelper::getProcessComponentContext();

        uno::Reference< xml::crypto::XNSSInitializer > xDigestContextSupplier = xml::crypto::NSSInitializer::create( xContext );

        xDigestContext.set( xDigestContextSupplier->getDigestContext( xEncryptionData->m_nCheckAlg, uno::Sequence< beans::NamedValue >() ), uno::UNO_SET_THROW );
    }
    else if ( xEncryptionData->m_nCheckAlg == xml::crypto::DigestID::SHA1_1K )
    {
        if (xEncryptionData->m_bTryWrongSHA1)
        {
            xDigestContext.set(StarOfficeSHA1DigestContext::Create(), uno::UNO_SET_THROW);
        }
        else
        {
            xDigestContext.set(CorrectSHA1DigestContext::Create(), uno::UNO_SET_THROW);
        }
    }

    return xDigestContext;
}

uno::Reference< xml::crypto::XCipherContext > ZipFile::StaticGetCipher( const uno::Reference< uno::XComponentContext >& xArgContext, const ::rtl::Reference< EncryptionData >& xEncryptionData, bool bEncrypt )
{
    uno::Reference< xml::crypto::XCipherContext > xResult;

    if (xEncryptionData->m_nDerivedKeySize < 0)
    {
        throw ZipIOException("Invalid derived key length!" );
    }

    uno::Sequence< sal_Int8 > aDerivedKey( xEncryptionData->m_nDerivedKeySize );
    if ( !xEncryptionData->m_nIterationCount &&
         xEncryptionData->m_nDerivedKeySize == xEncryptionData->m_aKey.getLength() )
    {
        // gpg4libre: no need to derive key, m_aKey is already
        // usable as symmetric session key
        aDerivedKey = xEncryptionData->m_aKey;
    }
    else if ( rtl_Digest_E_None != rtl_digest_PBKDF2( reinterpret_cast< sal_uInt8* >( aDerivedKey.getArray() ),
                        aDerivedKey.getLength(),
                        reinterpret_cast< const sal_uInt8 * > (xEncryptionData->m_aKey.getConstArray() ),
                        xEncryptionData->m_aKey.getLength(),
                        reinterpret_cast< const sal_uInt8 * > ( xEncryptionData->m_aSalt.getConstArray() ),
                        xEncryptionData->m_aSalt.getLength(),
                        xEncryptionData->m_nIterationCount ) )
    {
        throw ZipIOException("Can not create derived key!" );
    }

    if ( xEncryptionData->m_nEncAlg == xml::crypto::CipherID::AES_CBC_W3C_PADDING )
    {
        uno::Reference< uno::XComponentContext > xContext = xArgContext;
        if ( !xContext.is() )
            xContext = comphelper::getProcessComponentContext();

        uno::Reference< xml::crypto::XNSSInitializer > xCipherContextSupplier = xml::crypto::NSSInitializer::create( xContext );

        xResult = xCipherContextSupplier->getCipherContext( xEncryptionData->m_nEncAlg, aDerivedKey, xEncryptionData->m_aInitVector, bEncrypt, uno::Sequence< beans::NamedValue >() );
    }
    else if ( xEncryptionData->m_nEncAlg == xml::crypto::CipherID::BLOWFISH_CFB_8 )
    {
        xResult = BlowfishCFB8CipherContext::Create( aDerivedKey, xEncryptionData->m_aInitVector, bEncrypt );
    }
    else
    {
        throw ZipIOException("Unknown cipher algorithm is requested!" );
    }

    return xResult;
}

void ZipFile::StaticFillHeader( const ::rtl::Reference< EncryptionData >& rData,
                                sal_Int64 nSize,
                                const OUString& aMediaType,
                                sal_Int8 * & pHeader )
{
    // I think it's safe to restrict vector and salt length to 2 bytes !
    sal_Int16 nIVLength = static_cast < sal_Int16 > ( rData->m_aInitVector.getLength() );
    sal_Int16 nSaltLength = static_cast < sal_Int16 > ( rData->m_aSalt.getLength() );
    sal_Int16 nDigestLength = static_cast < sal_Int16 > ( rData->m_aDigest.getLength() );
    sal_Int16 nMediaTypeLength = static_cast < sal_Int16 > ( aMediaType.getLength() * sizeof( sal_Unicode ) );

    // First the header
    *(pHeader++) = ( n_ConstHeader >> 0 ) & 0xFF;
    *(pHeader++) = ( n_ConstHeader >> 8 ) & 0xFF;
    *(pHeader++) = ( n_ConstHeader >> 16 ) & 0xFF;
    *(pHeader++) = ( n_ConstHeader >> 24 ) & 0xFF;

    // Then the version
    *(pHeader++) = ( n_ConstCurrentVersion >> 0 ) & 0xFF;
    *(pHeader++) = ( n_ConstCurrentVersion >> 8 ) & 0xFF;

    // Then the iteration Count
    sal_Int32 nIterationCount = rData->m_nIterationCount;
    *(pHeader++) = static_cast< sal_Int8 >(( nIterationCount >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nIterationCount >> 8 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nIterationCount >> 16 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nIterationCount >> 24 ) & 0xFF);

    // FIXME64: need to handle larger sizes
    // Then the size:
    *(pHeader++) = static_cast< sal_Int8 >(( nSize >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nSize >> 8 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nSize >> 16 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nSize >> 24 ) & 0xFF);

    // Then the encryption algorithm
    sal_Int32 nEncAlgID = rData->m_nEncAlg;
    *(pHeader++) = static_cast< sal_Int8 >(( nEncAlgID >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nEncAlgID >> 8 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nEncAlgID >> 16 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nEncAlgID >> 24 ) & 0xFF);

    // Then the checksum algorithm
    sal_Int32 nChecksumAlgID = rData->m_nCheckAlg;
    *(pHeader++) = static_cast< sal_Int8 >(( nChecksumAlgID >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nChecksumAlgID >> 8 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nChecksumAlgID >> 16 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nChecksumAlgID >> 24 ) & 0xFF);

    // Then the derived key size
    sal_Int32 nDerivedKeySize = rData->m_nDerivedKeySize;
    *(pHeader++) = static_cast< sal_Int8 >(( nDerivedKeySize >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nDerivedKeySize >> 8 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nDerivedKeySize >> 16 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nDerivedKeySize >> 24 ) & 0xFF);

    // Then the start key generation algorithm
    sal_Int32 nKeyAlgID = rData->m_nStartKeyGenID;
    *(pHeader++) = static_cast< sal_Int8 >(( nKeyAlgID >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nKeyAlgID >> 8 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nKeyAlgID >> 16 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nKeyAlgID >> 24 ) & 0xFF);

    // Then the salt length
    *(pHeader++) = static_cast< sal_Int8 >(( nSaltLength >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nSaltLength >> 8 ) & 0xFF);

    // Then the IV length
    *(pHeader++) = static_cast< sal_Int8 >(( nIVLength >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nIVLength >> 8 ) & 0xFF);

    // Then the digest length
    *(pHeader++) = static_cast< sal_Int8 >(( nDigestLength >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nDigestLength >> 8 ) & 0xFF);

    // Then the mediatype length
    *(pHeader++) = static_cast< sal_Int8 >(( nMediaTypeLength >> 0 ) & 0xFF);
    *(pHeader++) = static_cast< sal_Int8 >(( nMediaTypeLength >> 8 ) & 0xFF);

    // Then the salt content
    memcpy ( pHeader, rData->m_aSalt.getConstArray(), nSaltLength );
    pHeader += nSaltLength;

    // Then the IV content
    memcpy ( pHeader, rData->m_aInitVector.getConstArray(), nIVLength );
    pHeader += nIVLength;

    // Then the digest content
    memcpy ( pHeader, rData->m_aDigest.getConstArray(), nDigestLength );
    pHeader += nDigestLength;

    // Then the mediatype itself
    memcpy ( pHeader, aMediaType.getStr(), nMediaTypeLength );
    pHeader += nMediaTypeLength;
}

bool ZipFile::StaticFillData (  ::rtl::Reference< BaseEncryptionData > const & rData,
                                    sal_Int32 &rEncAlg,
                                    sal_Int32 &rChecksumAlg,
                                    sal_Int32 &rDerivedKeySize,
                                    sal_Int32 &rStartKeyGenID,
                                    sal_Int32 &rSize,
                                    OUString& aMediaType,
                                    const uno::Reference< XInputStream >& rStream )
{
    bool bOk = false;
    const sal_Int32 nHeaderSize = n_ConstHeaderSize - 4;
    Sequence < sal_Int8 > aBuffer ( nHeaderSize );
    if ( nHeaderSize == rStream->readBytes ( aBuffer, nHeaderSize ) )
    {
        sal_Int16 nPos = 0;
        sal_Int8 *pBuffer = aBuffer.getArray();
        sal_Int16 nVersion = pBuffer[nPos++] & 0xFF;
        nVersion |= ( pBuffer[nPos++] & 0xFF ) << 8;
        if ( nVersion == n_ConstCurrentVersion )
        {
            sal_Int32 nCount = pBuffer[nPos++] & 0xFF;
            nCount |= ( pBuffer[nPos++] & 0xFF ) << 8;
            nCount |= ( pBuffer[nPos++] & 0xFF ) << 16;
            nCount |= ( pBuffer[nPos++] & 0xFF ) << 24;
            rData->m_nIterationCount = nCount;

            rSize  =   pBuffer[nPos++] & 0xFF;
            rSize |= ( pBuffer[nPos++] & 0xFF ) << 8;
            rSize |= ( pBuffer[nPos++] & 0xFF ) << 16;
            rSize |= ( pBuffer[nPos++] & 0xFF ) << 24;

            rEncAlg   =   pBuffer[nPos++] & 0xFF;
            rEncAlg  |= ( pBuffer[nPos++] & 0xFF ) << 8;
            rEncAlg  |= ( pBuffer[nPos++] & 0xFF ) << 16;
            rEncAlg  |= ( pBuffer[nPos++] & 0xFF ) << 24;

            rChecksumAlg   =   pBuffer[nPos++] & 0xFF;
            rChecksumAlg  |= ( pBuffer[nPos++] & 0xFF ) << 8;
            rChecksumAlg  |= ( pBuffer[nPos++] & 0xFF ) << 16;
            rChecksumAlg  |= ( pBuffer[nPos++] & 0xFF ) << 24;

            rDerivedKeySize   =   pBuffer[nPos++] & 0xFF;
            rDerivedKeySize  |= ( pBuffer[nPos++] & 0xFF ) << 8;
            rDerivedKeySize  |= ( pBuffer[nPos++] & 0xFF ) << 16;
            rDerivedKeySize  |= ( pBuffer[nPos++] & 0xFF ) << 24;

            rStartKeyGenID   =   pBuffer[nPos++] & 0xFF;
            rStartKeyGenID  |= ( pBuffer[nPos++] & 0xFF ) << 8;
            rStartKeyGenID  |= ( pBuffer[nPos++] & 0xFF ) << 16;
            rStartKeyGenID  |= ( pBuffer[nPos++] & 0xFF ) << 24;

            sal_Int16 nSaltLength =   pBuffer[nPos++] & 0xFF;
            nSaltLength          |= ( pBuffer[nPos++] & 0xFF ) << 8;
            sal_Int16 nIVLength   = ( pBuffer[nPos++] & 0xFF );
            nIVLength            |= ( pBuffer[nPos++] & 0xFF ) << 8;
            sal_Int16 nDigestLength = pBuffer[nPos++] & 0xFF;
            nDigestLength        |= ( pBuffer[nPos++] & 0xFF ) << 8;

            sal_Int16 nMediaTypeLength = pBuffer[nPos++] & 0xFF;
            nMediaTypeLength |= ( pBuffer[nPos++] & 0xFF ) << 8;

            if ( nSaltLength == rStream->readBytes ( aBuffer, nSaltLength ) )
            {
                rData->m_aSalt.realloc ( nSaltLength );
                memcpy ( rData->m_aSalt.getArray(), aBuffer.getConstArray(), nSaltLength );
                if ( nIVLength == rStream->readBytes ( aBuffer, nIVLength ) )
                {
                    rData->m_aInitVector.realloc ( nIVLength );
                    memcpy ( rData->m_aInitVector.getArray(), aBuffer.getConstArray(), nIVLength );
                    if ( nDigestLength == rStream->readBytes ( aBuffer, nDigestLength ) )
                    {
                        rData->m_aDigest.realloc ( nDigestLength );
                        memcpy ( rData->m_aDigest.getArray(), aBuffer.getConstArray(), nDigestLength );

                        if ( nMediaTypeLength == rStream->readBytes ( aBuffer, nMediaTypeLength ) )
                        {
                            aMediaType = OUString( reinterpret_cast<sal_Unicode const *>(aBuffer.getConstArray()),
                                                            nMediaTypeLength / sizeof( sal_Unicode ) );
                            bOk = true;
                        }
                    }
                }
            }
        }
    }
    return bOk;
}

uno::Reference< XInputStream > ZipFile::StaticGetDataFromRawStream( const rtl::Reference< comphelper::RefCountedMutex >& aMutexHolder,
                                                                const uno::Reference< uno::XComponentContext >& rxContext,
                                                                const uno::Reference< XInputStream >& xStream,
                                                                const ::rtl::Reference< EncryptionData > &rData )
{
    if ( !rData.is() )
        throw ZipIOException("Encrypted stream without encryption data!" );

    if ( !rData->m_aKey.hasElements() )
        throw packages::WrongPasswordException(THROW_WHERE );

    uno::Reference< XSeekable > xSeek( xStream, UNO_QUERY );
    if ( !xSeek.is() )
        throw ZipIOException("The stream must be seekable!" );

    // if we have a digest, then this file is an encrypted one and we should
    // check if we can decrypt it or not
    OSL_ENSURE( rData->m_aDigest.hasElements(), "Can't detect password correctness without digest!" );
    if ( rData->m_aDigest.hasElements() )
    {
        sal_Int32 nSize = sal::static_int_cast< sal_Int32 >( xSeek->getLength() );
        if ( nSize > n_ConstDigestLength + 32 )
            nSize = n_ConstDigestLength + 32;

        // skip header
        xSeek->seek( n_ConstHeaderSize + rData->m_aInitVector.getLength() +
                                rData->m_aSalt.getLength() + rData->m_aDigest.getLength() );

        // Only want to read enough to verify the digest
        Sequence < sal_Int8 > aReadBuffer ( nSize );

        xStream->readBytes( aReadBuffer, nSize );

        if ( !StaticHasValidPassword( rxContext, aReadBuffer, rData ) )
            throw packages::WrongPasswordException(THROW_WHERE );
    }

    return new XUnbufferedStream( aMutexHolder, xStream, rData );
}

#if 0
// for debugging purposes
void CheckSequence( const uno::Sequence< sal_Int8 >& aSequence )
{
    if ( aSequence.getLength() )
    {
        sal_Int32* pPointer = *( (sal_Int32**)&aSequence );
        sal_Int32 nSize = *( pPointer + 1 );
        sal_Int32 nMemSize = *( pPointer - 2 );
        sal_Int32 nUsedMemSize = ( nSize + 4 * sizeof( sal_Int32 ) );
        OSL_ENSURE( nSize == aSequence.getLength() && nUsedMemSize + 7 - ( nUsedMemSize - 1 ) % 8 == nMemSize, "Broken Sequence!" );
    }
}
#endif

bool ZipFile::StaticHasValidPassword( const uno::Reference< uno::XComponentContext >& rxContext, const Sequence< sal_Int8 > &aReadBuffer, const ::rtl::Reference< EncryptionData > &rData )
{
    if ( !rData.is() || !rData->m_aKey.hasElements() )
        return false;

    bool bRet = false;

    uno::Reference< xml::crypto::XCipherContext > xCipher( StaticGetCipher( rxContext, rData, false ), uno::UNO_SET_THROW );

    uno::Sequence< sal_Int8 > aDecryptBuffer;
    uno::Sequence< sal_Int8 > aDecryptBuffer2;
    try
    {
        aDecryptBuffer = xCipher->convertWithCipherContext( aReadBuffer );
        aDecryptBuffer2 = xCipher->finalizeCipherContextAndDispose();
    }
    catch( uno::Exception& )
    {
        // decryption with padding will throw the exception in finalizing if the buffer represent only part of the stream
        // it is no problem, actually this is why we read 32 additional bytes ( two of maximal possible encryption blocks )
    }

    if ( aDecryptBuffer2.hasElements() )
    {
        sal_Int32 nOldLen = aDecryptBuffer.getLength();
        aDecryptBuffer.realloc( nOldLen + aDecryptBuffer2.getLength() );
        memcpy( aDecryptBuffer.getArray() + nOldLen, aDecryptBuffer2.getConstArray(), aDecryptBuffer2.getLength() );
    }

    if ( aDecryptBuffer.getLength() > n_ConstDigestLength )
        aDecryptBuffer.realloc( n_ConstDigestLength );

    uno::Sequence< sal_Int8 > aDigestSeq;
    uno::Reference< xml::crypto::XDigestContext > xDigestContext( StaticGetDigestContextForChecksum( rxContext, rData ), uno::UNO_SET_THROW );

    xDigestContext->updateDigest( aDecryptBuffer );
    aDigestSeq = xDigestContext->finalizeDigestAndDispose();

    // If we don't have a digest, then we have to assume that the password is correct
    if (  rData->m_aDigest.hasElements() &&
          ( aDigestSeq.getLength() != rData->m_aDigest.getLength() ||
            0 != memcmp ( aDigestSeq.getConstArray(),
                                     rData->m_aDigest.getConstArray(),
                                    aDigestSeq.getLength() ) ) )
    {
        // We should probably tell the user that the password they entered was wrong
    }
    else
        bRet = true;

    return bRet;
}

bool ZipFile::hasValidPassword ( ZipEntry const & rEntry, const ::rtl::Reference< EncryptionData >& rData )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    bool bRet = false;
    if ( rData.is() && rData->m_aKey.hasElements() )
    {
        css::uno::Reference < css::io::XSeekable > xSeek(xStream, UNO_QUERY_THROW);
        xSeek->seek( rEntry.nOffset );
        sal_Int64 nSize = rEntry.nMethod == DEFLATED ? rEntry.nCompressedSize : rEntry.nSize;

        // Only want to read enough to verify the digest
        if ( nSize > n_ConstDigestDecrypt )
            nSize = n_ConstDigestDecrypt;

        Sequence < sal_Int8 > aReadBuffer ( nSize );

        xStream->readBytes( aReadBuffer, nSize );

        bRet = StaticHasValidPassword( m_xContext, aReadBuffer, rData );
    }

    return bRet;
}

namespace {

class XBufferedStream : public cppu::WeakImplHelper<css::io::XInputStream, css::io::XSeekable>
{
    std::vector<sal_Int8> maBytes;
    size_t mnPos;

    size_t remainingSize() const
    {
        return maBytes.size() - mnPos;
    }

    bool hasBytes() const
    {
        return mnPos < maBytes.size();
    }

public:
    XBufferedStream( const uno::Reference<XInputStream>& xSrcStream ) : mnPos(0)
    {
        sal_Int32 nRemaining = xSrcStream->available();
        maBytes.reserve(nRemaining);

        if (auto pByteReader = dynamic_cast< comphelper::ByteReader* >( xSrcStream.get() ))
        {
            maBytes.resize(nRemaining);

            sal_Int8* pData = maBytes.data();
            while (nRemaining > 0)
            {
                sal_Int32 nRead = pByteReader->readSomeBytes(pData, nRemaining);
                nRemaining -= nRead;
                pData += nRead;
            }
            return;
        }

        const sal_Int32 nBufSize = 8192;
        uno::Sequence<sal_Int8> aBuf(nBufSize);
        while (nRemaining > 0)
        {
            const sal_Int32 nBytes = xSrcStream->readBytes(aBuf, std::min(nBufSize, nRemaining));
            if (!nBytes)
                break;
            maBytes.insert(maBytes.end(), aBuf.begin(), aBuf.begin() + nBytes);
            nRemaining -= nBytes;
        }
    }

    virtual sal_Int32 SAL_CALL readBytes( uno::Sequence<sal_Int8>& rData, sal_Int32 nBytesToRead ) override
    {
        if (!hasBytes())
            return 0;

        sal_Int32 nReadSize = std::min<sal_Int32>(nBytesToRead, remainingSize());
        rData.realloc(nReadSize);
        auto pData = rData.getArray();
        std::vector<sal_Int8>::const_iterator it = maBytes.cbegin();
        std::advance(it, mnPos);
        for (sal_Int32 i = 0; i < nReadSize; ++i, ++it)
            pData[i] = *it;

        mnPos += nReadSize;

        return nReadSize;
    }

    virtual sal_Int32 SAL_CALL readSomeBytes( ::css::uno::Sequence<sal_Int8>& rData, sal_Int32 nMaxBytesToRead ) override
    {
        return readBytes(rData, nMaxBytesToRead);
    }

    virtual void SAL_CALL skipBytes( sal_Int32 nBytesToSkip ) override
    {
        if (!hasBytes())
            return;

        mnPos += nBytesToSkip;
    }

    virtual sal_Int32 SAL_CALL available() override
    {
        if (!hasBytes())
            return 0;

        return remainingSize();
    }

    virtual void SAL_CALL closeInput() override
    {
    }
    // XSeekable
    virtual void SAL_CALL seek( sal_Int64 location ) override
    {
        if ( location < 0 || o3tl::make_unsigned(location) > maBytes.size() )
            throw IllegalArgumentException(THROW_WHERE, uno::Reference< uno::XInterface >(), 1 );
        mnPos = location;
    }
    virtual sal_Int64 SAL_CALL getPosition() override
    {
        return mnPos;
    }
    virtual sal_Int64 SAL_CALL getLength() override
    {
        return maBytes.size();
    }
};

}

uno::Reference< XInputStream > ZipFile::createStreamForZipEntry(
            const rtl::Reference< comphelper::RefCountedMutex >& aMutexHolder,
            ZipEntry const & rEntry,
            const ::rtl::Reference< EncryptionData > &rData,
            sal_Int8 nStreamMode,
            bool bIsEncrypted,
            const bool bUseBufferedStream,
            const OUString& aMediaType )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    rtl::Reference< XUnbufferedStream > xSrcStream = new XUnbufferedStream(
        m_xContext, aMutexHolder, rEntry, xStream, rData, nStreamMode, bIsEncrypted, aMediaType, bRecoveryMode);

    if (!bUseBufferedStream)
        return xSrcStream;

    uno::Reference<io::XInputStream> xBufStream;
#ifndef EMSCRIPTEN
    static const sal_Int32 nThreadingThreshold = 10000;

    if( xSrcStream->available() > nThreadingThreshold )
        xBufStream = new XBufferedThreadedStream(xSrcStream, xSrcStream->getSize());
    else
#endif
        xBufStream = new XBufferedStream(xSrcStream);

    return xBufStream;
}

ZipEnumeration ZipFile::entries()
{
    return aEntries;
}

uno::Reference< XInputStream > ZipFile::getInputStream( ZipEntry& rEntry,
        const ::rtl::Reference< EncryptionData > &rData,
        bool bIsEncrypted,
        const rtl::Reference<comphelper::RefCountedMutex>& aMutexHolder )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    if ( rEntry.nOffset <= 0 )
        readLOC( rEntry );

    // We want to return a rawStream if we either don't have a key or if the
    // key is wrong

    bool bNeedRawStream = rEntry.nMethod == STORED;

    // if we have a digest, then this file is an encrypted one and we should
    // check if we can decrypt it or not
    if ( bIsEncrypted && rData.is() && rData->m_aDigest.hasElements() )
        bNeedRawStream = !hasValidPassword ( rEntry, rData );

    return createStreamForZipEntry ( aMutexHolder,
                                    rEntry,
                                    rData,
                                    bNeedRawStream ? UNBUFF_STREAM_RAW : UNBUFF_STREAM_DATA,
                                    bIsEncrypted );
}

uno::Reference< XInputStream > ZipFile::getDataStream( ZipEntry& rEntry,
        const ::rtl::Reference< EncryptionData > &rData,
        bool bIsEncrypted,
        const rtl::Reference<comphelper::RefCountedMutex>& aMutexHolder )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    if ( rEntry.nOffset <= 0 )
        readLOC( rEntry );

    // An exception must be thrown in case stream is encrypted and
    // there is no key or the key is wrong
    bool bNeedRawStream = false;
    if ( bIsEncrypted )
    {
        // in case no digest is provided there is no way
        // to detect password correctness
        if ( !rData.is() )
            throw ZipException("Encrypted stream without encryption data!" );

        // if we have a digest, then this file is an encrypted one and we should
        // check if we can decrypt it or not
        OSL_ENSURE( rData->m_aDigest.hasElements(), "Can't detect password correctness without digest!" );
        if ( rData->m_aDigest.hasElements() && !hasValidPassword ( rEntry, rData ) )
                throw packages::WrongPasswordException(THROW_WHERE );
    }
    else
        bNeedRawStream = ( rEntry.nMethod == STORED );

    return createStreamForZipEntry ( aMutexHolder,
                                    rEntry,
                                    rData,
                                    bNeedRawStream ? UNBUFF_STREAM_RAW : UNBUFF_STREAM_DATA,
                                    bIsEncrypted );
}

uno::Reference< XInputStream > ZipFile::getRawData( ZipEntry& rEntry,
        const ::rtl::Reference< EncryptionData >& rData,
        bool bIsEncrypted,
        const rtl::Reference<comphelper::RefCountedMutex>& aMutexHolder,
        const bool bUseBufferedStream )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    if ( rEntry.nOffset <= 0 )
        readLOC( rEntry );

    return createStreamForZipEntry ( aMutexHolder, rEntry, rData, UNBUFF_STREAM_RAW, bIsEncrypted, bUseBufferedStream );
}

uno::Reference< XInputStream > ZipFile::getWrappedRawStream(
        ZipEntry& rEntry,
        const ::rtl::Reference< EncryptionData >& rData,
        const OUString& aMediaType,
        const rtl::Reference<comphelper::RefCountedMutex>& aMutexHolder )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    if ( !rData.is() )
        throw packages::NoEncryptionException(THROW_WHERE );

    if ( rEntry.nOffset <= 0 )
        readLOC( rEntry );

    return createStreamForZipEntry ( aMutexHolder, rEntry, rData, UNBUFF_STREAM_WRAPPEDRAW, true, true, aMediaType );
}

void ZipFile::readLOC( ZipEntry &rEntry )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    sal_Int64 nPos = -rEntry.nOffset;

    aGrabber.seek(nPos);
    sal_Int32 nTestSig = aGrabber.ReadInt32();
    if (nTestSig != LOCSIG)
        throw ZipIOException("Invalid LOC header (bad signature)" );

    // Ignore all (duplicated) information from the local file header.
    // various programs produced "broken" zip files; even LO at some point.
    // Just verify the path and calculate the data offset and otherwise
    // rely on the central directory info.

    aGrabber.ReadInt16(); //version
    aGrabber.ReadInt16(); //flag
    aGrabber.ReadInt16(); //how
    aGrabber.ReadInt32(); //time
    aGrabber.ReadInt32(); //crc
    aGrabber.ReadInt32(); //compressed size
    aGrabber.ReadInt32(); //size
    sal_Int16 nPathLen = aGrabber.ReadInt16();
    sal_Int16 nExtraLen = aGrabber.ReadInt16();

    if (nPathLen < 0)
    {
        SAL_WARN("package", "bogus path len of: " << nPathLen);
        nPathLen = 0;
    }

    rEntry.nOffset = aGrabber.getPosition() + nPathLen + nExtraLen;

    // FIXME64: need to read 64bit LOC

    bool bBroken = false;

    try
    {
        // read always in UTF8, some tools seem not to set UTF8 bit
        // coverity[tainted_data] - we've checked negative lens, and up to max short is ok here
        uno::Sequence<sal_Int8> aNameBuffer(nPathLen);
        sal_Int32 nRead = aGrabber.readBytes(aNameBuffer, nPathLen);
        if (nRead < aNameBuffer.getLength())
            aNameBuffer.realloc(nRead);

        OUString sLOCPath( reinterpret_cast<const char *>(aNameBuffer.getConstArray()),
                           aNameBuffer.getLength(),
                           RTL_TEXTENCODING_UTF8 );

        if ( rEntry.nPathLen == -1 ) // the file was created
        {
            rEntry.nPathLen = nPathLen;
            rEntry.sPath = sLOCPath;
        }

        bBroken = rEntry.nPathLen != nPathLen
                        || rEntry.sPath != sLOCPath;
    }
    catch(...)
    {
        bBroken = true;
    }

    if ( bBroken && !bRecoveryMode )
        throw ZipIOException("The stream seems to be broken!" );
}

sal_Int32 ZipFile::findEND()
{
    // this method is called in constructor only, no need for mutex
    sal_Int32 nPos, nEnd;
    Sequence < sal_Int8 > aBuffer;
    try
    {
        sal_Int32 nLength = static_cast <sal_Int32 > (aGrabber.getLength());
        if (nLength < ENDHDR)
            return -1;
        nPos = nLength - ENDHDR - ZIP_MAXNAMELEN;
        nEnd = nPos >= 0 ? nPos : 0 ;

        aGrabber.seek( nEnd );

        auto nSize = nLength - nEnd;
        if (nSize != aGrabber.readBytes(aBuffer, nSize))
            throw ZipException("Zip END signature not found!" );

        const sal_Int8 *pBuffer = aBuffer.getConstArray();

        nPos = nSize - ENDHDR;
        while ( nPos >= 0 )
        {
            if (pBuffer[nPos] == 'P' && pBuffer[nPos+1] == 'K' && pBuffer[nPos+2] == 5 && pBuffer[nPos+3] == 6 )
                return nPos + nEnd;
            nPos--;
        }
    }
    catch ( IllegalArgumentException& )
    {
        throw ZipException("Zip END signature not found!" );
    }
    catch ( NotConnectedException& )
    {
        throw ZipException("Zip END signature not found!" );
    }
    catch ( BufferSizeExceededException& )
    {
        throw ZipException("Zip END signature not found!" );
    }
    throw ZipException("Zip END signature not found!" );
}

sal_Int32 ZipFile::readCEN()
{
    // this method is called in constructor only, no need for mutex
    sal_Int32 nCenPos = -1, nLocPos;
    sal_uInt16 nCount;

    try
    {
        sal_Int32 nEndPos = findEND();
        if (nEndPos == -1)
            return -1;
        aGrabber.seek(nEndPos + ENDTOT);
        sal_uInt16 nTotal = aGrabber.ReadUInt16();
        sal_Int32 nCenLen = aGrabber.ReadInt32();
        sal_Int32 nCenOff = aGrabber.ReadInt32();

        if ( nTotal * CENHDR > nCenLen )
            throw ZipException("invalid END header (bad entry count)" );

        if ( nTotal > ZIP_MAXENTRIES )
            throw ZipException("too many entries in ZIP File" );

        if ( nCenLen < 0 || nCenLen > nEndPos )
            throw ZipException("Invalid END header (bad central directory size)" );

        nCenPos = nEndPos - nCenLen;

        if ( nCenOff < 0 || nCenOff > nCenPos )
            throw ZipException("Invalid END header (bad central directory size)" );

        nLocPos = nCenPos - nCenOff;
        aGrabber.seek( nCenPos );
        Sequence < sal_Int8 > aCENBuffer ( nCenLen );
        sal_Int64 nRead = aGrabber.readBytes ( aCENBuffer, nCenLen );
        if ( static_cast < sal_Int64 > ( nCenLen ) != nRead )
            throw ZipException ("Error reading CEN into memory buffer!" );

        MemoryByteGrabber aMemGrabber(aCENBuffer);

        ZipEntry aEntry;
        sal_Int16 nCommentLen;

        aEntries.reserve(nTotal);
        for (nCount = 0 ; nCount < nTotal; nCount++)
        {
            sal_Int32 nTestSig = aMemGrabber.ReadInt32();
            if ( nTestSig != CENSIG )
                throw ZipException("Invalid CEN header (bad signature)" );

            aMemGrabber.skipBytes ( 2 );
            aEntry.nVersion = aMemGrabber.ReadInt16();
            aEntry.nFlag = aMemGrabber.ReadInt16();

            if ( ( aEntry.nFlag & 1 ) == 1 )
                throw ZipException("Invalid CEN header (encrypted entry)" );

            aEntry.nMethod = aMemGrabber.ReadInt16();

            if ( aEntry.nMethod != STORED && aEntry.nMethod != DEFLATED)
                throw ZipException("Invalid CEN header (bad compression method)" );

            aEntry.nTime = aMemGrabber.ReadInt32();
            aEntry.nCrc = aMemGrabber.ReadInt32();

            sal_uInt32 nCompressedSize = aMemGrabber.ReadUInt32();
            sal_uInt32 nSize = aMemGrabber.ReadUInt32();
            aEntry.nPathLen = aMemGrabber.ReadInt16();
            aEntry.nExtraLen = aMemGrabber.ReadInt16();
            nCommentLen = aMemGrabber.ReadInt16();
            aMemGrabber.skipBytes ( 8 );
            sal_uInt32 nOffset = aMemGrabber.ReadUInt32();

            // FIXME64: need to read the 64bit header instead
            if ( nSize == 0xffffffff ||
                 nOffset == 0xffffffff ||
                 nCompressedSize == 0xffffffff ) {
                throw ZipException("PK64 zip file entry" );
            }
            aEntry.nCompressedSize = nCompressedSize;
            aEntry.nSize = nSize;
            aEntry.nOffset = nOffset;

            aEntry.nOffset += nLocPos;
            aEntry.nOffset *= -1;

            if ( aEntry.nPathLen < 0 )
                throw ZipException("unexpected name length" );

            if ( nCommentLen < 0 )
                throw ZipException("unexpected comment length" );

            if ( aEntry.nExtraLen < 0 )
                throw ZipException("unexpected extra header info length" );

            if (aEntry.nPathLen > aMemGrabber.remainingSize())
                throw ZipException("name too long");

            // read always in UTF8, some tools seem not to set UTF8 bit
            aEntry.sPath = OUString( reinterpret_cast<char const *>(aMemGrabber.getCurrentPos()),
                                     aEntry.nPathLen,
                                     RTL_TEXTENCODING_UTF8 );

            if ( !::comphelper::OStorageHelper::IsValidZipEntryFileName( aEntry.sPath, true ) )
                throw ZipException("Zip entry has an invalid name." );

            aMemGrabber.skipBytes( aEntry.nPathLen + aEntry.nExtraLen + nCommentLen );
            aEntries[aEntry.sPath] = aEntry;
        }

        if (nCount != nTotal)
            throw ZipException("Count != Total" );
    }
    catch ( IllegalArgumentException & )
    {
        // seek can throw this...
        nCenPos = -1; // make sure we return -1 to indicate an error
    }
    return nCenPos;
}

void ZipFile::recover()
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    sal_Int64 nLength;
    Sequence < sal_Int8 > aBuffer;

    try
    {
        nLength = aGrabber.getLength();
        if (nLength < ENDHDR)
            return;

        aGrabber.seek( 0 );

        const sal_Int64 nToRead = 32000;
        for( sal_Int64 nGenPos = 0; aGrabber.readBytes( aBuffer, nToRead ) && aBuffer.getLength() > 16; )
        {
            const sal_Int8 *pBuffer = aBuffer.getConstArray();
            sal_Int32 nBufSize = aBuffer.getLength();

            sal_Int64 nPos = 0;
            // the buffer should contain at least one header,
            // or if it is end of the file, at least the postheader with sizes and hash
            while( nPos < nBufSize - 30
                || ( nBufSize < nToRead && nPos < nBufSize - 16 ) )

            {
                if ( nPos < nBufSize - 30 && pBuffer[nPos] == 'P' && pBuffer[nPos+1] == 'K' && pBuffer[nPos+2] == 3 && pBuffer[nPos+3] == 4 )
                {
                    ZipEntry aEntry;
                    Sequence<sal_Int8> aTmpBuffer(&(pBuffer[nPos+4]), 26);
                    MemoryByteGrabber aMemGrabber(aTmpBuffer);

                    aEntry.nVersion = aMemGrabber.ReadInt16();
                    aEntry.nFlag = aMemGrabber.ReadInt16();

                    if ( ( aEntry.nFlag & 1 ) != 1 )
                    {
                        aEntry.nMethod = aMemGrabber.ReadInt16();

                        if ( aEntry.nMethod == STORED || aEntry.nMethod == DEFLATED )
                        {
                            aEntry.nTime = aMemGrabber.ReadInt32();
                            aEntry.nCrc = aMemGrabber.ReadInt32();
                            sal_uInt32 nCompressedSize = aMemGrabber.ReadUInt32();
                            sal_uInt32 nSize = aMemGrabber.ReadUInt32();
                            aEntry.nPathLen = aMemGrabber.ReadInt16();
                            aEntry.nExtraLen = aMemGrabber.ReadInt16();

                            // FIXME64: need to read the 64bit header instead
                            if ( nSize == 0xffffffff ||
                                 nCompressedSize == 0xffffffff ) {
                                throw ZipException("PK64 zip file entry" );
                            }
                            aEntry.nCompressedSize = nCompressedSize;
                            aEntry.nSize = nSize;

                            sal_Int32 nDescrLength =
                                ( aEntry.nMethod == DEFLATED && ( aEntry.nFlag & 8 ) ) ? 16 : 0;

                            sal_Int64 nDataSize = ( aEntry.nMethod == DEFLATED ) ? aEntry.nCompressedSize : aEntry.nSize;
                            sal_Int64 nBlockLength = nDataSize + aEntry.nPathLen + aEntry.nExtraLen + 30 + nDescrLength;
                            if ( aEntry.nPathLen >= 0 && aEntry.nExtraLen >= 0
                                && ( nGenPos + nPos + nBlockLength ) <= nLength )
                            {
                                // read always in UTF8, some tools seem not to set UTF8 bit
                                if( nPos + 30 + aEntry.nPathLen <= nBufSize )
                                    aEntry.sPath = OUString ( reinterpret_cast<char const *>(&pBuffer[nPos + 30]),
                                                              aEntry.nPathLen,
                                                              RTL_TEXTENCODING_UTF8 );
                                else
                                {
                                    Sequence < sal_Int8 > aFileName;
                                    aGrabber.seek( nGenPos + nPos + 30 );
                                    aGrabber.readBytes( aFileName, aEntry.nPathLen );
                                    aEntry.sPath = OUString ( reinterpret_cast<const char *>(aFileName.getConstArray()),
                                                              aFileName.getLength(),
                                                              RTL_TEXTENCODING_UTF8 );
                                    aEntry.nPathLen = static_cast< sal_Int16 >(aFileName.getLength());
                                }

                                aEntry.nOffset = nGenPos + nPos + 30 + aEntry.nPathLen + aEntry.nExtraLen;

                                if ( ( aEntry.nSize || aEntry.nCompressedSize ) && !checkSizeAndCRC( aEntry ) )
                                {
                                    aEntry.nCrc = 0;
                                    aEntry.nCompressedSize = 0;
                                    aEntry.nSize = 0;
                                }

                                aEntries.emplace( aEntry.sPath, aEntry );
                            }
                        }
                    }

                    nPos += 4;
                }
                else if (pBuffer[nPos] == 'P' && pBuffer[nPos+1] == 'K' && pBuffer[nPos+2] == 7 && pBuffer[nPos+3] == 8 )
                {
                    sal_Int64 nCompressedSize, nSize;
                    Sequence<sal_Int8> aTmpBuffer(&(pBuffer[nPos+4]), 12);
                    MemoryByteGrabber aMemGrabber(aTmpBuffer);
                    sal_Int32 nCRC32 = aMemGrabber.ReadInt32();
                    sal_uInt32 nCompressedSize32 = aMemGrabber.ReadUInt32();
                    sal_uInt32 nSize32 = aMemGrabber.ReadUInt32();

                    // FIXME64: work to be done here ...
                    nCompressedSize = nCompressedSize32;
                    nSize = nSize32;

                    for( auto& rEntry : aEntries )
                    {
                        // this is a broken package, accept this block not only for DEFLATED streams
                        if( rEntry.second.nFlag & 8 )
                        {
                            sal_Int64 nStreamOffset = nGenPos + nPos - nCompressedSize;
                            if ( nStreamOffset == rEntry.second.nOffset && nCompressedSize > rEntry.second.nCompressedSize )
                            {
                                // only DEFLATED blocks need to be checked
                                bool bAcceptBlock = ( rEntry.second.nMethod == STORED && nCompressedSize == nSize );

                                if ( !bAcceptBlock )
                                {
                                    sal_Int64 nRealSize = 0;
                                    sal_Int32 nRealCRC = 0;
                                    getSizeAndCRC( nStreamOffset, nCompressedSize, &nRealSize, &nRealCRC );
                                    bAcceptBlock = ( nRealSize == nSize && nRealCRC == nCRC32 );
                                }

                                if ( bAcceptBlock )
                                {
                                    rEntry.second.nCrc = nCRC32;
                                    rEntry.second.nCompressedSize = nCompressedSize;
                                    rEntry.second.nSize = nSize;
                                }
                            }
#if 0
// for now ignore clearly broken streams
                            else if( !rEntry.second.nCompressedSize )
                            {
                                rEntry.second.nCrc = nCRC32;
                                sal_Int32 nRealStreamSize = nGenPos + nPos - rEntry.second.nOffset;
                                rEntry.second.nCompressedSize = nRealStreamSize;
                                rEntry.second.nSize = nSize;
                            }
#endif
                        }
                    }

                    nPos += 4;
                }
                else
                    nPos++;
            }

            nGenPos += nPos;
            aGrabber.seek( nGenPos );
        }
    }
    catch ( IllegalArgumentException& )
    {
        throw ZipException("Zip END signature not found!" );
    }
    catch ( NotConnectedException& )
    {
        throw ZipException("Zip END signature not found!" );
    }
    catch ( BufferSizeExceededException& )
    {
        throw ZipException("Zip END signature not found!" );
    }
}

bool ZipFile::checkSizeAndCRC( const ZipEntry& aEntry )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    sal_Int32 nCRC = 0;
    sal_Int64 nSize = 0;

    if( aEntry.nMethod == STORED )
        return ( getCRC( aEntry.nOffset, aEntry.nSize ) == aEntry.nCrc );

    getSizeAndCRC( aEntry.nOffset, aEntry.nCompressedSize, &nSize, &nCRC );
    return ( aEntry.nSize == nSize && aEntry.nCrc == nCRC );
}

sal_Int32 ZipFile::getCRC( sal_Int64 nOffset, sal_Int64 nSize )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    Sequence < sal_Int8 > aBuffer;
    CRC32 aCRC;
    sal_Int64 nBlockSize = ::std::min(nSize, static_cast< sal_Int64 >(32000));

    aGrabber.seek( nOffset );
    for (sal_Int64 ind = 0;
         aGrabber.readBytes( aBuffer, nBlockSize ) && ind * nBlockSize < nSize;
         ++ind)
    {
        sal_Int64 nLen = ::std::min(nBlockSize, nSize - ind * nBlockSize);
        aCRC.updateSegment(aBuffer, static_cast<sal_Int32>(nLen));
    }

    return aCRC.getValue();
}

void ZipFile::getSizeAndCRC( sal_Int64 nOffset, sal_Int64 nCompressedSize, sal_Int64 *nSize, sal_Int32 *nCRC )
{
    ::osl::MutexGuard aGuard( m_aMutexHolder->GetMutex() );

    Sequence < sal_Int8 > aBuffer;
    CRC32 aCRC;
    sal_Int64 nRealSize = 0;
    Inflater aInflaterLocal( true );
    sal_Int32 nBlockSize = static_cast< sal_Int32 > (::std::min( nCompressedSize, static_cast< sal_Int64 >( 32000 ) ) );

    aGrabber.seek( nOffset );
    for ( sal_Int64 ind = 0;
          !aInflaterLocal.finished() && aGrabber.readBytes( aBuffer, nBlockSize ) && ind * nBlockSize < nCompressedSize;
          ind++ )
    {
        Sequence < sal_Int8 > aData( nBlockSize );
        sal_Int32 nLastInflated = 0;
        sal_Int64 nInBlock = 0;

        aInflaterLocal.setInput( aBuffer );
        do
        {
            nLastInflated = aInflaterLocal.doInflateSegment( aData, 0, nBlockSize );
            aCRC.updateSegment( aData, nLastInflated );
            nInBlock += nLastInflated;
        } while( !aInflater.finished() && nLastInflated );

        nRealSize += nInBlock;
    }

    *nSize = nRealSize;
    *nCRC = aCRC.getValue();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
