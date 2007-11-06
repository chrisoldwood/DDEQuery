/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		PREFSDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CPrefsDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef PREFSDLG_HPP
#define PREFSDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

/******************************************************************************
** 
** The preferences dialog.
**
*******************************************************************************
*/

class CPrefsDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CPrefsDlg();
	
protected:
	//
	// Members.
	//
	
	//
	// Controls.
	//
	CDecimalBox	m_ebDDETimeOut;
	CDecimalBox	m_ebFlashTime;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual bool OnOk();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // PREFSDLG_HPP
