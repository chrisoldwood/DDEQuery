/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		FULLVALUEDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CFullValueDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef FULLVALUEDLG_HPP
#define FULLVALUEDLG_HPP

#include "TextViewDlg.hpp"
#include "BinaryViewDlg.hpp"

/******************************************************************************
** 
** The dialog used to display the full value as text and binary.
**
*******************************************************************************
*/

class CFullValueDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CFullValueDlg();
	
	//
	// Members.
	//
	CString	m_strItem;
	CBuffer	m_oValue;
	
protected:
	//
	// Controls.
	//
	CTabWndHost		m_tcViews;
	CTextViewDlg	m_dlgTextView;
	CBinaryViewDlg	m_dlgBinaryView;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual void OnDestroy();
	virtual bool OnOk();
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // FULLVALUEDLG_HPP
