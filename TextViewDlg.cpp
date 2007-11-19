/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		TEXTVIEWDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CTextViewDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "TextViewDlg.hpp"
#include "DDEQueryApp.hpp"

/******************************************************************************
** Method:		Default constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CTextViewDlg::CTextViewDlg()
	: CDialog(IDD_TEXT_VIEW)
	, m_pValue(NULL)
	, m_pFormat(NULL)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_VALUE, &m_ebValue)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_VALUE, EN_SETFOCUS, &CTextViewDlg::OnValueSetFocus)
	END_CTRLMSG_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_VALUE, LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
	END_GRAVITY_TABLE
}

/******************************************************************************
** Method:		OnInitDialog()
**
** Description:	Initialise the dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTextViewDlg::OnInitDialog()
{
	ASSERT(m_pValue  != NULL);
	ASSERT(m_pFormat != NULL);

	// Tab stops are in dialog units.
	const uint DLG_CHAR_WIDTH = 4;
	const uint CHARS_PER_TAB  = 4;

	// Set control styles.
	m_ebValue.Font(CFont(ANSI_FIXED_FONT));
	m_ebValue.TabWidth(CHARS_PER_TAB*DLG_CHAR_WIDTH);

	// Display value.
	m_ebValue.Text(App.GetDisplayValue(*m_pValue, *m_pFormat, false));
}

/******************************************************************************
** Method:		OnCtlColour()
**
** Description:	Gets the colours for a child control.
**				All WM_CTLCOLOR* messages map to this one.
**
** Parameters:	nCtlClrMsg	The WM_CTLCOLOR* message,
**				hDC			The drawing HDC.
**				hCtlWnd		The controls hWhd.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTextViewDlg::OnCtlColour(uint nCtlClrMsg, HDC hDC, HWND hCtlWnd)
{
	// Create a brush for the edit box background.
	static CBrush oBgBrush(::GetSysColor(COLOR_WINDOW));

	// Edit box background colour?
	if ( (nCtlClrMsg == WM_CTLCOLORSTATIC) && (hCtlWnd == m_ebValue.Handle()) )
	{
		// Set colour in HDC and return brush.
		::SetBkColor(hDC, ::GetSysColor(COLOR_WINDOW));

		MsgHandled((BOOL)oBgBrush.Handle());
		MsgResult (0);
		return;
	}

	// Forward to base class.
	return CDialog::OnCtlColour(nCtlClrMsg, hDC, hCtlWnd);
}

/******************************************************************************
** Method:		OnValueSetFocus()
**
** Description:	Focus moved to value edit box - disable the selection of all
**				text in the window.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CTextViewDlg::OnValueSetFocus()
{
	// Remove any selection.
	m_ebValue.Select(-1, 0);
}
