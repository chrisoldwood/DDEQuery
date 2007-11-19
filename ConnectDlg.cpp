/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		CONNECTDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CConnectDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "ConnectDlg.hpp"
#include "DDEQueryApp.hpp"
#include <limits.h>
#include <NCL/DDEClient.hpp>
#include <NCL/DDEException.hpp>
#include <NCL/DDELink.hpp>

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
		CTRL(IDC_SERVICE,    &m_cbService)
		CTRL(IDC_TOPIC,      &m_cbTopic  )
		CTRL(IDC_PASTE_LINK, &m_btnPaste )
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
		CMD_CTRLMSG(IDC_SERVICE,    CBN_SELCHANGE, &CConnectDlg::OnSelectServer)
		CMD_CTRLMSG(IDC_PASTE_LINK, BN_CLICKED,    &CConnectDlg::OnPasteLink   )
	END_CTRLMSG_TABLE
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
	try
	{
		CStrArray astrServers;

		// Populate servers combo.
		App.m_pDDEClient->QueryServers(astrServers);
	
		for (int i = 0; i < astrServers.Size(); ++i)
			m_cbService.Add(astrServers[i]);

		// Select 1st by default.
		if (m_cbService.Count() > 0)
			m_cbService.CurSel(0);
	}
	catch (CDDEException& /*e*/)
	{ }

	// Select previous service name.
	if (m_strService != "")
	{
		int nDefault = m_cbService.FindExact(m_strService);

		if (nDefault == CB_ERR)
			nDefault = m_cbService.Add(m_strService);

		m_cbService.CurSel(nDefault);
	}

	// Initalise topics list.
	OnSelectServer();

	// Select previous topic name.
	if (m_strTopic != "")
	{
		int nDefault = m_cbTopic.FindExact(m_strTopic);

		if (nDefault == CB_ERR)
			nDefault = m_cbTopic.Add(m_strTopic);

		m_cbTopic.CurSel(nDefault);
	}

	// Is link on clipboard?
	m_btnPaste.Enable(CDDELink::CanPasteLink());
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
	// Validate service name.
	if (m_cbService.TextLength() == 0)
	{
		AlertMsg("Please enter a DDE service name.");
		m_cbService.Focus();
		return false;
	}

	// Validate topic name.
	if (m_cbTopic.TextLength() == 0)
	{
		AlertMsg("Please enter a topic name.");
		m_cbTopic.Focus();
		return false;
	}

	// Save control values.
	m_strService = m_cbService.Text();
	m_strTopic   = m_cbTopic.Text();

	return true;
}

/******************************************************************************
** Method:		OnSelectServer()
**
** Description:	New server selected, populate the topic list.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CConnectDlg::OnSelectServer()
{
	// Clear old list.
	m_cbTopic.Reset();

	// Server list empty?
	if (m_cbService.CurSel() == CB_ERR)
		return;

	try
	{
		CString strServer = m_cbService.Text(m_cbService.CurSel());

		CStrArray astrTopics;

		// Populate topics combo.
		App.m_pDDEClient->QueryServerTopics(strServer, astrTopics);
	
		for (int i = 0; i < astrTopics.Size(); ++i)
			m_cbTopic.Add(astrTopics[i]);

		// Select 1st by default.
		if (m_cbTopic.Count() > 0)
			m_cbTopic.CurSel(0);
	}
	catch (CDDEException& /*e*/)
	{ }
}

/******************************************************************************
** Method:		OnPasteLink()
**
** Description:	Paste the link into the Service and Topic fields.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CConnectDlg::OnPasteLink()
{
	CString strService, strTopic, strItem;

	// Get the link from the clipboard.
	if (!CDDELink::PasteLink(strService, strTopic, strItem))
		return;

	// Copy to controls.
	m_cbService.Title(strService);
	m_cbTopic.Title(strTopic);
}
