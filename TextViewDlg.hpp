/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		TEXTVIEWDLG.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CTextViewDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef TEXTVIEWDLG_HPP
#define TEXTVIEWDLG_HPP

/******************************************************************************
** 
** The view used to display the value as a text string.
**
*******************************************************************************
*/

class CTextViewDlg : public CDialog
{
public:
	//
	// Constructors/Destructor.
	//
	CTextViewDlg();
	
	//
	// Members.
	//
	CBuffer*	m_pValue;
	
protected:
	//
	// Controls.
	//
	CEditBox	m_ebValue;

	//
	// Message handlers.
	//
	virtual void OnInitDialog();
	virtual void OnCtlColour(uint nCtlClrMsg, HDC hDC, HWND hCtlWnd);

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

#endif // TEXTVIEWDLG_HPP
