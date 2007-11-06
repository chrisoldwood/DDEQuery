/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		CONNECTDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CConnectDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef CONNECTDLG_HPP
#define CONNECTDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

/******************************************************************************
** 
** The server connection dialog.
**
*******************************************************************************
*/

class CConnectDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CConnectDlg();
	
	//
	// Members.
	//
	CString	m_strService;
	CString	m_strTopic;
	
protected:
	//
	// Controls.
	//
	CComboBox	m_cbService;
	CComboBox	m_cbTopic;
	CButton		m_btnPaste;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();

	void OnSelectServer();
	void OnPasteLink();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // CONNECTDLG_HPP
