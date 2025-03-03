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


#include <memory>
#include <vcl/image.hxx>

#include <com/sun/star/i18n/WordType.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>

#include <i18nlangtag/languagetag.hxx>
#include <i18nlangtag/mslangid.hxx>
#include <svl/languageoptions.hxx>
#include <svtools/ctrltool.hxx>
#include <svtools/langtab.hxx>
#include <tools/stream.hxx>

#include <svl/srchitem.hxx>

#include "impedit.hxx"
#include <comphelper/propertyvalue.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editview.hxx>
#include <editeng/flditem.hxx>
#include <editeng/svxacorr.hxx>
#include <editeng/langitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/eerdll.hxx>
#include <eerdll2.hxx>
#include <editeng/editrids.hrc>
#include <editeng.hxx>
#include <i18nlangtag/lang.h>
#include <vcl/window.hxx>
#include <editeng/acorrcfg.hxx>
#include <editeng/unolingu.hxx>
#include <unotools/lingucfg.hxx>

#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/linguistic2/XDictionary.hpp>
#include <com/sun/star/linguistic2/XSearchableDictionaryList.hpp>
#include <linguistic/lngprops.hxx>
#include <vcl/settings.hxx>
#include <vcl/svapp.hxx>
#include <LibreOfficeKit/LibreOfficeKitEnums.h>
#include <comphelper/lok.hxx>
#include <sfx2/viewsh.hxx>
#include <osl/diagnose.h>
#include <boost/property_tree/json_parser.hpp>

#include <com/sun/star/lang/XServiceInfo.hpp>

using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;


// static
LanguageType EditView::CheckLanguage(
        const OUString &rText,
        const Reference< linguistic2::XSpellChecker1 >& xSpell,
        const Reference< linguistic2::XLanguageGuessing >& xLangGuess,
        bool bIsParaText )
{
    LanguageType nLang = LANGUAGE_NONE;
    if (bIsParaText)    // check longer texts with language-guessing...
    {
        if (!xLangGuess.is())
            return nLang;

        LanguageTag aGuessTag( xLangGuess->guessPrimaryLanguage( rText, 0, rText.getLength()) );

        // If the result from language guessing does not provide a 'Country'
        // part, try to get it by looking up the locale setting of the office,
        // "Tools/Options - Language Settings - Languages: Locale setting", if
        // the language matches.
        if ( aGuessTag.getCountry().isEmpty() )
        {
            const LanguageTag& rAppLocaleTag = Application::GetSettings().GetLanguageTag();
            if (rAppLocaleTag.getLanguage() == aGuessTag.getLanguage())
                nLang = rAppLocaleTag.getLanguageType();
        }
        if (nLang == LANGUAGE_NONE) // language not found by looking up the system language...
            nLang = aGuessTag.makeFallback().getLanguageType();     // best known locale match
        if (nLang == LANGUAGE_SYSTEM)
            nLang = Application::GetSettings().GetLanguageTag().getLanguageType();
        if (nLang == LANGUAGE_DONTKNOW)
            nLang = LANGUAGE_NONE;
    }
    else    // check single word
    {
        if (!xSpell.is())
            return nLang;


        // build list of languages to check

        LanguageType aLangList[4];
        const AllSettings& rSettings  = Application::GetSettings();
        SvtLinguOptions aLinguOpt;
        SvtLinguConfig().GetOptions( aLinguOpt );
        // The default document language from "Tools/Options - Language Settings - Languages: Western"
        aLangList[0] = MsLangId::resolveSystemLanguageByScriptType( aLinguOpt.nDefaultLanguage,
                css::i18n::ScriptType::LATIN);
        // The one from "Tools/Options - Language Settings - Languages: User interface"
        aLangList[1] = rSettings.GetUILanguageTag().getLanguageType();
        // The one from "Tools/Options - Language Settings - Languages: Locale setting"
        aLangList[2] = rSettings.GetLanguageTag().getLanguageType();
        // en-US
        aLangList[3] = LANGUAGE_ENGLISH_US;
#ifdef DEBUG
        lang::Locale a0( LanguageTag::convertToLocale( aLangList[0] ) );
        lang::Locale a1( LanguageTag::convertToLocale( aLangList[1] ) );
        lang::Locale a2( LanguageTag::convertToLocale( aLangList[2] ) );
        lang::Locale a3( LanguageTag::convertToLocale( aLangList[3] ) );
#endif

        for (const LanguageType& nTmpLang : aLangList)
        {
            if (nTmpLang != LANGUAGE_NONE  &&  nTmpLang != LANGUAGE_DONTKNOW)
            {
                if (xSpell->hasLanguage( static_cast<sal_uInt16>(nTmpLang) ) &&
                    xSpell->isValid( rText, static_cast<sal_uInt16>(nTmpLang), Sequence< PropertyValue >() ))
                {
                    nLang = nTmpLang;
                    break;
                }
            }
        }
    }

    return nLang;
}

EditViewCallbacks::~EditViewCallbacks()
{
}

EditView::EditView( EditEngine* pEng, vcl::Window* pWindow )
{
    pImpEditView.reset( new ImpEditView( this, pEng, pWindow ) );
}

EditView::~EditView()
{
}

void EditView::setEditViewCallbacks(EditViewCallbacks* pEditViewCallbacks)
{
    pImpEditView->setEditViewCallbacks(pEditViewCallbacks);
}

EditViewCallbacks* EditView::getEditViewCallbacks() const
{
    return pImpEditView->getEditViewCallbacks();
}

ImpEditEngine* EditView::GetImpEditEngine() const
{
    return pImpEditView->pEditEngine->pImpEditEngine.get();
}

EditEngine* EditView::GetEditEngine() const
{
    return pImpEditView->pEditEngine;
}

tools::Rectangle EditView::GetInvalidateRect() const
{
    if ( !pImpEditView->DoInvalidateMore() )
        return pImpEditView->aOutArea;
    else
    {
        tools::Rectangle aRect( pImpEditView->aOutArea );
        tools::Long nMore = pImpEditView->GetOutputDevice().PixelToLogic( Size( pImpEditView->GetInvalidateMore(), 0 ) ).Width();
        aRect.AdjustLeft( -nMore );
        aRect.AdjustRight(nMore );
        aRect.AdjustTop( -nMore );
        aRect.AdjustBottom(nMore );
        return aRect;
    }
}

namespace {

tools::Rectangle lcl_negateRectX(const tools::Rectangle& rRect)
{
    return tools::Rectangle(-rRect.Right(), rRect.Top(), -rRect.Left(), rRect.Bottom());
}

}

void EditView::InvalidateWindow(const tools::Rectangle& rClipRect)
{
    bool bNegativeX = IsNegativeX();
    if (EditViewCallbacks* pEditViewCallbacks = pImpEditView->getEditViewCallbacks())
    {
        // do not invalidate and trigger a global repaint, but forward
        // the need for change to the applied EditViewCallback, can e.g.
        // be used to visualize the active edit text in an OverlayObject
        pEditViewCallbacks->EditViewInvalidate(bNegativeX ? lcl_negateRectX(rClipRect) : rClipRect);
    }
    else
    {
        // classic mode: invalidate and trigger full repaint
        // of the changed area
        GetWindow()->Invalidate(bNegativeX ? lcl_negateRectX(rClipRect) : rClipRect);
    }
}

void EditView::InvalidateOtherViewWindows( const tools::Rectangle& rInvRect )
{
    if (comphelper::LibreOfficeKit::isActive())
    {
        bool bNegativeX = IsNegativeX();
        for (auto& pWin : pImpEditView->aOutWindowSet)
        {
            if (pWin)
                pWin->Invalidate( bNegativeX ? lcl_negateRectX(rInvRect) : rInvRect );
        }
    }
}

void EditView::Invalidate()
{
    const tools::Rectangle& rInvRect = GetInvalidateRect();
    pImpEditView->InvalidateAtWindow(rInvRect);
    InvalidateOtherViewWindows(rInvRect);
}

void EditView::SetReadOnly( bool bReadOnly )
{
    pImpEditView->bReadOnly = bReadOnly;
}

bool EditView::IsReadOnly() const
{
    return pImpEditView->bReadOnly;
}

void EditView::SetSelection( const ESelection& rESel )
{
    // If someone has just left an empty attribute, and then the outliner manipulates the
    // selection, call the CursorMoved method so that empty attributes get cleaned up.
    if ( !HasSelection() )
    {
        // tdf#113591 Get node from EditDoc, as the selection might have a pointer to an
        // already deleted node.
        const ContentNode* pNode = pImpEditView->pEditEngine->GetEditDoc().GetEndPaM().GetNode();
        pImpEditView->pEditEngine->CursorMoved( pNode );
    }
    EditSelection aNewSelection( pImpEditView->pEditEngine->pImpEditEngine->ConvertSelection(
                                            rESel.nStartPara, rESel.nStartPos, rESel.nEndPara, rESel.nEndPos ) );

    // If the selection is manipulated after a KeyInput:
    pImpEditView->pEditEngine->CheckIdleFormatter();

    // Selection may not start/end at an invisible paragraph:
    const ParaPortion* pPortion = pImpEditView->pEditEngine->FindParaPortion( aNewSelection.Min().GetNode() );
    if ( !pPortion->IsVisible() )
    {
        pPortion = pImpEditView->pEditEngine->GetPrevVisPortion( pPortion );
        ContentNode* pNode = pPortion ? pPortion->GetNode() : pImpEditView->pEditEngine->GetEditDoc().GetObject( 0 );
        aNewSelection.Min() = EditPaM( pNode, pNode->Len() );
    }
    pPortion = pImpEditView->pEditEngine->FindParaPortion( aNewSelection.Max().GetNode() );
    if ( !pPortion->IsVisible() )
    {
        pPortion = pImpEditView->pEditEngine->GetPrevVisPortion( pPortion );
        ContentNode* pNode = pPortion ? pPortion->GetNode() : pImpEditView->pEditEngine->GetEditDoc().GetObject( 0 );
        aNewSelection.Max() = EditPaM( pNode, pNode->Len() );
    }

    pImpEditView->DrawSelectionXOR();
    pImpEditView->SetEditSelection( aNewSelection );
    pImpEditView->DrawSelectionXOR();
    bool bGotoCursor = pImpEditView->DoAutoScroll();

    // comments section in Writer:
    // don't scroll to the selection if it is
    // out of visible area of comment canvas.
    if (HasSelection())
        ShowCursor( bGotoCursor );
}

ESelection EditView::GetSelection() const
{
    ESelection aSelection;

    aSelection.nStartPara = pImpEditView->pEditEngine->GetEditDoc().GetPos( pImpEditView->GetEditSelection().Min().GetNode() );
    aSelection.nEndPara = pImpEditView->pEditEngine->GetEditDoc().GetPos( pImpEditView->GetEditSelection().Max().GetNode() );

    aSelection.nStartPos = pImpEditView->GetEditSelection().Min().GetIndex();
    aSelection.nEndPos = pImpEditView->GetEditSelection().Max().GetIndex();

    return aSelection;
}

bool EditView::HasSelection() const
{
    return pImpEditView->HasSelection();
}

bool EditView::IsSelectionAtPoint(const Point& rPointPixel)
{
    return pImpEditView->IsSelectionAtPoint(rPointPixel);
}

void EditView::DeleteSelected()
{
    pImpEditView->DeleteSelected();
}

SvtScriptType EditView::GetSelectedScriptType() const
{
    return pImpEditView->pEditEngine->GetScriptType( pImpEditView->GetEditSelection() );
}

void EditView::GetSelectionRectangles(std::vector<tools::Rectangle>& rLogicRects) const
{
    return pImpEditView->GetSelectionRectangles(pImpEditView->GetEditSelection(), rLogicRects);
}

void EditView::Paint( const tools::Rectangle& rRect, OutputDevice* pTargetDevice )
{
    pImpEditView->pEditEngine->pImpEditEngine->Paint( pImpEditView.get(), rRect, pTargetDevice );
}

void EditView::SetEditEngine( EditEngine* pEditEng )
{
    pImpEditView->pEditEngine = pEditEng;
    EditSelection aStartSel = pImpEditView->pEditEngine->GetEditDoc().GetStartPaM();
    pImpEditView->SetEditSelection( aStartSel );
}

void EditView::SetWindow( vcl::Window* pWin )
{
    pImpEditView->pOutWin = pWin;
    pImpEditView->pEditEngine->pImpEditEngine->GetSelEngine().Reset();
}

vcl::Window* EditView::GetWindow() const
{
    return pImpEditView->pOutWin;
}

OutputDevice& EditView::GetOutputDevice() const
{
    return pImpEditView->GetOutputDevice();
}

LanguageType EditView::GetInputLanguage() const
{
    // it might make sense to add this to getEditViewCallbacks
    if (const vcl::Window* pWindow = GetWindow())
        return pWindow->GetInputLanguage();
    return LANGUAGE_DONTKNOW;
}

bool EditView::HasOtherViewWindow( vcl::Window* pWin )
{
    OutWindowSet& rOutWindowSet = pImpEditView->aOutWindowSet;
    auto found = std::find(rOutWindowSet.begin(), rOutWindowSet.end(), pWin);
    return (found != rOutWindowSet.end());
}

bool EditView::AddOtherViewWindow( vcl::Window* pWin )
{
    if (HasOtherViewWindow(pWin))
        return false;
    pImpEditView->aOutWindowSet.emplace_back(pWin);
    return true;
}

bool EditView::RemoveOtherViewWindow( vcl::Window* pWin )
{
    OutWindowSet& rOutWindowSet = pImpEditView->aOutWindowSet;
    auto found = std::find(rOutWindowSet.begin(), rOutWindowSet.end(), pWin);
    if (found == rOutWindowSet.end())
        return false;
    rOutWindowSet.erase(found);
    return true;
}

void EditView::SetVisArea( const tools::Rectangle& rRect )
{
    pImpEditView->SetVisDocStartPos( rRect.TopLeft() );
}

tools::Rectangle EditView::GetVisArea() const
{
    return pImpEditView->GetVisDocArea();
}

void EditView::SetOutputArea( const tools::Rectangle& rRect )
{
    pImpEditView->SetOutputArea( rRect );

    // the rest here only if it is an API call:
    pImpEditView->CalcAnchorPoint();
    if ( pImpEditView->pEditEngine->pImpEditEngine->GetStatus().AutoPageSize() )
        pImpEditView->RecalcOutputArea();
    pImpEditView->ShowCursor( false, false );
}

const tools::Rectangle& EditView::GetOutputArea() const
{
    return pImpEditView->GetOutputArea();
}

PointerStyle EditView::GetPointer() const
{
    return pImpEditView->GetPointer();
}

vcl::Cursor* EditView::GetCursor() const
{
    return pImpEditView->pCursor.get();
}

void EditView::InsertText( const OUString& rStr, bool bSelect, bool bLOKShowSelect )
{

    EditEngine* pEE = pImpEditView->pEditEngine;

    if (bLOKShowSelect)
        pImpEditView->DrawSelectionXOR();

    EditPaM aPaM1;
    if ( bSelect )
    {
        EditSelection aTmpSel( pImpEditView->GetEditSelection() );
        aTmpSel.Adjust( pEE->GetEditDoc() );
        aPaM1 = aTmpSel.Min();
    }

    pEE->UndoActionStart( EDITUNDO_INSERT );
    EditPaM aPaM2( pEE->InsertText( pImpEditView->GetEditSelection(), rStr ) );
    pEE->UndoActionEnd();

    if ( bSelect )
    {
        DBG_ASSERT( !aPaM1.DbgIsBuggy( pEE->GetEditDoc() ), "Insert: PaM broken" );
        pImpEditView->SetEditSelection( EditSelection( aPaM1, aPaM2 ) );
    }
    else
        pImpEditView->SetEditSelection( EditSelection( aPaM2, aPaM2 ) );

    if (bLOKShowSelect)
        pEE->FormatAndLayout( this );
}

bool EditView::PostKeyEvent( const KeyEvent& rKeyEvent, vcl::Window const * pFrameWin )
{
    return pImpEditView->PostKeyEvent( rKeyEvent, pFrameWin );
}

bool EditView::MouseButtonUp( const MouseEvent& rMouseEvent )
{
    return pImpEditView->MouseButtonUp( rMouseEvent );
}

void EditView::ReleaseMouse()
{
    return pImpEditView->ReleaseMouse();
}

bool EditView::MouseButtonDown( const MouseEvent& rMouseEvent )
{
    return pImpEditView->MouseButtonDown( rMouseEvent );
}

bool EditView::MouseMove( const MouseEvent& rMouseEvent )
{
    return pImpEditView->MouseMove( rMouseEvent );
}

bool EditView::Command(const CommandEvent& rCEvt)
{
    return pImpEditView->Command(rCEvt);
}

void EditView::SetBroadcastLOKViewCursor(bool bSet)
{
    pImpEditView->SetBroadcastLOKViewCursor(bSet);
}

tools::Rectangle EditView::GetEditCursor() const
{
    return pImpEditView->GetEditCursor();
}

void EditView::ShowCursor( bool bGotoCursor, bool bForceVisCursor, bool bActivate )
{
    if ( !pImpEditView->pEditEngine->HasView( this ) )
        return;

    // The control word is more important:
    if ( !pImpEditView->DoAutoScroll() )
        bGotoCursor = false;
    pImpEditView->ShowCursor( bGotoCursor, bForceVisCursor );

    if (pImpEditView->mpViewShell && !bActivate)
    {
        if (!pImpEditView->pOutWin)
            return;
        VclPtr<vcl::Window> pParent = pImpEditView->pOutWin->GetParentWithLOKNotifier();
        if (pParent && pParent->GetLOKWindowId() != 0)
            return;

        static const OString aPayload = OString::boolean(true);
        pImpEditView->mpViewShell->libreOfficeKitViewCallback(LOK_CALLBACK_CURSOR_VISIBLE, aPayload.getStr());
        pImpEditView->mpViewShell->NotifyOtherViews(LOK_CALLBACK_VIEW_CURSOR_VISIBLE, "visible", aPayload);
    }
}

void EditView::HideCursor(bool bDeactivate)
{
    pImpEditView->GetCursor()->Hide();

    if (pImpEditView->mpViewShell && !bDeactivate)
    {
        if (!pImpEditView->pOutWin)
            return;
        VclPtr<vcl::Window> pParent = pImpEditView->pOutWin->GetParentWithLOKNotifier();
        if (pParent && pParent->GetLOKWindowId() != 0)
            return;

        OString aPayload = OString::boolean(false);
        pImpEditView->mpViewShell->libreOfficeKitViewCallback(LOK_CALLBACK_CURSOR_VISIBLE, aPayload.getStr());
        pImpEditView->mpViewShell->NotifyOtherViews(LOK_CALLBACK_VIEW_CURSOR_VISIBLE, "visible", aPayload);
    }
}

Pair EditView::Scroll( tools::Long ndX, tools::Long ndY, ScrollRangeCheck nRangeCheck )
{
    return pImpEditView->Scroll( ndX, ndY, nRangeCheck );
}

const SfxItemSet& EditView::GetEmptyItemSet() const
{
    return pImpEditView->pEditEngine->GetEmptyItemSet();
}

void EditView::SetAttribs( const SfxItemSet& rSet )
{
    DBG_ASSERT( !pImpEditView->aEditSelection.IsInvalid(), "Blind Selection in..." );

    pImpEditView->DrawSelectionXOR();
    pImpEditView->pEditEngine->SetAttribs( pImpEditView->GetEditSelection(), rSet, SetAttribsMode::WholeWord );
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pImpEditView->pEditEngine->FormatAndLayout( this );
}

void EditView::RemoveAttribsKeepLanguages( bool bRemoveParaAttribs )
{

    pImpEditView->DrawSelectionXOR();
    pImpEditView->pEditEngine->UndoActionStart( EDITUNDO_RESETATTRIBS );
    EditSelection aSelection( pImpEditView->GetEditSelection() );

    for (sal_uInt16 nWID = EE_ITEMS_START; nWID <= EE_ITEMS_END; ++nWID)
    {
        bool bIsLang =  EE_CHAR_LANGUAGE     == nWID ||
                        EE_CHAR_LANGUAGE_CJK == nWID ||
                        EE_CHAR_LANGUAGE_CTL == nWID;
        if (!bIsLang)
            pImpEditView->pEditEngine->RemoveCharAttribs( aSelection, bRemoveParaAttribs, nWID );
    }

    pImpEditView->pEditEngine->UndoActionEnd();
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pImpEditView->pEditEngine->FormatAndLayout( this );
}

void EditView::RemoveAttribs( bool bRemoveParaAttribs, sal_uInt16 nWhich )
{
    RemoveAttribs(bRemoveParaAttribs ? EERemoveParaAttribsMode::RemoveAll
            : EERemoveParaAttribsMode::RemoveCharItems, nWhich);
}

void EditView::RemoveAttribs( EERemoveParaAttribsMode eMode, sal_uInt16 nWhich )
{
    pImpEditView->DrawSelectionXOR();
    pImpEditView->pEditEngine->UndoActionStart( EDITUNDO_RESETATTRIBS );
    pImpEditView->pEditEngine->RemoveCharAttribs( pImpEditView->GetEditSelection(), eMode, nWhich  );
    pImpEditView->pEditEngine->UndoActionEnd();
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pImpEditView->pEditEngine->FormatAndLayout( this );
}

void EditView::RemoveCharAttribs( sal_Int32 nPara, sal_uInt16 nWhich )
{
    pImpEditView->pEditEngine->UndoActionStart( EDITUNDO_RESETATTRIBS );
    pImpEditView->pEditEngine->RemoveCharAttribs( nPara, nWhich );
    pImpEditView->pEditEngine->UndoActionEnd();
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pImpEditView->pEditEngine->FormatAndLayout( this );
}

SfxItemSet EditView::GetAttribs()
{
    DBG_ASSERT( !pImpEditView->aEditSelection.IsInvalid(), "Blind Selection in..." );
    return pImpEditView->pEditEngine->pImpEditEngine->GetAttribs( pImpEditView->GetEditSelection() );
}

void EditView::Undo()
{
    pImpEditView->pEditEngine->Undo( this );
}

void EditView::Redo()
{
    pImpEditView->pEditEngine->Redo( this );
}

ErrCode EditView::Read( SvStream& rInput, EETextFormat eFormat, SvKeyValueIterator* pHTTPHeaderAttrs )
{
    EditSelection aOldSel( pImpEditView->GetEditSelection() );
    pImpEditView->DrawSelectionXOR();
    pImpEditView->pEditEngine->pImpEditEngine->UndoActionStart( EDITUNDO_READ );
    EditPaM aEndPaM = pImpEditView->pEditEngine->pImpEditEngine->Read( rInput, "", eFormat, aOldSel, pHTTPHeaderAttrs );
    pImpEditView->pEditEngine->pImpEditEngine->UndoActionEnd();
    EditSelection aNewSel( aEndPaM, aEndPaM );

    pImpEditView->SetEditSelection( aNewSel );
    bool bGotoCursor = pImpEditView->DoAutoScroll();
    ShowCursor( bGotoCursor );

    return rInput.GetError();
}

void EditView::Cut()
{
    Reference<css::datatransfer::clipboard::XClipboard> aClipBoard(GetClipboard());
    pImpEditView->CutCopy( aClipBoard, true );
}

Reference<css::datatransfer::clipboard::XClipboard> EditView::GetClipboard() const
{
    return pImpEditView->GetClipboard();
}

css::uno::Reference< css::datatransfer::XTransferable > EditView::GetTransferable() const
{
    uno::Reference< datatransfer::XTransferable > xData =
        GetEditEngine()->CreateTransferable( pImpEditView->GetEditSelection() );
    return xData;
}

void EditView::Copy()
{
    Reference<css::datatransfer::clipboard::XClipboard> aClipBoard(GetClipboard());
    pImpEditView->CutCopy( aClipBoard, false );
}

void EditView::Paste()
{
    Reference<css::datatransfer::clipboard::XClipboard> aClipBoard(GetClipboard());
    pImpEditView->Paste( aClipBoard );
}

void EditView::PasteSpecial()
{
    Reference<css::datatransfer::clipboard::XClipboard> aClipBoard(GetClipboard());
    pImpEditView->Paste(aClipBoard, true );
}

Point EditView::GetWindowPosTopLeft( sal_Int32 nParagraph )
{
    Point aDocPos( pImpEditView->pEditEngine->GetDocPosTopLeft( nParagraph ) );
    return pImpEditView->GetWindowPos( aDocPos );
}

void EditView::SetSelectionMode( EESelectionMode eMode )
{
    pImpEditView->SetSelectionMode( eMode );
}

OUString EditView::GetSelected() const
{
    return pImpEditView->pEditEngine->pImpEditEngine->GetSelected( pImpEditView->GetEditSelection() );
}

void EditView::MoveParagraphs( Range aParagraphs, sal_Int32 nNewPos )
{
    pImpEditView->pEditEngine->pImpEditEngine->UndoActionStart( EDITUNDO_MOVEPARAS );
    pImpEditView->pEditEngine->pImpEditEngine->MoveParagraphs( aParagraphs, nNewPos, this );
    pImpEditView->pEditEngine->pImpEditEngine->UndoActionEnd();
}

void EditView::MoveParagraphs( tools::Long nDiff )
{
    ESelection aSel = GetSelection();
    Range aRange( aSel.nStartPara, aSel.nEndPara );
    aRange.Normalize();
    tools::Long nDest = ( nDiff > 0  ? aRange.Max() : aRange.Min() ) + nDiff;
    if ( nDiff > 0 )
        nDest++;
    DBG_ASSERT( ( nDest >= 0 ) && ( nDest <= pImpEditView->pEditEngine->GetParagraphCount() ), "MoveParagraphs - wrong Parameters!" );
    MoveParagraphs( aRange, sal::static_int_cast< sal_Int32 >( nDest ) );
}

void EditView::SetBackgroundColor( const Color& rColor )
{
    pImpEditView->SetBackgroundColor( rColor );
}

Color const & EditView::GetBackgroundColor() const
{
    return pImpEditView->GetBackgroundColor();
}

void EditView::RegisterViewShell(OutlinerViewShell* pViewShell)
{
    pImpEditView->RegisterViewShell(pViewShell);
}

void EditView::RegisterOtherShell(OutlinerViewShell* pOtherShell)
{
    pImpEditView->RegisterOtherShell(pOtherShell);
}

void EditView::SetControlWord( EVControlBits nWord )
{
    pImpEditView->nControl = nWord;
}

EVControlBits EditView::GetControlWord() const
{
    return pImpEditView->nControl;
}

std::unique_ptr<EditTextObject> EditView::CreateTextObject()
{
    return pImpEditView->pEditEngine->pImpEditEngine->CreateTextObject( pImpEditView->GetEditSelection() );
}

void EditView::InsertText( const EditTextObject& rTextObject )
{
    pImpEditView->DrawSelectionXOR();

    pImpEditView->pEditEngine->UndoActionStart( EDITUNDO_INSERT );
    EditSelection aTextSel( pImpEditView->pEditEngine->InsertText( rTextObject, pImpEditView->GetEditSelection() ) );
    pImpEditView->pEditEngine->UndoActionEnd();

    aTextSel.Min() = aTextSel.Max();    // Selection not retained.
    pImpEditView->SetEditSelection( aTextSel );
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pImpEditView->pEditEngine->FormatAndLayout( this );
}

void EditView::InsertText( css::uno::Reference< css::datatransfer::XTransferable > const & xDataObj, const OUString& rBaseURL, bool bUseSpecial )
{
    pImpEditView->pEditEngine->UndoActionStart( EDITUNDO_INSERT );
    pImpEditView->DeleteSelected();
    EditSelection aTextSel =
        pImpEditView->pEditEngine->InsertText(xDataObj, rBaseURL, pImpEditView->GetEditSelection().Max(), bUseSpecial);
    pImpEditView->pEditEngine->UndoActionEnd();

    aTextSel.Min() = aTextSel.Max();    // Selection not retained.
    pImpEditView->SetEditSelection( aTextSel );
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pImpEditView->pEditEngine->FormatAndLayout( this );
}

bool EditView::SetEditEngineUpdateLayout( bool bUpdate )
{
    return pImpEditView->pEditEngine->pImpEditEngine->SetUpdateLayout( bUpdate, this );
}

void EditView::ForceLayoutCalculation()
{
    pImpEditView->pEditEngine->pImpEditEngine->SetUpdateLayout( true, this, true );
}

SfxStyleSheet* EditView::GetStyleSheet()
{
    EditSelection aSel( pImpEditView->GetEditSelection() );
    aSel.Adjust( pImpEditView->pEditEngine->GetEditDoc() );
    sal_Int32 nStartPara = pImpEditView->pEditEngine->GetEditDoc().GetPos( aSel.Min().GetNode() );
    sal_Int32 nEndPara = pImpEditView->pEditEngine->GetEditDoc().GetPos( aSel.Max().GetNode() );

    SfxStyleSheet* pStyle = nullptr;
    for ( sal_Int32 n = nStartPara; n <= nEndPara; n++ )
    {
        SfxStyleSheet* pTmpStyle = pImpEditView->pEditEngine->GetStyleSheet( n );
        if ( ( n != nStartPara ) && ( pStyle != pTmpStyle ) )
            return nullptr;    // Not unique.
        pStyle = pTmpStyle;
    }
    return pStyle;
}

const SfxStyleSheet* EditView::GetStyleSheet() const
{
    return const_cast< EditView* >( this )->GetStyleSheet();
}

bool EditView::IsInsertMode() const
{
    return pImpEditView->IsInsertMode();
}

void EditView::SetInsertMode( bool bInsert )
{
    pImpEditView->SetInsertMode( bInsert );
}

void EditView::SetAnchorMode( EEAnchorMode eMode )
{
    pImpEditView->SetAnchorMode( eMode );
}

EEAnchorMode EditView::GetAnchorMode() const
{
    return pImpEditView->GetAnchorMode();
}

void EditView::TransliterateText( TransliterationFlags nTransliterationMode )
{
    EditSelection aOldSel( pImpEditView->GetEditSelection() );
    EditSelection aNewSel = pImpEditView->pEditEngine->TransliterateText( pImpEditView->GetEditSelection(), nTransliterationMode );
    if ( aNewSel != aOldSel )
    {
        pImpEditView->DrawSelectionXOR();
        pImpEditView->SetEditSelection( aNewSel );
        pImpEditView->DrawSelectionXOR();
    }
}

void EditView::CompleteAutoCorrect( vcl::Window const * pFrameWin )
{
    if ( !HasSelection() && pImpEditView->pEditEngine->pImpEditEngine->GetStatus().DoAutoCorrect() )
    {
        pImpEditView->DrawSelectionXOR();
        EditSelection aSel = pImpEditView->GetEditSelection();
        aSel = pImpEditView->pEditEngine->EndOfWord( aSel.Max() );
        aSel = pImpEditView->pEditEngine->pImpEditEngine->AutoCorrect( aSel, 0, !IsInsertMode(), pFrameWin );
        pImpEditView->SetEditSelection( aSel );
        if ( pImpEditView->pEditEngine->IsModified() )
            pImpEditView->pEditEngine->FormatAndLayout( this );
    }
}

EESpellState EditView::StartSpeller(weld::Widget* pDialogParent, bool bMultipleDoc)
{
    if ( !pImpEditView->pEditEngine->pImpEditEngine->GetSpeller().is() )
        return EESpellState::NoSpeller;

    return pImpEditView->pEditEngine->pImpEditEngine->Spell(this, pDialogParent, bMultipleDoc);
}

EESpellState EditView::StartThesaurus(weld::Widget* pDialogParent)
{
    if ( !pImpEditView->pEditEngine->pImpEditEngine->GetSpeller().is() )
        return EESpellState::NoSpeller;

    return pImpEditView->pEditEngine->pImpEditEngine->StartThesaurus(this, pDialogParent);
}

void EditView::StartTextConversion(weld::Widget* pDialogParent,
        LanguageType nSrcLang, LanguageType nDestLang, const vcl::Font *pDestFont,
        sal_Int32 nOptions, bool bIsInteractive, bool bMultipleDoc )
{
    pImpEditView->pEditEngine->pImpEditEngine->Convert(this, pDialogParent, nSrcLang, nDestLang, pDestFont, nOptions, bIsInteractive, bMultipleDoc);
}

sal_Int32 EditView::StartSearchAndReplace( const SvxSearchItem& rSearchItem )
{
    return pImpEditView->pEditEngine->pImpEditEngine->StartSearchAndReplace( this, rSearchItem );
}

bool EditView::IsCursorAtWrongSpelledWord()
{
    bool bIsWrong = false;
    if ( !HasSelection() )
    {
        EditPaM aPaM = pImpEditView->GetEditSelection().Max();
        bIsWrong = pImpEditView->IsWrongSpelledWord( aPaM, false/*bMarkIfWrong*/ );
    }
    return bIsWrong;
}

bool EditView::IsWrongSpelledWordAtPos( const Point& rPosPixel, bool bMarkIfWrong )
{
    Point aPos(pImpEditView->GetOutputDevice().PixelToLogic(rPosPixel));
    aPos = pImpEditView->GetDocPos( aPos );
    EditPaM aPaM = pImpEditView->pEditEngine->GetPaM(aPos, false);
    return pImpEditView->IsWrongSpelledWord( aPaM , bMarkIfWrong );
}

static void LOKSendSpellPopupMenu(const weld::Menu& rMenu, LanguageType nGuessLangWord,
                                  LanguageType nGuessLangPara, sal_uInt16 nSuggestions)
{
    if (!comphelper::LibreOfficeKit::isActive())
        return;

    // Generate the menu structure and send it to the client code.
    SfxViewShell* pViewShell = SfxViewShell::Current();
    if (!pViewShell)
        return;

    boost::property_tree::ptree aMenu;

    boost::property_tree::ptree aItemTree;
    if (nSuggestions)
    {
        for(int i = 0; i < nSuggestions; ++i)
        {
            OString sItemId = OString::number(MN_ALTSTART + i);
            OUString sText = rMenu.get_label(sItemId);
            aItemTree.put("text", sText.toUtf8().getStr());
            aItemTree.put("type", "command");
            OUString sCommandString = ".uno:SpellCheckApplySuggestion?ApplyRule:string=Spelling_" + sText;
            aItemTree.put("command", sCommandString.toUtf8().getStr());
            aItemTree.put("enabled", rMenu.get_sensitive(sItemId));
            aMenu.push_back(std::make_pair("", aItemTree));
            aItemTree.clear();
        }

        aItemTree.put("type", "separator");
        aMenu.push_back(std::make_pair("", aItemTree));
        aItemTree.clear();
    }

    // First we need to set item commands for the context menu.
    OUString aTmpWord( SvtLanguageTable::GetLanguageString( nGuessLangWord ) );
    OUString aTmpPara( SvtLanguageTable::GetLanguageString( nGuessLangPara ) );

    aItemTree.put("text", rMenu.get_label("ignore").toUtf8().getStr());
    aItemTree.put("type", "command");
    aItemTree.put("command", ".uno:SpellCheckIgnoreAll?Type:string=Spelling");
    aItemTree.put("enabled", rMenu.get_sensitive("ignore"));
    aMenu.push_back(std::make_pair("", aItemTree));
    aItemTree.clear();

    aItemTree.put("type", "separator");
    aMenu.push_back(std::make_pair("", aItemTree));
    aItemTree.clear();

    aItemTree.put("text", rMenu.get_label("wordlanguage").toUtf8().getStr());
    aItemTree.put("type", "command");
    OUString sCommandString = ".uno:LanguageStatus?Language:string=Current_" + aTmpWord;
    aItemTree.put("command", sCommandString.toUtf8().getStr());
    aItemTree.put("enabled", rMenu.get_sensitive("wordlanguage"));
    aMenu.push_back(std::make_pair("", aItemTree));
    aItemTree.clear();

    aItemTree.put("text", rMenu.get_label("paralanguage").toUtf8().getStr());
    aItemTree.put("type", "command");
    sCommandString = ".uno:LanguageStatus?Language:string=Paragraph_" + aTmpPara;
    aItemTree.put("command", sCommandString.toUtf8().getStr());
    aItemTree.put("enabled", rMenu.get_sensitive("paralanguage"));
    aMenu.push_back(std::make_pair("", aItemTree));
    aItemTree.clear();

    boost::property_tree::ptree aRoot;
    aRoot.add_child("menu", aMenu);

    std::stringstream aStream;
    boost::property_tree::write_json(aStream, aRoot, true);
    pViewShell->libreOfficeKitViewCallback(LOK_CALLBACK_CONTEXT_MENU, aStream.str().c_str());
}

bool EditView::ExecuteSpellPopup(const Point& rPosPixel, const Link<SpellCallbackInfo&,void> &rCallBack)
{
    OutputDevice& rDevice = pImpEditView->GetOutputDevice();
    Point aPos(rDevice.PixelToLogic(rPosPixel));
    aPos = pImpEditView->GetDocPos( aPos );
    EditPaM aPaM = pImpEditView->pEditEngine->GetPaM(aPos, false);
    Reference< linguistic2::XSpellChecker1 >  xSpeller( pImpEditView->pEditEngine->pImpEditEngine->GetSpeller() );
    ESelection aOldSel = GetSelection();
    if ( !(xSpeller.is() && pImpEditView->IsWrongSpelledWord( aPaM, true )) )
        return false;

    // PaMtoEditCursor returns Logical units
    tools::Rectangle aTempRect = pImpEditView->pEditEngine->pImpEditEngine->PaMtoEditCursor( aPaM, GetCursorFlags::TextOnly );
    // GetWindowPos works in Logical units
    aTempRect = pImpEditView->GetWindowPos(aTempRect);
    // Convert to pixels
    aTempRect = rDevice.LogicToPixel(aTempRect);

    weld::Widget* pPopupParent = pImpEditView->GetPopupParent(aTempRect);
    std::unique_ptr<weld::Builder> xBuilder(Application::CreateBuilder(pPopupParent, "editeng/ui/spellmenu.ui"));
    std::unique_ptr<weld::Menu> xPopupMenu(xBuilder->weld_menu("editviewspellmenu"));
    std::unique_ptr<weld::Menu> xInsertMenu(xBuilder->weld_menu("insertmenu")); // add word to user-dictionaries
    std::unique_ptr<weld::Menu> xAutoMenu(xBuilder->weld_menu("automenu"));

    EditPaM aPaM2( aPaM );
    aPaM2.SetIndex( aPaM2.GetIndex()+1 );

    // Are there any replace suggestions?
    OUString aSelected( GetSelected() );

    // restrict the maximal number of suggestions displayed
    // in the context menu.
    // Note: That could of course be done by clipping the
    // resulting sequence but the current third party
    // implementations result differs greatly if the number of
    // suggestions to be returned gets changed. Statistically
    // it gets much better if told to return e.g. only 7 strings
    // than returning e.g. 16 suggestions and using only the
    // first 7. Thus we hand down the value to use to that
    // implementation here by providing an additional parameter.
    Sequence< PropertyValue > aPropVals { comphelper::makePropertyValue(UPN_MAX_NUMBER_OF_SUGGESTIONS, sal_Int16(7)) };

    // Are there any replace suggestions?
    Reference< linguistic2::XSpellAlternatives >  xSpellAlt =
            xSpeller->spell( aSelected, static_cast<sal_uInt16>(pImpEditView->pEditEngine->pImpEditEngine->GetLanguage( aPaM2 ).nLang), aPropVals );

    Reference< linguistic2::XLanguageGuessing >  xLangGuesser( EditDLL::Get().GetGlobalData()->GetLanguageGuesser() );

    // check if text might belong to a different language...
    LanguageType nGuessLangWord = LANGUAGE_NONE;
    LanguageType nGuessLangPara = LANGUAGE_NONE;
    if (xSpellAlt.is() && xLangGuesser.is())
    {
        OUString aParaText;
        ContentNode *pNode = aPaM.GetNode();
        if (pNode)
        {
            aParaText = pNode->GetString();
        }
        else
        {
            OSL_FAIL( "content node is NULL" );
        }

        nGuessLangWord = CheckLanguage( xSpellAlt->getWord(), xSpeller, xLangGuesser, false );
        nGuessLangPara = CheckLanguage( aParaText, xSpeller, xLangGuesser, true );
    }
    if (nGuessLangWord != LANGUAGE_NONE || nGuessLangPara != LANGUAGE_NONE)
    {
        // make sure LANGUAGE_NONE gets not used as menu entry
        if (nGuessLangWord == LANGUAGE_NONE)
            nGuessLangWord = nGuessLangPara;
        if (nGuessLangPara == LANGUAGE_NONE)
            nGuessLangPara = nGuessLangWord;

        xPopupMenu->append_separator("separator1");
        OUString aTmpWord( SvtLanguageTable::GetLanguageString( nGuessLangWord ) );
        OUString aTmpPara( SvtLanguageTable::GetLanguageString( nGuessLangPara ) );
        OUString aWordStr( EditResId( RID_STR_WORD ) );
        aWordStr = aWordStr.replaceFirst( "%x", aTmpWord );
        OUString aParaStr( EditResId( RID_STR_PARAGRAPH ) );
        aParaStr = aParaStr.replaceFirst( "%x", aTmpPara );
        xPopupMenu->append("wordlanguage", aWordStr);
        xPopupMenu->append("paralanguage", aParaStr);
    }

    // Replace suggestions...
    Sequence< OUString > aAlt;
    if (xSpellAlt.is())
        aAlt = xSpellAlt->getAlternatives();
    const OUString *pAlt = aAlt.getConstArray();
    sal_uInt16 nWords = static_cast<sal_uInt16>(aAlt.getLength());
    if ( nWords )
    {
        for ( sal_uInt16 nW = 0; nW < nWords; nW++ )
        {
            OUString aAlternate( pAlt[nW] );
            xPopupMenu->append(OUString::number(MN_ALTSTART + nW), aAlternate);
            xAutoMenu->append(OUString::number(MN_AUTOSTART + nW), aAlternate);
        }
        xPopupMenu->append_separator("separator2");
    }
    else
    {
        xAutoMenu.reset();
        xPopupMenu->remove("autocorrect");
    }

    SvtLinguConfig aCfg;

    Reference< linguistic2::XSearchableDictionaryList >  xDicList( LinguMgr::GetDictionaryList() );
    Sequence< Reference< linguistic2::XDictionary >  > aDics;
    if (xDicList.is())
    {
        const Reference< linguistic2::XDictionary >  *pDic = nullptr;
        // add the default positive dictionary to dic-list (if not already done).
        // This is to ensure that there is at least one dictionary to which
        // words could be added.
        uno::Reference< linguistic2::XDictionary >  xDic( LinguMgr::GetStandardDic() );
        if (xDic.is())
            xDic->setActive( true );

        aDics = xDicList->getDictionaries();
        pDic  = aDics.getConstArray();
        LanguageType nCheckedLanguage = pImpEditView->pEditEngine->pImpEditEngine->GetLanguage( aPaM2 ).nLang;
        sal_uInt16 nDicCount = static_cast<sal_uInt16>(aDics.getLength());
        for (sal_uInt16 i = 0; i < nDicCount; i++)
        {
            uno::Reference< linguistic2::XDictionary >  xDicTmp = pDic[i];
            if (!xDicTmp.is() || LinguMgr::GetIgnoreAllList() == xDicTmp)
                continue;

            uno::Reference< frame::XStorable > xStor( xDicTmp, uno::UNO_QUERY );
            LanguageType nActLanguage = LanguageTag( xDicTmp->getLocale() ).getLanguageType();
            if( xDicTmp->isActive()
                &&  xDicTmp->getDictionaryType() != linguistic2::DictionaryType_NEGATIVE
                && (nCheckedLanguage == nActLanguage || LANGUAGE_NONE == nActLanguage )
                && (!xStor.is() || !xStor->isReadonly()) )
            {
                OUString sImage;

                uno::Reference< lang::XServiceInfo > xSvcInfo( xDicTmp, uno::UNO_QUERY );
                if (xSvcInfo.is())
                {
                    OUString aDictionaryImageUrl( aCfg.GetSpellAndGrammarContextDictionaryImage(
                            xSvcInfo->getImplementationName()) );
                    if (!aDictionaryImageUrl.isEmpty() )
                        sImage = aDictionaryImageUrl;
                }

                if (sImage.isEmpty())
                {
                    xInsertMenu->append(OUString::number(MN_DICTSTART + i), xDicTmp->getName());
                }
                else
                {
                    Image aImage(sImage);
                    ScopedVclPtr<VirtualDevice> xVirDev(pPopupParent->create_virtual_device());
                    Size aSize(aImage.GetSizePixel());
                    xVirDev->SetOutputSizePixel(aSize);
                    xVirDev->DrawImage(Point(0, 0), aImage);
                    xInsertMenu->append(OUString::number(MN_DICTSTART + i), xDicTmp->getName(), *xVirDev);
                }
                aDicNameSingle = xDicTmp->getName();
            }
        }
    }

    if (xInsertMenu->n_children() != 1)
        xPopupMenu->remove("add");
    if (xInsertMenu->n_children() < 2)
    {
        xInsertMenu.reset();
        xPopupMenu->remove("insert");
    }

    //tdf#106123 store and restore the EditPaM around the menu Execute
    //because the loss of focus in the current editeng causes writer
    //annotations to save their contents, making the pContent of the
    //current EditPams invalid
    EPaM aP = pImpEditView->pEditEngine->pImpEditEngine->CreateEPaM(aPaM);
    EPaM aP2 = pImpEditView->pEditEngine->pImpEditEngine->CreateEPaM(aPaM2);

    if (comphelper::LibreOfficeKit::isActive())
    {
        xPopupMenu->remove("autocorrect");
        xPopupMenu->remove("autocorrectdlg");

        LOKSendSpellPopupMenu(*xPopupMenu, nGuessLangWord, nGuessLangPara, nWords);
        return true;
    }

    OString sId = xPopupMenu->popup_at_rect(pPopupParent, aTempRect);

    aPaM2 = pImpEditView->pEditEngine->pImpEditEngine->CreateEditPaM(aP2);
    aPaM = pImpEditView->pEditEngine->pImpEditEngine->CreateEditPaM(aP);

    if (sId == "ignore")
    {
        OUString aWord = pImpEditView->SpellIgnoreWord();
        SpellCallbackInfo aInf( SpellCallbackCommand::IGNOREWORD, aWord );
        rCallBack.Call(aInf);
        SetSelection( aOldSel );
    }
    else if (sId == "wordlanguage" || sId == "paralanguage")
    {
        LanguageType nLangToUse = (sId == "wordlanguage") ? nGuessLangWord : nGuessLangPara;
        SvtScriptType nScriptType = SvtLanguageOptions::GetScriptTypeOfLanguage( nLangToUse );

        SfxItemSet aAttrs = GetEditEngine()->GetEmptyItemSet();
        if (nScriptType == SvtScriptType::LATIN)
            aAttrs.Put( SvxLanguageItem( nLangToUse, EE_CHAR_LANGUAGE ) );
        if (nScriptType == SvtScriptType::COMPLEX)
            aAttrs.Put( SvxLanguageItem( nLangToUse, EE_CHAR_LANGUAGE_CTL ) );
        if (nScriptType == SvtScriptType::ASIAN)
            aAttrs.Put( SvxLanguageItem( nLangToUse, EE_CHAR_LANGUAGE_CJK ) );
        if (sId == "paralanguage")
        {
            ESelection aSel = GetSelection();
            aSel.nStartPos = 0;
            aSel.nEndPos = EE_TEXTPOS_ALL;
            SetSelection( aSel );
        }
        SetAttribs( aAttrs );
        pImpEditView->pEditEngine->pImpEditEngine->StartOnlineSpellTimer();

        SpellCallbackInfo aInf((sId == "wordlanguage") ? SpellCallbackCommand::WORDLANGUAGE : SpellCallbackCommand::PARALANGUAGE);
        rCallBack.Call(aInf);
        SetSelection( aOldSel );
    }
    else if (sId == "check")
    {
        SpellCallbackInfo aInf( SpellCallbackCommand::STARTSPELLDLG, OUString() );
        rCallBack.Call(aInf);
    }
    else if (sId == "autocorrectdlg")
    {
        SpellCallbackInfo aInf( SpellCallbackCommand::AUTOCORRECT_OPTIONS, OUString() );
        rCallBack.Call(aInf);
    }
    else if ( sId.toInt32() >= MN_DICTSTART || sId == "add")
    {
        OUString aDicName;
        if (sId.toInt32() >= MN_DICTSTART)
        {
            assert(xInsertMenu && "this case only occurs when xInsertMenu exists");
            // strip_mnemonic is necessary to retrieve the correct dictionary name
            aDicName = pPopupParent->strip_mnemonic(xInsertMenu->get_label(sId));
        }
        else
            aDicName = aDicNameSingle;

        uno::Reference< linguistic2::XDictionary >      xDic;
        if (xDicList.is())
            xDic = xDicList->getDictionaryByName( aDicName );

        if (xDic.is())
            xDic->add( aSelected, false, OUString() );
        // save modified user-dictionary if it is persistent
        Reference< frame::XStorable >  xSavDic( xDic, UNO_QUERY );
        if (xSavDic.is())
            xSavDic->store();

        aPaM.GetNode()->GetWrongList()->ResetInvalidRange(0, aPaM.GetNode()->Len());
        pImpEditView->pEditEngine->pImpEditEngine->StartOnlineSpellTimer();

        SpellCallbackInfo aInf( SpellCallbackCommand::ADDTODICTIONARY, aSelected );
        rCallBack.Call(aInf);
        SetSelection( aOldSel );
    }
    else if ( sId.toInt32() >= MN_AUTOSTART )
    {
        DBG_ASSERT(sId.toInt32() - MN_AUTOSTART < aAlt.getLength(), "index out of range");
        OUString aWord = pAlt[sId.toInt32() - MN_AUTOSTART];
        SvxAutoCorrect* pAutoCorrect = SvxAutoCorrCfg::Get().GetAutoCorrect();
        if ( pAutoCorrect )
            pAutoCorrect->PutText( aSelected, aWord, pImpEditView->pEditEngine->pImpEditEngine->GetLanguage( aPaM2 ).nLang );
        InsertText( aWord );
    }
    else if ( sId.toInt32() >= MN_ALTSTART )  // Replace
    {
        DBG_ASSERT(sId.toInt32() - MN_ALTSTART < aAlt.getLength(), "index out of range");
        OUString aWord = pAlt[sId.toInt32() - MN_ALTSTART];
        InsertText( aWord );
    }
    else
    {
        SetSelection( aOldSel );
    }
    return true;
}

OUString EditView::SpellIgnoreWord()
{
    return pImpEditView->SpellIgnoreWord();
}

void EditView::SelectCurrentWord( sal_Int16 nWordType )
{
    EditSelection aCurSel( pImpEditView->GetEditSelection() );
    pImpEditView->DrawSelectionXOR();
    aCurSel = pImpEditView->pEditEngine->SelectWord(aCurSel.Max(), nWordType);
    pImpEditView->SetEditSelection( aCurSel );
    pImpEditView->DrawSelectionXOR();
    ShowCursor( true, false );
}

void EditView::InsertParaBreak()
{
    pImpEditView->pEditEngine->UndoActionStart(EDITUNDO_INSERT);
    pImpEditView->DeleteSelected();
    EditPaM aPaM(pImpEditView->pEditEngine->InsertParaBreak(pImpEditView->GetEditSelection()));
    pImpEditView->pEditEngine->UndoActionEnd();
    pImpEditView->SetEditSelection(EditSelection(aPaM, aPaM));
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pImpEditView->pEditEngine->FormatAndLayout(this);
}

void EditView::InsertField( const SvxFieldItem& rFld )
{
    EditEngine* pEE = pImpEditView->pEditEngine;
    pImpEditView->DrawSelectionXOR();
    pEE->UndoActionStart( EDITUNDO_INSERT );
    EditPaM aPaM( pEE->InsertField( pImpEditView->GetEditSelection(), rFld ) );
    pEE->UndoActionEnd();
    pImpEditView->SetEditSelection( EditSelection( aPaM, aPaM ) );
    pEE->UpdateFields();
    if (pImpEditView->pEditEngine->IsUpdateLayout())
        pEE->FormatAndLayout( this );
}

const SvxFieldItem* EditView::GetFieldUnderMousePointer() const
{
    sal_Int32 nPara;
    sal_Int32 nPos;
    return GetFieldUnderMousePointer( nPara, nPos );
}

const SvxFieldItem* EditView::GetField( const Point& rPos, sal_Int32* pPara, sal_Int32* pPos ) const
{
    return pImpEditView->GetField( rPos, pPara, pPos );
}

const SvxFieldItem* EditView::GetFieldUnderMousePointer( sal_Int32& nPara, sal_Int32& nPos ) const
{
    Point aPos;
    if (EditViewCallbacks* pEditViewCallbacks = pImpEditView->getEditViewCallbacks())
        aPos = pEditViewCallbacks->EditViewPointerPosPixel();
    else
        aPos = pImpEditView->GetWindow()->GetPointerPosPixel();
    OutputDevice& rDevice = pImpEditView->GetOutputDevice();
    aPos = rDevice.PixelToLogic(aPos);
    return GetField( aPos, &nPara, &nPos );
}

const SvxFieldItem* EditView::GetFieldAtSelection() const
{
    EditSelection aSel( pImpEditView->GetEditSelection() );
    aSel.Adjust( pImpEditView->pEditEngine->GetEditDoc() );
    // Only when cursor is in font of field, no selection,
    // or only selecting field
    if ( ( aSel.Min().GetNode() == aSel.Max().GetNode() ) &&
         ( ( aSel.Max().GetIndex() == aSel.Min().GetIndex() ) ||
           ( aSel.Max().GetIndex() == aSel.Min().GetIndex()+1 ) ) )
    {
        EditPaM aPaM = aSel.Min();
        const CharAttribList::AttribsType& rAttrs = aPaM.GetNode()->GetCharAttribs().GetAttribs();
        const sal_Int32 nXPos = aPaM.GetIndex();
        for (size_t nAttr = rAttrs.size(); nAttr; )
        {
            const EditCharAttrib& rAttr = *rAttrs[--nAttr];
            if (rAttr.GetStart() == nXPos)
                if (rAttr.Which() == EE_FEATURE_FIELD)
                {
                    DBG_ASSERT(dynamic_cast<const SvxFieldItem* >(rAttr.GetItem() ) != nullptr, "No FieldItem...");
                    return static_cast<const SvxFieldItem*>(rAttr.GetItem());
                }
        }
    }
    return nullptr;
}

void EditView::SelectFieldAtCursor()
{
    const SvxFieldItem* pFieldItem = GetFieldAtSelection();
    if (pFieldItem)
    {
        // Make sure the whole field is selected
        ESelection aSel = GetSelection();
        if (aSel.nStartPos == aSel.nEndPos)
        {
            aSel.nEndPos++;
            SetSelection(aSel);
        }
    }
    if (!pFieldItem)
    {
        // Cursor probably behind the field - extend selection to select the field
        ESelection aSel = GetSelection();
        if (aSel.nStartPos > 0 && aSel.nStartPos == aSel.nEndPos)
        {
            aSel.nStartPos--;
            SetSelection(aSel);
        }
    }
}

const SvxFieldData* EditView::GetFieldAtCursor() const
{
    const SvxFieldItem* pFieldItem = GetFieldUnderMousePointer();
    if (!pFieldItem)
        pFieldItem = GetFieldAtSelection();

    return pFieldItem ? pFieldItem->GetField() : nullptr;
}

sal_Int32 EditView::countFieldsOffsetSum(sal_Int32 nPara, sal_Int32 nPos, bool bCanOverflow) const
{
    if (!pImpEditView || !pImpEditView->pEditEngine)
        return 0;

    int nOffset = 0;

    for (int nCurrentPara = 0; nCurrentPara <= nPara; nCurrentPara++)
    {
        int nFields = pImpEditView->pEditEngine->GetFieldCount( nCurrentPara );
        for (int nField = 0; nField < nFields; nField++)
        {
            EFieldInfo aFieldInfo
                = pImpEditView->pEditEngine->GetFieldInfo( nCurrentPara, nField );

            bool bLastPara = nCurrentPara == nPara;
            sal_Int32 nFieldPos = aFieldInfo.aPosition.nIndex;

            if (bLastPara && nFieldPos >= nPos)
                break;

            sal_Int32 nFieldLen = aFieldInfo.aCurrentText.getLength();

            // position in the middle of a field
            if (!bCanOverflow && bLastPara && nFieldPos + nFieldLen > nPos)
                nFieldLen = nPos - nFieldPos;

            nOffset += nFieldLen - 1;
        }
    }

    return nOffset;
}

sal_Int32 EditView::GetPosNoField(sal_Int32 nPara, sal_Int32 nPos) const
{
    sal_Int32 nOffset = countFieldsOffsetSum(nPara, nPos, false);
    assert(nPos >= nOffset);
    return nPos - nOffset;
}

sal_Int32 EditView::GetPosWithField(sal_Int32 nPara, sal_Int32 nPos) const
{
    sal_Int32 nOffset = countFieldsOffsetSum(nPara, nPos, true);
    return nPos + nOffset;
}

void EditView::SetInvalidateMore( sal_uInt16 nPixel )
{
    pImpEditView->SetInvalidateMore( nPixel );
}

sal_uInt16 EditView::GetInvalidateMore() const
{
    return pImpEditView->GetInvalidateMore();
}

static void ChangeFontSizeImpl( EditView* pEditView, bool bGrow, const ESelection& rSel, const FontList* pFontList )
{
    pEditView->SetSelection( rSel );

    SfxItemSet aSet( pEditView->GetAttribs() );
    if( EditView::ChangeFontSize( bGrow, aSet, pFontList ) )
    {
        SfxItemSet aNewSet( pEditView->GetEmptyItemSet() );
        aNewSet.Put( aSet.Get( EE_CHAR_FONTHEIGHT ) );
        aNewSet.Put( aSet.Get( EE_CHAR_FONTHEIGHT_CJK ) );
        aNewSet.Put( aSet.Get( EE_CHAR_FONTHEIGHT_CTL ) );
        pEditView->SetAttribs( aNewSet );
    }
}

void EditView::ChangeFontSize( bool bGrow, const FontList* pFontList )
{

    EditEngine& rEditEngine = *pImpEditView->pEditEngine;

    ESelection aSel( GetSelection() );
    ESelection aOldSelection( aSel );
    aSel.Adjust();

    if( !aSel.HasRange() )
    {
        aSel = rEditEngine.GetWord( aSel, css::i18n::WordType::DICTIONARY_WORD );
    }

    if( aSel.HasRange() )
    {
        for( sal_Int32 nPara = aSel.nStartPara; nPara <= aSel.nEndPara; nPara++ )
        {
            std::vector<sal_Int32> aPortions;
            rEditEngine.GetPortions( nPara, aPortions );

            if( aPortions.empty() )
                aPortions.push_back( rEditEngine.GetTextLen(nPara) );

            const sal_Int32 nBeginPos = (nPara == aSel.nStartPara) ? aSel.nStartPos : 0;
            const sal_Int32 nEndPos = (nPara == aSel.nEndPara) ? aSel.nEndPos : EE_TEXTPOS_ALL;

            for ( size_t nPos = 0; nPos < aPortions.size(); ++nPos )
            {
                sal_Int32 nPortionEnd   = aPortions[ nPos ];
                sal_Int32 nPortionStart = nPos > 0 ? aPortions[ nPos - 1 ] : 0;

                if( (nPortionEnd < nBeginPos) || (nPortionStart > nEndPos) )
                    continue;

                if( nPortionStart < nBeginPos )
                    nPortionStart = nBeginPos;
                if( nPortionEnd > nEndPos )
                    nPortionEnd = nEndPos;

                if( nPortionStart == nPortionEnd )
                    continue;

                ESelection aPortionSel( nPara, nPortionStart, nPara, nPortionEnd );
                ChangeFontSizeImpl( this, bGrow, aPortionSel, pFontList );
            }
        }
    }
    else
    {
        ChangeFontSizeImpl( this, bGrow, aSel, pFontList );
    }

    SetSelection( aOldSelection );
}

bool EditView::ChangeFontSize( bool bGrow, SfxItemSet& rSet, const FontList* pFontList )
{
    if (!pFontList)
        return false;

    static const sal_uInt16 gFontSizeWichMap[] = { EE_CHAR_FONTHEIGHT, EE_CHAR_FONTHEIGHT_CJK, EE_CHAR_FONTHEIGHT_CTL, 0 };
    bool bRet = false;

    const sal_uInt16* pWhich = gFontSizeWichMap;
    while( *pWhich )
    {
        SvxFontHeightItem aFontHeightItem( static_cast<const SvxFontHeightItem&>(rSet.Get( *pWhich )) );
        tools::Long nHeight = aFontHeightItem.GetHeight();
        const MapUnit eUnit = rSet.GetPool()->GetMetric( *pWhich );
        nHeight = OutputDevice::LogicToLogic(nHeight * 10, eUnit, MapUnit::MapPoint);

        const int* pAry = FontList::GetStdSizeAry();

        if( bGrow )
        {
            while( *pAry )
            {
                if( *pAry > nHeight )
                {
                    nHeight = *pAry;
                    break;
                }
                pAry++;
            }

            if( *pAry == 0 )
            {
                nHeight += (nHeight + 5) / 10;
                if( nHeight > 9999 )
                    nHeight = 9999;
            }

        }
        else if( *pAry )
        {
            bool bFound = false;
            if( *pAry < nHeight )
            {
                pAry++;
                while( *pAry )
                {
                    if( *pAry >= nHeight )
                    {
                        nHeight = pAry[-1];
                        bFound = true;
                        break;
                    }
                    pAry++;
                }
            }

            if( !bFound )
            {
                nHeight -= (nHeight + 5) / 10;
                if( nHeight < 2 )
                    nHeight = 2;
            }
        }

        if( (nHeight >= 2) && (nHeight <= 9999 ) )
        {
            nHeight = OutputDevice::LogicToLogic( nHeight, MapUnit::MapPoint, eUnit  ) / 10;

            if( nHeight != static_cast<tools::Long>(aFontHeightItem.GetHeight()) )
            {
                aFontHeightItem.SetHeight( nHeight );
                rSet.Put( aFontHeightItem.CloneSetWhich(*pWhich) );
                bRet = true;
            }
        }
        pWhich++;
    }
    return bRet;
}

OUString EditView::GetSurroundingText() const
{
    EditSelection aSel( pImpEditView->GetEditSelection() );
    aSel.Adjust( pImpEditView->pEditEngine->GetEditDoc() );

    if( HasSelection() )
    {
        OUString aStr = pImpEditView->pEditEngine->GetSelected(aSel);

        // Stop reconversion if the selected text includes a line break.
        if ( aStr.indexOf( 0x0A ) == -1 )
          return aStr;
        else
          return OUString();
    }
    else
    {
        aSel.Min().SetIndex( 0 );
        aSel.Max().SetIndex( aSel.Max().GetNode()->Len() );
        return pImpEditView->pEditEngine->GetSelected(aSel);
    }
}

Selection EditView::GetSurroundingTextSelection() const
{
    ESelection aSelection( GetSelection() );
    aSelection.Adjust();

    if( HasSelection() )
    {
        EditSelection aSel( pImpEditView->GetEditSelection() );
        aSel.Adjust( pImpEditView->pEditEngine->GetEditDoc() );
        OUString aStr = pImpEditView->pEditEngine->GetSelected(aSel);

        // Stop reconversion if the selected text includes a line break.
        if ( aStr.indexOf( 0x0A ) == -1 )
            return Selection( 0, aSelection.nEndPos - aSelection.nStartPos );
        else
            return Selection( 0, 0 );
    }
    else
    {
        return Selection( aSelection.nStartPos, aSelection.nEndPos );
    }
}

bool EditView::DeleteSurroundingText(const Selection& rRange)
{
    ESelection aSel(GetSelection());
    aSel.nEndPara = aSel.nStartPara;
    aSel.nStartPos = rRange.Min();
    aSel.nEndPos = rRange.Max();
    SetSelection(aSel);
    DeleteSelected();
    return true;
}

void EditView::SetCursorLogicPosition(const Point& rPosition, bool bPoint, bool bClearMark)
{
    Point aDocPos(pImpEditView->GetDocPos(rPosition));
    EditPaM aPaM = pImpEditView->pEditEngine->GetPaM(aDocPos);
    EditSelection aSelection(pImpEditView->GetEditSelection());

    // Explicitly create or delete the selection.
    if (bClearMark)
    {
        pImpEditView->DeselectAll();
        aSelection = pImpEditView->GetEditSelection();
    }
    else
        pImpEditView->CreateAnchor();

    if (bPoint)
        aSelection.Max() = aPaM;
    else
        aSelection.Min() = aPaM;

    if (pImpEditView->GetEditSelection().Min() != aSelection.Min())
        pImpEditView->pEditEngine->CursorMoved(pImpEditView->GetEditSelection().Min().GetNode());
    pImpEditView->DrawSelectionXOR(aSelection);
    if (pImpEditView->GetEditSelection() != aSelection)
        pImpEditView->SetEditSelection(aSelection);
    ShowCursor(/*bGotoCursor=*/false);
}

void EditView::DrawSelectionXOR(OutlinerViewShell* pOtherShell)
{
    pImpEditView->RegisterOtherShell(pOtherShell);
    pImpEditView->DrawSelectionXOR();
    pImpEditView->RegisterOtherShell(nullptr);
}

void EditView::InitLOKSpecialPositioning(MapUnit eUnit,
                                         const tools::Rectangle& rOutputArea,
                                         const Point& rVisDocStartPos)
{
    pImpEditView->InitLOKSpecialPositioning(eUnit, rOutputArea, rVisDocStartPos);
}

void EditView::SetLOKSpecialOutputArea(const tools::Rectangle& rOutputArea)
{
    pImpEditView->SetLOKSpecialOutputArea(rOutputArea);
}

const tools::Rectangle & EditView::GetLOKSpecialOutputArea() const
{
    return pImpEditView->GetLOKSpecialOutputArea();
}

void EditView::SetLOKSpecialVisArea(const tools::Rectangle& rVisArea)
{
    pImpEditView->SetLOKSpecialVisArea(rVisArea);
}

tools::Rectangle EditView::GetLOKSpecialVisArea() const
{
    return pImpEditView->GetLOKSpecialVisArea();
}

bool EditView::HasLOKSpecialPositioning() const
{
    return pImpEditView->HasLOKSpecialPositioning();
}

void EditView::SuppressLOKMessages(bool bSet)
{
    pImpEditView->SuppressLOKMessages(bSet);
}

bool EditView::IsSuppressLOKMessages() const
{
    return pImpEditView->IsSuppressLOKMessages();
}

void EditView::SetNegativeX(bool bSet)
{
    pImpEditView->SetNegativeX(bSet);
}

bool EditView::IsNegativeX() const
{
    return pImpEditView->IsNegativeX();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
