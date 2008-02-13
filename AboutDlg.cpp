/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		ABOUTDLG.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAboutDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "AboutDlg.hpp"
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

CAboutDlg::CAboutDlg()
	: CDialog(IDD_ABOUT)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_VERSION,	&m_txtVersion)
		CTRL(IDC_EMAIL,		&m_txtEmail  )
		CTRL(IDC_WEBSITE,	&m_txtWebSite)
	END_CTRL_TABLE

	// Set the URL label protocols.
	m_txtEmail.Protocol(TXT("mailto:"));
	m_txtWebSite.Protocol(TXT("http://"));
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

void CAboutDlg::OnInitDialog()
{
	m_txtVersion.Text(App.VERSION);
}
