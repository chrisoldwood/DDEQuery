/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPWND.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CAppWnd class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef APPWND_HPP
#define APPWND_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/DlgFrame.hpp>
#include "AppDlg.hpp"
#include <WCL/FrameMenu.hpp>
#include <WCL/StatusBar.hpp>
#include <WCL/StatusBarIcon.hpp>
#include <WCL/Accel.hpp>
#include <WCL/TransparentBmp.hpp>

/******************************************************************************
**
** This is the the applications main window.
**
*******************************************************************************
*/

class CAppWnd : public CDlgFrame
{
public:
	//
	// Constructors/Destructor.
	//
	CAppWnd();
	~CAppWnd();

	//
	// Methods.
	//
	void UpdateTitle();
	void ShowFlashIcon(bool bShow);

	//
	// Members.
	//
	CAppDlg			m_AppDlg;
	CFrameMenu		m_Menu;
	CStatusBar		m_StatusBar;
	CStatusBarIcon	m_oInDDEIcon;

protected:
	//
	// Internal members.
	//
	CAccel			m_Accel;
	CTransparentBmp	m_oFlashIcon;

	// Child window IDs.
	static const uint IDC_STATUS_BAR = 101;
	static const uint IDC_INDDE_ICON = 102;

	//
	// Message processors.
	//
	virtual void OnCreate(const CRect& rcClient);
	virtual bool OnQueryClose();
	virtual void OnClose();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif //APPWND_HPP
