/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		FULLVALUEDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CFullValueDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "FullValueDlg.hpp"

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

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

CFullValueDlg::CFullValueDlg()
	: CDialog(IDD_FULL_VALUE)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_VIEWS, &m_tcViews)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_VIEWS, LEFT_EDGE,  TOP_EDGE,    RIGHT_EDGE, BOTTOM_EDGE)
		CTRLGRAV(IDCANCEL,  RIGHT_EDGE, BOTTOM_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
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

void CFullValueDlg::OnInitDialog()
{
	ASSERT(!m_strItem.Empty());

	// Set the dialog title.
	Title(m_strItem);

	// Restore previous position, if set.
	if (!App.m_rcFullValueDlg.Empty())
		Move(App.m_rcFullValueDlg);

	// Attach value buffer to views.
	m_dlgTextView.m_pValue   = &m_oValue;
	m_dlgBinaryView.m_pValue = &m_oValue;

	// Create views.
	m_dlgTextView.RunModeless(*this);
	m_dlgBinaryView.RunModeless(*this);

	// Add views to tab control.
	m_tcViews.AddTab("Text",   m_dlgTextView);
	m_tcViews.AddTab("Binary", m_dlgBinaryView);

	// Display "Text" view by default.
	m_tcViews.CurSel(0);
}

/******************************************************************************
** Method:		OnDestroy()
**
** Description:	The window is being destroyed.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CFullValueDlg::OnDestroy()
{
	// Remember its position.
	App.m_rcFullValueDlg = Placement();
}

/******************************************************************************
** Method:		OnOk()
**
** Description:	The OK button was pressed.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CFullValueDlg::OnOk()
{
	return true;
}
