/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		CONNECTDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CConnectDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "ConnectDlg.hpp"
#include <limits.h>

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

CConnectDlg::CConnectDlg()
	: CDialog(IDD_CONNECT)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_SERVICE, &m_ebService)
		CTRL(IDC_TOPIC,   &m_ebTopic  )
	END_CTRL_TABLE
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

void CConnectDlg::OnInitDialog()
{
	// Display defaults.
	m_ebService.Text(m_strService);
	m_ebTopic.Text(m_strTopic);
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

bool CConnectDlg::OnOk()
{
	m_strService = m_ebService.Text();
	m_strTopic   = m_ebTopic.Text();

	// Validate service name.
	if (m_strService.Length() == 0)
	{
		AlertMsg("Please enter a DDE service name.");
		m_ebService.Focus();
		return false;
	}

	// Validate topic name.
	if (m_strTopic.Length() == 0)
	{
		AlertMsg("Please enter a topic name.");
		m_ebTopic.Focus();
		return false;
	}

	return true;
}
