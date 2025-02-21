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

#include <fmtrfmrk.hxx>

#include <libxml/xmlwriter.h>

#include <hintids.hxx>
#include <hints.hxx>
#include <txtrfmrk.hxx>
#include <unorefmark.hxx>
#include <utility>
#include <sfx2/viewsh.hxx>
#include <tools/json_writer.hxx>
#include <LibreOfficeKit/LibreOfficeKitEnums.h>
#include <comphelper/lok.hxx>
#include <doc.hxx>
#include <ndtxt.hxx>

SwFormatRefMark::~SwFormatRefMark( )
{
}

SwFormatRefMark::SwFormatRefMark( OUString aName )
    : SfxPoolItem(RES_TXTATR_REFMARK)
    , sw::BroadcastingModify()
    , m_pTextAttr(nullptr)
    , m_aRefName(std::move(aName))
{
}

SwFormatRefMark::SwFormatRefMark( const SwFormatRefMark& rAttr )
    : SfxPoolItem(RES_TXTATR_REFMARK)
    , sw::BroadcastingModify()
    , m_pTextAttr(nullptr)
    , m_aRefName(rAttr.m_aRefName)
{
}

void SwFormatRefMark::SetXRefMark(rtl::Reference<SwXReferenceMark> const& xMark)
{ m_wXReferenceMark = xMark.get(); }

bool SwFormatRefMark::operator==( const SfxPoolItem& rAttr ) const
{
    assert(SfxPoolItem::operator==(rAttr));
    return m_aRefName == static_cast<const SwFormatRefMark&>(rAttr).m_aRefName;
}

SwFormatRefMark* SwFormatRefMark::Clone( SfxItemPool* ) const
{
    return new SwFormatRefMark( *this );
}

void SwFormatRefMark::SwClientNotify(const SwModify&, const SfxHint& rHint)
{
    if (rHint.GetId() != SfxHintId::SwLegacyModify)
        return;
    auto pLegacy = static_cast<const sw::LegacyModifyHint*>(&rHint);
    CallSwClientNotify(rHint);
    if(RES_REMOVE_UNO_OBJECT == pLegacy->GetWhich())
        SetXRefMark(nullptr);
}

void SwFormatRefMark::InvalidateRefMark()
{
    SwPtrMsgPoolItem const item(RES_REMOVE_UNO_OBJECT,
            &static_cast<sw::BroadcastingModify&>(*this)); // cast to base class (void*)
    CallSwClientNotify(sw::LegacyModifyHint(&item, &item));
}

void SwFormatRefMark::dumpAsXml(xmlTextWriterPtr pWriter) const
{
    (void)xmlTextWriterStartElement(pWriter, BAD_CAST("SwFormatRefMark"));
    (void)xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST("ptr"), "%p", this);
    (void)xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST("m_pTextAttr"), "%p", m_pTextAttr);
    (void)xmlTextWriterWriteAttribute(pWriter, BAD_CAST("ref-name"),
                                      BAD_CAST(m_aRefName.toUtf8().getStr()));
    SfxPoolItem::dumpAsXml(pWriter);


    (void)xmlTextWriterEndElement(pWriter);
}

// attribute for content references in the text

SwTextRefMark::SwTextRefMark( SwFormatRefMark& rAttr,
            sal_Int32 const nStartPos, sal_Int32 const*const pEnd)
    : SwTextAttr(rAttr, nStartPos)
    , SwTextAttrEnd( rAttr, nStartPos, nStartPos )
    , m_pTextNode( nullptr )
    , m_pEnd( nullptr )
{
    rAttr.m_pTextAttr = this;
    if ( pEnd )
    {
        m_nEnd = *pEnd;
        m_pEnd = & m_nEnd;
    }
    else
    {
        SetHasDummyChar(true);
    }
    SetDontMoveAttr( true );
    SetOverlapAllowedAttr( true );
}

SwTextRefMark::~SwTextRefMark()
{
    if (!comphelper::LibreOfficeKit::isActive() || GetTextNode().GetDoc().IsClipBoard())
        return;

    SfxViewShell* pViewShell = SfxViewShell::Current();
    if (!pViewShell)
        return;

    OUString fieldCommand = GetRefMark().GetRefName();
    tools::JsonWriter aJson;
    aJson.put("commandName", ".uno:DeleteField");
    aJson.put("success", true);
    {
        auto result = aJson.startNode("result");
        aJson.put("DeleteField", fieldCommand);
    }

    pViewShell->libreOfficeKitViewCallback(LOK_CALLBACK_UNO_COMMAND_RESULT, aJson.extractData());
}

const sal_Int32* SwTextRefMark::GetEnd() const
{
    return m_pEnd;
}

void SwTextRefMark::SetEnd(sal_Int32 n)
{
    *m_pEnd = n;
    if (m_pHints)
        m_pHints->EndPosChanged();
}

void SwTextRefMark::dumpAsXml(xmlTextWriterPtr pWriter) const
{
    (void)xmlTextWriterStartElement(pWriter, BAD_CAST("SwTextRefMark"));
    (void)xmlTextWriterWriteFormatAttribute(pWriter, BAD_CAST("ptr"), "%p", this);
    SwTextAttr::dumpAsXml(pWriter);

    (void)xmlTextWriterEndElement(pWriter);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
