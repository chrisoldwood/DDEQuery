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
**
** Local variables.
**
*******************************************************************************
*/

static char szTXTExts[] = {	"Text Files (*.txt)\0*.txt\0"
							"All Files (*.*)\0*.*\0"
							"\0\0"							};

static char szTXTDefExt[] = { "txt" };

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
		CMD_RANGE(ID_SERVER_MRU_1,
				  ID_SERVER_MRU_5,		OnServerMRU,			NULL,					-1) 
		CMD_ENTRY(ID_SERVER_EXIT,		OnServerExit,			NULL,					-1)
		// Command menu.
		CMD_ENTRY(ID_COMMAND_REQUEST,	OnCommandRequest,		OnUICommandRequest,		-1)
		CMD_ENTRY(ID_COMMAND_POKE,		OnCommandPoke,			OnUICommandPoke,		-1)
		CMD_ENTRY(ID_COMMAND_EXECUTE,	OnCommandExecute,		OnUICommandExecute,		-1)
		// Link menu.
		CMD_ENTRY(ID_LINK_ADVISE,		OnLinkAdvise,			OnUILinkAdvise,			-1)
		CMD_ENTRY(ID_LINK_UNADVISE,		OnLinkUnadvise,			OnUILinkUnadvise,		-1)
		CMD_ENTRY(ID_LINK_FILE,			OnLinkFile,				OnUILinkFile,			-1)
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
		OnServerConnect(Dlg.m_strService, Dlg.m_strTopic);
}

/******************************************************************************
** Method:		OnServerDisconnect()
**
** Description:	User closing the connection to the server.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnServerDisconnect()
{
	// Inside DDE call?
	if ((App.m_pDDEConv != NULL) && (App.m_bInDDECall))
	{
		App.m_AppWnd.AlertMsg("You cannot close the connection while a request is in progress.");
		return;
	}

	DoServerDisconnect();
}

/******************************************************************************
** Method:		DoServerDisconnect()
**
** Description:	Close the connection to the server.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::DoServerDisconnect()
{
	// Conversation open.
	if (App.m_pDDEConv != NULL)
	{
		App.m_pDDEClient->DestroyConversation(App.m_pDDEConv);

		App.m_pDDEConv = NULL;
	}

	// Clear the links listview.
	App.m_AppWnd.m_AppDlg.RemoveAllLinks();

	UpdateUI();
	App.m_AppWnd.UpdateTitle();
}

/******************************************************************************
** Method:		OnServerMRU()
**
** Description:	Open a connection from the MRU list.
**
** Parameters:	nCmdID	The ID of the command.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnServerMRU(int nCmdID)
{
	CStrArray astrFields;

	// Get the MRU item.
	CString strConv = App.m_oMRUList[nCmdID - ID_SERVER_MRU_1];

	// Split "Service|Topic" into separate strings.
	if (CStrTok::Split(strConv, "|", astrFields) != 2)
	{
		App.AlertMsg("Invalid MRU entry: %s", strConv);
		return;
	}

	// Call main connection handler.
	OnServerConnect(astrFields[0], astrFields[1]);
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
** Method:		OnServerConnect()
**
** Description:	Open a DDE conversation for the specified service and topic.
**
** Parameters:	strService		The service name.
**				strTopc			The topic name.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnServerConnect(const CString& strService, const CString& strTopic)
{
	CBusyCursor oCursor;

	try
	{
		// Close existing conversation.
		OnServerDisconnect();

		ASSERT(App.m_pDDEConv == NULL);

		CAutoBool oLock(&App.m_bInDDECall);

		// Create conversation.
		App.m_pDDEConv = App.m_pDDEClient->CreateConversation(strService, strTopic);

		// Remember new settings.
		App.m_strLastService = strService;
		App.m_strLastTopic   = strTopic;

		// Set focus to Item field.
		App.m_AppWnd.m_AppDlg.m_ebItem.Focus();

		// Update the MRU list.
		App.m_oMRUList.Add(strService + "|" + strTopic);
		App.m_oMRUList.UpdateMenu(*App.m_AppWnd.Menu(), ID_SERVER_MRU_1);
	}
	catch (CDDEException& e)
	{
		OnServerDisconnect();
		App.AlertMsg(e.ErrorText());
	}

	UpdateUI();
	App.m_AppWnd.UpdateTitle();
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
	CString strItem   = App.m_AppWnd.m_AppDlg.m_ebItem.Text();
	uint    nFormat   = App.m_AppWnd.m_AppDlg.GetSelFormat();

	// Item provided?
	if (strItem == "")
	{
		App.AlertMsg("Please provide the item name.");
		App.m_AppWnd.m_AppDlg.m_ebItem.Focus();
		return;
	}

	try
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Make the request.
		CDDEData oData = App.m_pDDEConv->Request(strItem, nFormat);

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
** Description:	Poke a value into an item.
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

	// Get the DDE item and value.
	CString strItem  = App.m_AppWnd.m_AppDlg.m_ebItem.Text();
	CString strValue = App.m_AppWnd.m_AppDlg.m_ebValue.Text();

	// Item provided?
	if (strItem == "")
	{
		App.AlertMsg("Please provide the item name.");
		App.m_AppWnd.m_AppDlg.m_ebItem.Focus();
		return;
	}

	try
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Poke the value.
		App.m_pDDEConv->Poke(strItem, strValue);
	}
	catch (CDDEException& e)
	{
		App.AlertMsg(e.ErrorText());
	}
}

/******************************************************************************
** Method:		OnCommandExecute()
**
** Description:	Execute a command.
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

	// Get the DDE command.
	CString strCmd = App.m_AppWnd.m_AppDlg.m_ebItem.Text();

	// Command provided?
	if (strCmd == "")
	{
		App.AlertMsg("Please provide the command.");
		App.m_AppWnd.m_AppDlg.m_ebItem.Focus();
		return;
	}

	try
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Execute it.
		App.m_pDDEConv->Execute(strCmd);
	}
	catch (CDDEException& e)
	{
		App.AlertMsg(e.ErrorText());
	}
}

/******************************************************************************
** Method:		OnLinkAdvise()
**
** Description:	Link to an item.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkAdvise()
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
		CAutoBool oLock(&App.m_bInDDECall);

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
** Method:		OnLinkUnadvise()
**
** Description:	Unlink from an item.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkUnadvise()
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

	CAutoBool oLock(&App.m_bInDDECall);

	// Destroy the link.
	App.m_pDDEConv->DestroyLink(pLink);
}

/******************************************************************************
** Method:		OnLinkFile()
**
** Description:	Link to items specified in a file.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkFile()
{
	ASSERT(App.m_pDDEConv != NULL);

	CPath strPath;

	// Select the filename.
	if (!strPath.Select(App.m_AppWnd, CPath::OpenFile, szTXTExts, szTXTDefExt, App.m_strLastFolder))
		return;

	CStrArray astrLinks;

	try
	{
		CFile oFile;

		// Open, for reading.
		oFile.Open(strPath, GENERIC_READ);

		// For all lines...
		while (!oFile.IsEOF())
		{
			CString strLine = oFile.ReadLine();

			// Ignore empty lines and comments.
			if ((strLine == "") || (strLine[0] == '#') || (strLine[0] == ';'))
				continue;

			astrLinks.Add(strLine);
		}

		// Close.
		oFile.Close();

		// Remember folder used.
		App.m_strLastFolder = strPath.Directory();
	}
	catch(CFileException& e)
	{
		// Notify user.
		App.AlertMsg(e.ErrorText());
		return;
	}

	uint nErrors = astrLinks.Size();

	// Create links...
	for (int i = 0; i < astrLinks.Size(); ++i)
	{
		// Conversation terminated?
		if (App.m_pDDEConv == NULL)
		{
			App.AlertMsg("Lost connection to DDE server.");
			return;
		}

		try
		{
			CAutoBool oLock(&App.m_bInDDECall);

			// Create the link.
			CDDELink* pLink = App.m_pDDEConv->CreateLink(astrLinks[i]);

			// Add to the links listview.
			App.m_AppWnd.m_AppDlg.AddLink(pLink);

			--nErrors;
		}
		catch(CDDEException& /*e*/)
		{
		}
	}

	// Report any errors.
	if (nErrors > 0)
		App.AlertMsg("Failed to create %d links.", nErrors);
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
** Method:		UpdateUI()
**
** Description:	Updates the main dialog controls.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::UpdateUI()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.m_AppDlg.m_ebItem.ReadOnly(!bConvOpen);
	App.m_AppWnd.m_AppDlg.m_cbFormat.Enable(bConvOpen);
	App.m_AppWnd.m_AppDlg.m_ebValue.ReadOnly(!bConvOpen);
	App.m_AppWnd.m_AppDlg.m_lvLinks.Enable(bConvOpen);

	// Invoke specific commmand handlers.
	CCmdControl::UpdateUI();
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
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_COMMAND_POKE, bConvOpen);
}

void CAppCmds::OnUICommandExecute()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_COMMAND_EXECUTE, bConvOpen);
}

void CAppCmds::OnUILinkAdvise()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_ADVISE, bConvOpen);
}

void CAppCmds::OnUILinkUnadvise()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bLinkSeln = (App.m_AppWnd.m_AppDlg.m_lvLinks.IsSelection());

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_UNADVISE, bConvOpen && bLinkSeln);
}

void CAppCmds::OnUILinkFile()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_FILE, bConvOpen);
}
