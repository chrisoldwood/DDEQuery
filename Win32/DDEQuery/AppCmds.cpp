/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPCMDS.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppCmds class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "AboutDlg.hpp"
#include "ConnectDlg.hpp"

/******************************************************************************
** Method:		Constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CAppCmds::CAppCmds()
{
	// Define the command table.
	DEFINE_CMD_TABLE
		// Server menu.
		CMD_ENTRY(ID_SERVER_CONNECT,	OnServerConnect,		OnUIServerConnect,		-1)
		CMD_ENTRY(ID_SERVER_DISCONNECT,	OnServerDisconnect,		OnUIServerDisconnect,	-1)
		CMD_ENTRY(ID_SERVER_EXIT,		OnServerExit,			NULL,					-1)
		// Command menu.
		CMD_ENTRY(ID_COMMAND_REQUEST,	OnCommandRequest,		OnUICommandRequest,		-1)
		CMD_ENTRY(ID_COMMAND_POKE,		OnCommandPoke,			OnUICommandPoke,		-1)
		CMD_ENTRY(ID_COMMAND_EXECUTE,	OnCommandExecute,		OnUICommandExecute,		-1)
		CMD_ENTRY(ID_COMMAND_ADVISE,	OnCommandAdvise,		OnUICommandAdvise,		-1)
		CMD_ENTRY(ID_COMMAND_UNADVISE,	OnCommandUnadvise,		OnUICommandUnadvise,	-1)
		// Help menu.
		CMD_ENTRY(ID_HELP_ABOUT,		OnHelpAbout,			NULL,					10)
	END_CMD_TABLE
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

CAppCmds::~CAppCmds()
{
}

/******************************************************************************
** Method:		OnServerConnect()
**
** Description:	Open a conection to a server.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnServerConnect()
{
	CConnectDlg Dlg;

	// Initialise with previous settings.
	Dlg.m_strService = App.m_strLastService;
	Dlg.m_strTopic   = App.m_strLastTopic;

	// Get service and topic.
	if (Dlg.RunModal(App.m_AppWnd) == IDOK)
	{
		CBusyCursor oCursor;

		try
		{
			// Close existing conversation.
			OnServerDisconnect();

			ASSERT(App.m_pDDEConv == NULL);

			// Create conversation.
			App.m_pDDEConv = App.m_pDDEClient->CreateConversation(Dlg.m_strService, Dlg.m_strTopic);

			// Remember new settings.
			App.m_strLastService = Dlg.m_strService;
			App.m_strLastTopic   = Dlg.m_strTopic;

			// Set focus to Item field.
			App.m_AppWnd.m_AppDlg.m_ebItem.Focus();
		}
		catch (CDDEException& e)
		{
			OnServerDisconnect();
			App.AlertMsg(e.ErrorText());
		}
	}

	UpdateUI();
	App.m_AppWnd.UpdateTitle();
}

/******************************************************************************
** Method:		OnServerDisconnect()
**
** Description:	Close the connection to the server.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnServerDisconnect()
{
	// Conversation open?
	if (App.m_pDDEConv != NULL)
	{
		App.m_pDDEClient->DestroyConversation(App.m_pDDEConv);

		App.m_pDDEConv = NULL;
	}

	// Clear to the links listview.
	App.m_AppWnd.m_AppDlg.RemoveAllLinks();

	UpdateUI();
	App.m_AppWnd.UpdateTitle();
}

/******************************************************************************
** Method:		OnServerExit()
**
** Description:	Terminate the app.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnServerExit()
{
	App.m_AppWnd.Close();
}

/******************************************************************************
** Method:		OnCommandRequest()
**
** Description:	Request the items value.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnCommandRequest()
{
	ASSERT(App.m_pDDEConv != NULL);

	// Get the DDE item.
	CString strItem = App.m_AppWnd.m_AppDlg.m_ebItem.Text();
	uint    nFormat = CF_TEXT;

	// Item provided?
	if (strItem == "")
	{
		App.AlertMsg("Please provide the item name.");
		App.m_AppWnd.m_AppDlg.m_ebItem.Focus();
		return;
	}

	try
	{
		// Make the request.
		CDDEData oData   = App.m_pDDEConv->Request(strItem, nFormat);

		// Display value.
		App.m_AppWnd.m_AppDlg.m_ebValue.Text(oData.GetString());
	}
	catch (CDDEException& e)
	{
		App.AlertMsg(e.ErrorText());
	}
}

/******************************************************************************
** Method:		OnCommandPoke()
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnCommandPoke()
{
	ASSERT(App.m_pDDEConv != NULL);

	App.AlertMsg("Feature not implemented.");
}

/******************************************************************************
** Method:		OnCommandExecute()
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnCommandExecute()
{
	ASSERT(App.m_pDDEConv != NULL);

	App.AlertMsg("Feature not implemented.");
}

/******************************************************************************
** Method:		OnCommandAdvise()
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnCommandAdvise()
{
	ASSERT(App.m_pDDEConv != NULL);

	// Get the DDE item.
	CString strItem = App.m_AppWnd.m_AppDlg.m_ebItem.Text();
	uint    nFormat = CF_TEXT;

	// Item provided?
	if (strItem == "")
	{
		App.AlertMsg("Please provide the item name.");
		App.m_AppWnd.m_AppDlg.m_ebItem.Focus();
		return;
	}

	try
	{
		// Create the link.
		CDDELink* pLink = App.m_pDDEConv->CreateLink(strItem, nFormat);

		// Add to the links listview.
		App.m_AppWnd.m_AppDlg.AddLink(pLink);
	}
	catch (CDDEException& e)
	{
		App.AlertMsg(e.ErrorText());
	}
}

/******************************************************************************
** Method:		OnCommandUnadvise()
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnCommandUnadvise()
{
	ASSERT(App.m_pDDEConv != NULL);
	ASSERT(App.m_AppWnd.m_AppDlg.m_lvLinks.IsSelection());

	// Shortcut to dialog.
	CAppDlg& oDlg = App.m_AppWnd.m_AppDlg;

	// Get selected link.
	int       nSel  = oDlg.m_lvLinks.Selection();
	CDDELink* pLink = oDlg.GetLink(nSel);

	// Remove from links listview.
	oDlg.RemoveLink(nSel);

	// Destroy the link.
	App.m_pDDEConv->DestroyLink(pLink);
}

/******************************************************************************
** Method:		OnHelpAbout()
**
** Description:	Show the about dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnHelpAbout()
{
	CAboutDlg Dlg;

	Dlg.RunModal(App.m_rMainWnd);
}

/******************************************************************************
** Method:		OnUI...()
**
** Description:	UI update handlers.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnUIServerConnect()
{
	App.m_AppWnd.Menu()->EnableCmd(ID_SERVER_CONNECT, true);
}

void CAppCmds::OnUIServerDisconnect()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_SERVER_DISCONNECT, bConvOpen);
}

void CAppCmds::OnUICommandRequest()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_COMMAND_REQUEST, bConvOpen);
}

void CAppCmds::OnUICommandPoke()
{
	bool bConvOpen = false; /*(App.m_pDDEConv != NULL);*/

	App.m_AppWnd.Menu()->EnableCmd(ID_COMMAND_POKE, bConvOpen);
}

void CAppCmds::OnUICommandExecute()
{
	bool bConvOpen = false; /*(App.m_pDDEConv != NULL);*/

	App.m_AppWnd.Menu()->EnableCmd(ID_COMMAND_EXECUTE, bConvOpen);
}

void CAppCmds::OnUICommandAdvise()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_COMMAND_ADVISE, bConvOpen);
}

void CAppCmds::OnUICommandUnadvise()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bLinkSeln = (App.m_AppWnd.m_AppDlg.m_lvLinks.IsSelection());

	App.m_AppWnd.Menu()->EnableCmd(ID_COMMAND_UNADVISE, bConvOpen && bLinkSeln);
}
