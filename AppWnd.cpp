/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPWND.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppWnd class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

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

CAppWnd::CAppWnd()
	: CDlgFrame(IDR_APPICON, m_AppDlg, false)
{
}

/******************************************************************************
** Method:		Destructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CAppWnd::~CAppWnd()
{
}

/******************************************************************************
** Method:		OnCreate()
**
** Description:	Creates the toolbar and status bars.
**
** Parameters:	rcClient	The client rectangle.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::OnCreate(const CRect& rcClient)
{
	//
	// Create and attach the components.
	//
	m_Accel.LoadRsc(IDR_APPACCEL);
	Accel(&m_Accel);

	m_Menu.LoadRsc(IDR_APPMENU);
	Menu(&m_Menu);

	m_oFlashIcon.LoadRsc(IDB_LISTICONS, RGB(255, 0, 255));

	m_StatusBar.Create(*this, IDC_STATUS_BAR, CRect());
	StatusBar(&m_StatusBar);

	m_oInDDEIcon.Create(m_StatusBar, IDC_INDDE_ICON, CRect());
	m_StatusBar.AddPanel(m_oInDDEIcon);

	m_AppDlg.RunModeless(*this);
	ActiveDlg(&m_AppDlg);

	// Call base class.
	CDlgFrame::OnCreate(rcClient);
}

/******************************************************************************
** Method:		OnQueryClose()
**
** Description:	Check if the app can close.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CAppWnd::OnQueryClose()
{
	// Inside a DDE request?
	if (App.m_bInDDECall)
	{
		App.m_AppWnd.AlertMsg("You cannot close DDE Query while a request is in progress.");
		return false;
	}

	return true;
}

/******************************************************************************
** Method:		OnClose()
**
** Description:	The main window is closing.
**				NB: Called from WM_CLOSE or WM_ENDSESSION.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::OnClose()
{
	// Fetch windows final placement.
	App.m_rcLastPos = Placement();

	// Close connection.
	App.m_AppCmds.DoServerDisconnect();
}

/******************************************************************************
** Method:		UpdateTitle()
**
** Description:	Update the title bar with the connection status.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::UpdateTitle()
{
	CString strTitle = App.m_strTitle;

	// Append connection, if open.
	if (App.m_pDDEConv != NULL)
	{
		strTitle += " - ";
		strTitle += App.m_strLastService;
		strTitle += "|";
		strTitle += App.m_strLastTopic;
	}

	Title(strTitle);
}

/******************************************************************************
** Method:		ShowFlashIcon()
**
** Description:	Show or hide the flash icon in the status bar.
**
** Parameters:	bShow	Show or hide flag.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppWnd::ShowFlashIcon(bool bShow)
{
	if (bShow)
		m_oInDDEIcon.SetIcon(m_oFlashIcon, 1, true);
	else
		m_oInDDEIcon.ClearIcon(true);
}
