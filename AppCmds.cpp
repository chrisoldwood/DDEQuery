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
#include "ConnectDlg.hpp"
#include "FullValueDlg.hpp"
#include "PrefsDlg.hpp"
#include "AboutDlg.hpp"

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
		CMD_ENTRY(ID_SERVER_CONNECT,		OnServerConnect,		OnUIServerConnect,		-1)
		CMD_ENTRY(ID_SERVER_DISCONNECT,		OnServerDisconnect,		OnUIServerDisconnect,	-1)
		CMD_RANGE(ID_SERVER_MRU_1,
				  ID_SERVER_MRU_5,			OnServerMRU,			NULL,					-1) 
		CMD_ENTRY(ID_SERVER_EXIT,			OnServerExit,			NULL,					-1)
		// Command menu.
		CMD_ENTRY(ID_COMMAND_REQUEST,		OnCommandRequest,		OnUICommandRequest,		-1)
		CMD_ENTRY(ID_COMMAND_POKE,			OnCommandPoke,			OnUICommandPoke,		-1)
		CMD_ENTRY(ID_COMMAND_EXECUTE,		OnCommandExecute,		OnUICommandExecute,		-1)
		// Link menu.
		CMD_ENTRY(ID_LINK_ADVISE,			OnLinkAdvise,			OnUILinkAdvise,			-1)
		CMD_ENTRY(ID_LINK_UNADVISE,			OnLinkUnadvise,			OnUILinkUnadvise,		-1)
		CMD_ENTRY(ID_LINK_UNADVISE_ALL,		OnLinkUnadviseAll,		OnUILinkUnadviseAll,	-1)
		CMD_ENTRY(ID_LINK_REQ_VALUES,		OnLinkReqValues,		OnUILinkReqValues,		-1)
		CMD_ENTRY(ID_LINK_COPY_CLIPBOARD,	OnLinkCopyClipboard,	OnUILinkCopyClipboard,	-1)
		CMD_ENTRY(ID_LINK_COPY_ITEM,		OnLinkCopyItem,			OnUILinkCopyItem,		-1)
		CMD_ENTRY(ID_LINK_PASTE_ITEM,		OnLinkPasteItem,		OnUILinkPasteItem,		-1)
		CMD_ENTRY(ID_LINK_OPEN_FILE,		OnLinkOpenFile,			OnUILinkOpenFile,		-1)
		CMD_ENTRY(ID_LINK_SAVE_FILE,		OnLinkSaveFile,			OnUILinkSaveFile,		-1)
		CMD_ENTRY(ID_LINK_SHOW_VALUE,		OnLinkShowValue,		OnUILinkShowValue,		-1)
		// Options menu.
		CMD_ENTRY(ID_OPTIONS_PREFS,			OnOptionsPrefs,			NULL,					-1)
		// Help menu.
		CMD_ENTRY(ID_HELP_ABOUT,			OnHelpAbout,			NULL,					10)
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

		// Update status bar.
		App.m_AppWnd.ShowFlashIcon(true);

		// Create conversation.
		App.m_pDDEConv = App.m_pDDEClient->CreateConversation(strService, strTopic);

		// Remember new settings.
		App.m_strLastService = strService;
		App.m_strLastTopic   = strTopic;

		// Set focus to Item field.
		App.m_AppWnd.m_AppDlg.Focus();

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
	App.m_AppWnd.ShowFlashIcon(false);
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
	CString strItem   = App.m_AppWnd.m_AppDlg.GetItemName();
	uint    nFormat   = App.m_AppWnd.m_AppDlg.GetSelFormat();

	// Item provided?
	if (strItem == "")
	{
		App.AlertMsg("Please provide the item name.");
		App.m_AppWnd.m_AppDlg.SetItemFocus();
		return;
	}

	try
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Update status bar.
		App.m_AppWnd.ShowFlashIcon(true);

		// Make the request.
		CDDEData oData = App.m_pDDEConv->Request(strItem, nFormat);

		// Display value.
		App.m_AppWnd.m_AppDlg.SetItemValue(oData.GetBuffer());
	}
	catch (CDDEException& e)
	{
		// Cleanup, if disconnected.
		if (!App.m_pDDEConv->IsConnected())
			DoServerDisconnect();

		App.AlertMsg(e.ErrorText());
	}

	App.m_AppWnd.ShowFlashIcon(false);
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
	CString strItem  = App.m_AppWnd.m_AppDlg.GetItemName();
	CString strValue = App.m_AppWnd.m_AppDlg.GetItemValue();

	// Item provided?
	if (strItem == "")
	{
		App.AlertMsg("Please provide the item name.");
		App.m_AppWnd.m_AppDlg.SetItemFocus();
		return;
	}

	try
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Update status bar.
		App.m_AppWnd.ShowFlashIcon(true);

		// Poke the value.
		App.m_pDDEConv->Poke(strItem, strValue);
	}
	catch (CDDEException& e)
	{
		// Cleanup, if disconnected.
		if (!App.m_pDDEConv->IsConnected())
			DoServerDisconnect();

		App.AlertMsg(e.ErrorText());
	}

	App.m_AppWnd.ShowFlashIcon(false);
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
	CString strCmd = App.m_AppWnd.m_AppDlg.GetItemName();

	// Command provided?
	if (strCmd == "")
	{
		App.AlertMsg("Please provide the command.");
		App.m_AppWnd.m_AppDlg.SetItemFocus();
		return;
	}

	try
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Update status bar.
		App.m_AppWnd.ShowFlashIcon(true);

		// Execute it.
		App.m_pDDEConv->Execute(strCmd);
	}
	catch (CDDEException& e)
	{
		// Cleanup, if disconnected.
		if (!App.m_pDDEConv->IsConnected())
			DoServerDisconnect();

		App.AlertMsg(e.ErrorText());
	}

	App.m_AppWnd.ShowFlashIcon(false);
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
	CString strItem = App.m_AppWnd.m_AppDlg.GetItemName();
	uint    nFormat = CF_TEXT;

	// Item provided?
	if (strItem == "")
	{
		App.AlertMsg("Please provide the item name.");
		App.m_AppWnd.m_AppDlg.SetItemFocus();
		return;
	}

	try
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Update status bar.
		App.m_AppWnd.ShowFlashIcon(true);

		// Create the link.
		CDDELink* pLink = App.m_pDDEConv->CreateLink(strItem, nFormat);

		// Add to the links listview.
		App.m_AppWnd.m_AppDlg.AddLink(pLink);
	}
	catch (CDDEException& e)
	{
		// Cleanup, if disconnected.
		if (!App.m_pDDEConv->IsConnected())
			DoServerDisconnect();

		App.AlertMsg(e.ErrorText());
	}

	UpdateUI();
	App.m_AppWnd.ShowFlashIcon(false);
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
	ASSERT(App.m_AppWnd.m_AppDlg.IsLinkSelected());

	// Shortcut to dialog.
	CAppDlg& oDlg = App.m_AppWnd.m_AppDlg;

	CListView::CUIntArray vItems;

	// Get selected items.
	if (oDlg.GetAllSelLinks(vItems) > 0)
	{
		CAutoBool oLock(&App.m_bInDDECall);

		// Update status bar.
		App.m_AppWnd.ShowFlashIcon(true);

		// Destroy all selected links...
		for (int i = 0; i < vItems.Size(); ++i)
		{
			CDDELink* pLink = oDlg.GetLink(vItems[i]);

			// Destroy the link.
			App.m_pDDEConv->DestroyLink(pLink);
		}

		// Remove all selected links...
		for (int i = vItems.Size()-1; i >= 0; --i)
			oDlg.RemoveLink(vItems[i]);

		UpdateUI();
		App.m_AppWnd.ShowFlashIcon(false);
	}
}

/******************************************************************************
** Method:		OnLinkUnadviseAll()
**
** Description:	Unlink all items.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkUnadviseAll()
{
	ASSERT(App.m_pDDEConv != NULL);

	// Remove all links from links listview.
	App.m_AppWnd.m_AppDlg.RemoveAllLinks();

	CAutoBool oLock(&App.m_bInDDECall);

	// Update status bar.
	App.m_AppWnd.ShowFlashIcon(true);

	// Destroy all links.
	App.m_pDDEConv->DestroyAllLinks();

	UpdateUI();
	App.m_AppWnd.ShowFlashIcon(false);
}

/******************************************************************************
** Method:		OnLinkReqValues()
**
** Description:	Request current values for unadvised links.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkReqValues()
{
	ASSERT(App.m_pDDEConv != NULL);

	CDDECltLinks vLinks;

	App.m_pDDEConv->GetAllLinks(vLinks);

	// Update status bar.
	App.m_AppWnd.ShowFlashIcon(true);

	// For all links...
	for (int i = 0; i < vLinks.Size(); ++i) 
	{
		CDDELink* pLink = vLinks[i];

		try
		{
			CAutoBool oLock(&App.m_bInDDECall);

			// Link not advised?
			if (App.m_AppWnd.m_AppDlg.IsLinkUnadvised(pLink))
			{
				CDDEData oData = App.m_pDDEConv->Request(pLink->Item(), pLink->Format());

				App.m_AppWnd.m_AppDlg.UpdateLink(pLink, oData.GetBuffer(), false);
			}
		}
		catch (CDDEException& /*e*/)
		{
		}
	}

	App.m_AppWnd.ShowFlashIcon(false);
}

/******************************************************************************
** Method:		OnLinkCopy()
**
** Description:	Copy a link to the clipboard for the selected link.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkCopyClipboard()
{
	ASSERT(App.m_pDDEConv != NULL);
	ASSERT(App.m_AppWnd.m_AppDlg.IsLinkSelected());

	// Shortcut to dialog.
	CAppDlg& oDlg = App.m_AppWnd.m_AppDlg;

	// Get selected link.
	CDDELink* pLink = oDlg.GetFirstSelLink();

	ASSERT(pLink != NULL);

	// Copy to clipboard.
	CDDELink::CopyLink(App.m_AppWnd.Handle(), pLink);
}

/******************************************************************************
** Method:		OnLinkCopyItem()
**
** Description:	Copy a link to the Item field for the selected link.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkCopyItem()
{
	ASSERT(App.m_pDDEConv != NULL);
	ASSERT(App.m_AppWnd.m_AppDlg.IsLinkSelected());

	// Shortcut to dialog.
	CAppDlg& oDlg = App.m_AppWnd.m_AppDlg;

	// Get selected link.
	CDDELink* pLink = oDlg.GetFirstSelLink();

	ASSERT(pLink != NULL);

	// Copy to Item field.
	oDlg.SetItemName(pLink->Item());
}

/******************************************************************************
** Method:		OnLinkPasteItem()
**
** Description:	Paste a link from the clipboard into the "Item" control.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkPasteItem()
{
	ASSERT(App.m_pDDEConv != NULL);

	CString strService, strTopic, strItem;

	// Copy "Item" only to view.
	if (CDDELink::PasteLink(strService, strTopic, strItem))
		App.m_AppWnd.m_AppDlg.SetItemName(strItem);

	UpdateUI();
}

/******************************************************************************
** Method:		OnLinkOpenFile()
**
** Description:	Load links from a file.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkOpenFile()
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

	// Update status bar.
	App.m_AppWnd.ShowFlashIcon(true);

	// Create links...
	for (int i = 0; i < astrLinks.Size(); ++i)
	{
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
			// Cleanup, if disconnected.
			if (!App.m_pDDEConv->IsConnected())
			{
				DoServerDisconnect();
				return;
			}
		}
	}

	App.m_AppWnd.ShowFlashIcon(false);

	// Report any errors.
	if (nErrors > 0)
		App.AlertMsg("Failed to create %d links.", nErrors);

	UpdateUI();
}

/******************************************************************************
** Method:		OnLinkSaveFile()
**
** Description:	Save links to a file.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkSaveFile()
{
	ASSERT(App.m_pDDEConv != NULL);

	CPath strPath;

	// Select the filename.
	if (!strPath.Select(App.m_AppWnd, CPath::SaveFile, szTXTExts, szTXTDefExt, App.m_strLastFolder))
		return;

	try
	{
		CFile oFile;

		// Open, for reading.
		oFile.Create(strPath);

		// Write header.
		oFile.WriteLine(CString::Fmt("# %s|%s", App.m_pDDEConv->Service(), App.m_pDDEConv->Topic()));

		// For all links...
		for (int i = 0; i < App.m_pDDEConv->NumLinks(); ++i)
			oFile.WriteLine(App.m_pDDEConv->GetLink(i)->Item());

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
}

/******************************************************************************
** Method:		OnLinkShowValue()
**
** Description:	Display the full value for the last advise on the first
**				selected link.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnLinkShowValue()
{
	ASSERT(App.m_pDDEConv != NULL);
	ASSERT(App.m_AppWnd.m_AppDlg.IsLinkSelected());

	// Shortcut to dialog.
	CAppDlg& oDlg = App.m_AppWnd.m_AppDlg;

	// Get selected link.
	CDDELink* pLink = oDlg.GetFirstSelLink();

	ASSERT(pLink != NULL);

	CFullValueDlg Dlg;

	Dlg.m_strItem = pLink->Item();
	Dlg.m_oValue  = oDlg.GetLinkLastValue(pLink);

	// Show the dialog.
	Dlg.RunModal(App.m_rMainWnd);
}

/******************************************************************************
** Method:		OnOptionsPrefs()
**
** Description:	Show the settings dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppCmds::OnOptionsPrefs()
{
	CPrefsDlg Dlg;

	Dlg.RunModal(App.m_rMainWnd);
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

	App.m_AppWnd.m_AppDlg.EnableUI(bConvOpen);

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
	bool bLinkSeln = (App.m_AppWnd.m_AppDlg.IsLinkSelected());

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_UNADVISE, bConvOpen && bLinkSeln);
}

void CAppCmds::OnUILinkUnadviseAll()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bAnyLinks = ((bConvOpen) && (App.m_pDDEConv->NumLinks() > 0));

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_UNADVISE_ALL, bConvOpen && bAnyLinks);
}

void CAppCmds::OnUILinkReqValues()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bAnyLinks = ((bConvOpen) && (App.m_pDDEConv->NumLinks() > 0));

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_REQ_VALUES, bConvOpen && bAnyLinks);
}

void CAppCmds::OnUILinkCopyClipboard()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bLinkSeln = (App.m_AppWnd.m_AppDlg.IsLinkSelected());

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_COPY_CLIPBOARD, bConvOpen && bLinkSeln);
}

void CAppCmds::OnUILinkCopyItem()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bLinkSeln = (App.m_AppWnd.m_AppDlg.IsLinkSelected());

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_COPY_ITEM, bConvOpen && bLinkSeln);
}

void CAppCmds::OnUILinkPasteItem()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_PASTE_ITEM, bConvOpen);
}

void CAppCmds::OnUILinkOpenFile()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_OPEN_FILE, bConvOpen);
}

void CAppCmds::OnUILinkSaveFile()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bAnyLinks = ((bConvOpen) && (App.m_pDDEConv->NumLinks() > 0));

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_SAVE_FILE, bConvOpen && bAnyLinks);
}

void CAppCmds::OnUILinkShowValue()
{
	bool bConvOpen = (App.m_pDDEConv != NULL);
	bool bLinkSeln = (App.m_AppWnd.m_AppDlg.IsLinkSelected());

	App.m_AppWnd.Menu()->EnableCmd(ID_LINK_SHOW_VALUE, bConvOpen && bLinkSeln);
}
