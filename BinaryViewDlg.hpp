/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		BINARYVIEWDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CBinaryViewDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef BINARYVIEWDLG_HPP
#define BINARYVIEWDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

// Forward declarations.
class CBuffer;

/******************************************************************************
** 
** The view used to display the value as a binary chunk.
**
*******************************************************************************
*/

class CBinaryViewDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CBinaryViewDlg();
	
	//
	// Members.
	//
	CBuffer*	m_pValue;
	
protected:
	//
	// Controls.
	//
	CListBox	m_lbValue;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();

	//
	// Internal methods.
	//
	void DisplayValue();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // BINARYVIEWDLG_HPP
