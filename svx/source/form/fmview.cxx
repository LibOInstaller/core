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

#include <sfx2/docfile.hxx>
#ifdef REFERENCE
#undef REFERENCE
#endif
#include <com/sun/star/sdb/SQLContext.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/form/XLoadable.hpp>
#include <fmvwimp.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/bindings.hxx>
#include <fmobj.hxx>
#include <svx/svditer.hxx>
#include <svx/svdpagv.hxx>
#include <svx/fmview.hxx>
#include <svx/fmmodel.hxx>
#include <svx/fmpage.hxx>
#include <svx/fmshell.hxx>
#include <fmshimp.hxx>
#include <fmservs.hxx>
#include <fmundo.hxx>
#include <svx/dataaccessdescriptor.hxx>
#include <comphelper/namedvaluecollection.hxx>
#include <o3tl/deleter.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <tools/debug.hxx>
#include <svx/sdrpagewindow.hxx>
#include <svx/sdrpaintwindow.hxx>
#include <svx/svxids.hrc>
#include <vcl/i18nhelp.hxx>
#include <vcl/window.hxx>
#include <o3tl/string_view.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::util;
using namespace ::svxform;
using namespace ::svx;

FmFormView::FmFormView(
    SdrModel& rSdrModel,
    OutputDevice* pOut)
:   E3dView(rSdrModel, pOut)
{
    Init();
}

void FmFormView::Init()
{
    pFormShell = nullptr;
    pImpl = new FmXFormView(this);

    // set model
    SdrModel* pModel = &GetModel();

    DBG_ASSERT( dynamic_cast<const FmFormModel*>( pModel) !=  nullptr, "Wrong model" );
    FmFormModel* pFormModel = dynamic_cast<FmFormModel*>(pModel);
    if( !pFormModel )
        return;

    // get DesignMode from model
    bool bInitDesignMode = pFormModel->GetOpenInDesignMode();
    if ( pFormModel->OpenInDesignModeIsDefaulted( ) )
    {   // this means that nobody ever explicitly set this on the model, and the model has never
        // been loaded from a stream.
        // This means this is a newly created document. This means, we want to have it in design
        // mode by default (though a newly created model returns true for GetOpenInDesignMode).
        // We _want_ to have this because it makes a lot of hacks following the original fix
        DBG_ASSERT( !bInitDesignMode, "FmFormView::Init: doesn't the model default to FALSE anymore?" );
            // if this asserts, either the on-construction default in the model has changed (then this here
            // may not be necessary anymore), or we're not dealing with a new document...
        bInitDesignMode = true;
    }

    SfxObjectShell* pObjShell = pFormModel->GetObjectShell();
    if ( pObjShell && pObjShell->GetMedium() )
    {
        if ( const SfxUnoAnyItem *pItem = pObjShell->GetMedium()->GetItemSet()->GetItemIfSet( SID_COMPONENTDATA, false ) )
        {
            ::comphelper::NamedValueCollection aComponentData( pItem->GetValue() );
            bInitDesignMode = aComponentData.getOrDefault( "ApplyFormDesignMode", bInitDesignMode );
        }
    }

    // this will be done in the shell
    // bDesignMode = !bInitDesignMode;  // forces execution of SetDesignMode
    SetDesignMode( bInitDesignMode );
}

FmFormView::~FmFormView()
{
    if (pFormShell)
        suppress_fun_call_w_exception(pFormShell->SetView(nullptr));

    pImpl->notifyViewDying();
}

FmFormPage* FmFormView::GetCurPage()
{
    SdrPageView* pPageView = GetSdrPageView();
    FmFormPage*  pCurPage = pPageView ? dynamic_cast<FmFormPage*>( pPageView->GetPage()  ) : nullptr;
    return pCurPage;
}

void FmFormView::MarkListHasChanged()
{
    E3dView::MarkListHasChanged();

    if ( !(pFormShell && IsDesignMode()) )
        return;

    FmFormObj* pObj = getMarkedGrid();
    if ( pImpl->m_pMarkedGrid && pImpl->m_pMarkedGrid != pObj )
    {
        pImpl->m_pMarkedGrid = nullptr;
        if ( pImpl->m_xWindow.is() )
        {
            pImpl->m_xWindow->removeFocusListener(pImpl);
            pImpl->m_xWindow = nullptr;
        }
        SetMoveOutside(false);
        //OLMRefreshAllIAOManagers();
    }

    pFormShell->GetImpl()->SetSelectionDelayed_Lock();
}

namespace
{
    const SdrPageWindow* findPageWindow( const SdrPaintView* _pView, OutputDevice const * _pWindow )
    {
        SdrPageView* pPageView = _pView->GetSdrPageView();
        if(pPageView)
        {
            for ( sal_uInt32 window = 0; window < pPageView->PageWindowCount(); ++window )
            {
                const SdrPageWindow* pPageWindow = pPageView->GetPageWindow( window );
                if ( !pPageWindow || &pPageWindow->GetPaintWindow().GetOutputDevice() != _pWindow )
                    continue;

                return pPageWindow;
            }
        }
        return nullptr;
    }
}


void FmFormView::AddDeviceToPaintView(OutputDevice& rNewDev, vcl::Window* pWindow)
{
    E3dView::AddDeviceToPaintView(rNewDev, pWindow);

    // look up the PageViewWindow for the newly inserted window, and care for it
    // #i39269# / 2004-12-20 / frank.schoenheit@sun.com
    const SdrPageWindow* pPageWindow = findPageWindow( this, &rNewDev );
    if ( pPageWindow )
        pImpl->addWindow( *pPageWindow );
}


void FmFormView::DeleteDeviceFromPaintView(OutputDevice& rNewDev)
{
    const SdrPageWindow* pPageWindow = findPageWindow( this, &rNewDev );
    if ( pPageWindow )
        pImpl->removeWindow( pPageWindow->GetControlContainer() );

    E3dView::DeleteDeviceFromPaintView(rNewDev);
}


void FmFormView::ChangeDesignMode(bool bDesign)
{
    if (bDesign == IsDesignMode())
        return;

    FmFormModel* pModel = dynamic_cast<FmFormModel*>(&GetModel());
    if (pModel)
    {   // For the duration of the transition the Undo-Environment is disabled. This ensures that non-transient Properties can
        // also be changed (this should be done with care and also reversed before switching the mode back. An example is the
        // setting of the maximal length of the text by FmXEditModel on its control.)
        pModel->GetUndoEnv().Lock();
    }

    // --- 1. deactivate all controls if we are switching to design mode
    if ( bDesign )
        DeactivateControls( GetSdrPageView() );

    // --- 2. simulate a deactivation (the shell will handle some things there ...?)
    if ( pFormShell && pFormShell->GetImpl() )
        pFormShell->GetImpl()->viewDeactivated_Lock(*this);
    else
        pImpl->Deactivate();

    // --- 3. activate all controls, if we're switching to alive mode
    if ( !bDesign )
        ActivateControls( GetSdrPageView() );

    // --- 4. load resp. unload the forms
    FmFormPage*  pCurPage = GetCurPage();
    if ( pCurPage )
    {
        if ( pFormShell && pFormShell->GetImpl() )
            pFormShell->GetImpl()->loadForms_Lock(pCurPage, (bDesign ? LoadFormsFlags::Unload : LoadFormsFlags::Load));
    }

    // --- 5. base class functionality
    SetDesignMode( bDesign );

    // --- 6. simulate an activation (the shell will handle some things there ...?)
    OSL_PRECOND( pFormShell && pFormShell->GetImpl(), "FmFormView::ChangeDesignMode: is this really allowed? No shell?" );
    if ( pFormShell && pFormShell->GetImpl() )
        pFormShell->GetImpl()->viewActivated_Lock(*this);
    else
        pImpl->Activate();

    if ( pCurPage )
    {
        if ( bDesign )
        {
            if ( GetActualOutDev() && GetActualOutDev()->GetOutDevType() == OUTDEV_WINDOW )
            {
                const vcl::Window* pWindow = GetActualOutDev()->GetOwnerWindow();
                const_cast< vcl::Window* >( pWindow )->GrabFocus();
            }

            // redraw UNO objects
            if ( GetSdrPageView() )
            {
                SdrObjListIter aIter(pCurPage);
                while( aIter.IsMore() )
                {
                    SdrObject* pObj = aIter.Next();
                    if (pObj && pObj->IsUnoObj())
                    {
                        // For redraw just use ActionChanged()
                        // pObj->BroadcastObjectChange();
                        pObj->ActionChanged();
                    }
                }
            }
        }
        else
        {
            // set the auto focus to the first control (if indicated by the model to do so)
            bool bForceControlFocus = pModel && pModel->GetAutoControlFocus();
            if (bForceControlFocus)
                pImpl->AutoFocus();
        }
    }

    // Unlock Undo-Environment
    if (pModel)
        pModel->GetUndoEnv().UnLock();
}


void FmFormView::GrabFirstControlFocus()
{
    if ( !IsDesignMode() )
        pImpl->AutoFocus();
}


SdrPageView* FmFormView::ShowSdrPage(SdrPage* pPage)
{
    SdrPageView* pPV = E3dView::ShowSdrPage(pPage);

    if (pPage)
    {
        if (!IsDesignMode())
        {
            // creating the controllers
            ActivateControls(pPV);

            // Deselect all
            UnmarkAll();
        }
        else if ( pFormShell && pFormShell->IsDesignMode() )
        {
            FmXFormShell* pFormShellImpl = pFormShell->GetImpl();
            pFormShellImpl->UpdateForms_Lock(true);

            // so that the form navigator can react to the pagechange
            pFormShell->GetViewShell()->GetViewFrame()->GetBindings().Invalidate(SID_FM_FMEXPLORER_CONTROL, true);

            pFormShellImpl->SetSelection_Lock(GetMarkedObjectList());
        }
    }

    // notify our shell that we have been activated
    if ( pFormShell && pFormShell->GetImpl() )
        pFormShell->GetImpl()->viewActivated_Lock(*this);
    else
        pImpl->Activate();

    return pPV;
}


void FmFormView::HideSdrPage()
{
    // --- 1. deactivate controls
    if ( !IsDesignMode() )
        DeactivateControls(GetSdrPageView());

    // --- 2. tell the shell the view is (going to be) deactivated
    if ( pFormShell && pFormShell->GetImpl() )
        pFormShell->GetImpl()->viewDeactivated_Lock(*this);
    else
        pImpl->Deactivate();

    // --- 3. base class behavior
    E3dView::HideSdrPage();
}


void FmFormView::ActivateControls(SdrPageView const * pPageView)
{
    if (!pPageView)
        return;

    for (sal_uInt32 i = 0; i < pPageView->PageWindowCount(); ++i)
    {
        const SdrPageWindow& rPageWindow = *pPageView->GetPageWindow(i);
        pImpl->addWindow(rPageWindow);
    }
}


void FmFormView::DeactivateControls(SdrPageView const * pPageView)
{
    if( !pPageView )
        return;

    for (sal_uInt32 i = 0; i < pPageView->PageWindowCount(); ++i)
    {
        const SdrPageWindow& rPageWindow = *pPageView->GetPageWindow(i);
        pImpl->removeWindow(rPageWindow.GetControlContainer() );
    }
}


rtl::Reference<SdrObject> FmFormView::CreateFieldControl( const ODataAccessDescriptor& _rColumnDescriptor )
{
    return pImpl->implCreateFieldControl( _rColumnDescriptor );
}


rtl::Reference<SdrObject> FmFormView::CreateXFormsControl( const OXFormsDescriptor &_rDesc )
{
    return pImpl->implCreateXFormsControl(_rDesc);
}


rtl::Reference<SdrObject> FmFormView::CreateFieldControl(std::u16string_view rFieldDesc) const
{
    sal_Int32 nIdx{ 0 };
    OUString sDataSource(    o3tl::getToken(rFieldDesc, 0, u'\x000B', nIdx));
    OUString sObjectName(    o3tl::getToken(rFieldDesc, 0, u'\x000B', nIdx));
    sal_uInt16 nObjectType   = static_cast<sal_uInt16>(o3tl::toInt32(o3tl::getToken(rFieldDesc, 0, u'\x000B', nIdx)));
    OUString sFieldName(      o3tl::getToken(rFieldDesc, 0, u'\x000B', nIdx));

    if (sFieldName.isEmpty() || sObjectName.isEmpty() || sDataSource.isEmpty())
        return nullptr;

    ODataAccessDescriptor aColumnDescriptor;
    aColumnDescriptor.setDataSource(sDataSource);
    aColumnDescriptor[ DataAccessDescriptorProperty::Command ]          <<= sObjectName;
    aColumnDescriptor[ DataAccessDescriptorProperty::CommandType ]      <<= nObjectType;
    aColumnDescriptor[ DataAccessDescriptorProperty::ColumnName ]       <<= sFieldName;

    return pImpl->implCreateFieldControl( aColumnDescriptor );
}


void FmFormView::InsertControlContainer(const Reference< css::awt::XControlContainer > & xCC)
{
    if( IsDesignMode() )
        return;

    SdrPageView* pPageView = GetSdrPageView();
    if( !pPageView )
        return;

    for( sal_uInt32 i = 0; i < pPageView->PageWindowCount(); i++ )
    {
        const SdrPageWindow& rPageWindow = *pPageView->GetPageWindow(i);

        if( rPageWindow.GetControlContainer( false ) == xCC )
        {
            pImpl->addWindow(rPageWindow);
            break;
        }
    }
}


void FmFormView::RemoveControlContainer(const Reference< css::awt::XControlContainer > & xCC)
{
    if( !IsDesignMode() )
    {
        pImpl->removeWindow( xCC );
    }
}


SdrPaintWindow* FmFormView::BeginCompleteRedraw(OutputDevice* pOut)
{
    SdrPaintWindow* pPaintWindow = E3dView::BeginCompleteRedraw( pOut );
    pImpl->suspendTabOrderUpdate();
    return pPaintWindow;
}


void FmFormView::EndCompleteRedraw( SdrPaintWindow& rPaintWindow, bool bPaintFormLayer )
{
    E3dView::EndCompleteRedraw( rPaintWindow, bPaintFormLayer );
    pImpl->resumeTabOrderUpdate();
}


bool FmFormView::KeyInput(const KeyEvent& rKEvt, vcl::Window* pWin)
{
    bool bDone = false;
    const vcl::KeyCode& rKeyCode = rKEvt.GetKeyCode();
    if  (   IsDesignMode()
        &&  rKeyCode.GetCode() == KEY_RETURN
        )
    {
        // RETURN alone enters grid controls, for keyboard accessibility
        if  (   pWin
            &&  !rKeyCode.IsShift()
            &&  !rKeyCode.IsMod1()
            &&  !rKeyCode.IsMod2()
            )
        {
            FmFormObj* pObj = getMarkedGrid();
            if ( pObj )
            {
                Reference< awt::XWindow > xWindow( pObj->GetUnoControl( *this, *pWin->GetOutDev() ), UNO_QUERY );
                if ( xWindow.is() )
                {
                    pImpl->m_pMarkedGrid = pObj;
                    pImpl->m_xWindow = xWindow;
                    // add as listener to get notified when ESC will be pressed inside the grid
                    pImpl->m_xWindow->addFocusListener(pImpl);
                    SetMoveOutside(true);
                    //OLMRefreshAllIAOManagers();
                    xWindow->setFocus();
                    bDone = true;
                }
            }
        }
        // Alt-RETURN alone shows the properties of the selection
        if  (   pFormShell
            &&  pFormShell->GetImpl()
            &&  !rKeyCode.IsShift()
            &&  !rKeyCode.IsMod1()
            &&   rKeyCode.IsMod2()
            )
        {
            pFormShell->GetImpl()->handleShowPropertiesRequest_Lock();
        }

    }

    // tdf#139804 Allow selecting form controls with Alt-<Mnemonic>
    if (rKeyCode.IsMod2() && rKeyCode.GetCode())
    {
        if (FmFormPage* pCurPage = GetCurPage())
        {
            for (size_t a = 0; a < pCurPage->GetObjCount(); ++a)
            {
                SdrObject* pObj = pCurPage->GetObj(a);
                FmFormObj* pFormObject = FmFormObj::GetFormObject(pObj);
                if (!pFormObject)
                    continue;

                Reference<awt::XControl> xControl = pFormObject->GetUnoControl(*this, *pWin->GetOutDev());
                if (!xControl.is())
                    continue;
                const vcl::I18nHelper& rI18nHelper = Application::GetSettings().GetUILocaleI18nHelper();
                VclPtr<vcl::Window> pWindow = VCLUnoHelper::GetWindow(xControl->getPeer());
                if (rI18nHelper.MatchMnemonic(pWindow->GetText(), rKEvt.GetCharCode()))
                {
                    pWindow->GrabFocus();
                    pWindow->KeyInput(rKEvt);
                    bDone = true;
                    break;
                }
            }
        }
    }

    if ( !bDone )
        bDone = E3dView::KeyInput(rKEvt,pWin);
    return bDone;
}

bool FmFormView::checkUnMarkAll(const Reference< XInterface >& _xSource)
{
    Reference< css::awt::XControl> xControl(pImpl->m_xWindow,UNO_QUERY);
    bool bRet = !xControl.is() || !_xSource.is() || _xSource != xControl->getModel();
    if ( bRet )
        UnmarkAll();

    return bRet;
}


bool FmFormView::MouseButtonDown( const MouseEvent& _rMEvt, OutputDevice* _pWin )
{
    bool bReturn = E3dView::MouseButtonDown( _rMEvt, _pWin );

    if ( pFormShell && pFormShell->GetImpl() )
    {
        SdrViewEvent aViewEvent;
        PickAnything( _rMEvt, SdrMouseEventKind::BUTTONDOWN, aViewEvent );
        pFormShell->GetImpl()->handleMouseButtonDown_Lock(aViewEvent);
    }

    return bReturn;
}


FmFormObj* FmFormView::getMarkedGrid() const
{
    FmFormObj* pFormObject = nullptr;
    const SdrMarkList& rMarkList = GetMarkedObjectList();
    if ( 1 == rMarkList.GetMarkCount() )
    {
        SdrMark* pMark = rMarkList.GetMark(0);
        if ( pMark )
        {
            pFormObject = FmFormObj::GetFormObject( pMark->GetMarkedSdrObj() );
            if ( pFormObject )
            {
                Reference< XServiceInfo > xServInfo( pFormObject->GetUnoControlModel(), UNO_QUERY );
                if ( !xServInfo.is() || !xServInfo->supportsService( FM_SUN_COMPONENT_GRIDCONTROL ) )
                    pFormObject = nullptr;
            }
        }
    }
    return pFormObject;
}

void FmFormView::createControlLabelPair( OutputDevice const * _pOutDev, sal_Int32 _nXOffsetMM, sal_Int32 _nYOffsetMM,
    const Reference< XPropertySet >& _rxField, const Reference< XNumberFormats >& _rxNumberFormats,
    SdrObjKind _nControlObjectID, SdrInventor _nInventor, SdrObjKind _nLabelObjectID,
    SdrModel& _rModel,
    rtl::Reference<SdrUnoObj>& _rpLabel,
    rtl::Reference<SdrUnoObj>& _rpControl )
{
    FmXFormView::createControlLabelPair(
        *_pOutDev, _nXOffsetMM, _nYOffsetMM,
        _rxField, _rxNumberFormats,
        _nControlObjectID, u"", _nInventor, _nLabelObjectID,
        _rModel,
        _rpLabel, _rpControl
    );
}

Reference< runtime::XFormController > FmFormView::GetFormController( const Reference< XForm >& _rxForm, const OutputDevice& _rDevice ) const
{
    return pImpl->getFormController( _rxForm, _rDevice );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
