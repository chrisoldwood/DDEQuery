/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		DDEQUERYAPP.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CDDEQueryApp class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

/******************************************************************************
**
** Global variables.
**
*******************************************************************************
*/

// "The" application object.
CDDEQueryApp App;

/******************************************************************************
**
** Class constants.
**
*******************************************************************************
*/

#ifdef _DEBUG
const char* CDDEQueryApp::VERSION      = "v1.5 Beta [Debug]";
#else
const char* CDDEQueryApp::VERSION      = "v1.5 Beta";
#endif
const char* CDDEQueryApp::INI_FILE_VER_10 = "1.0";
const char* CDDEQueryApp::INI_FILE_VER_15 = "1.5";
const uint  CDDEQueryApp::DEF_FLASH_TIME  = 1000;

/******************************************************************************
** Method:		Constructor
**
** Description:	Default constructor.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CDDEQueryApp::CDDEQueryApp()
	: CApp(m_AppWnd, m_AppCmds)
	, m_strLastFolder(CPath::ApplicationDir())
	, m_oMRUList(5)
	, m_nFlashTime(DEF_FLASH_TIME)
	, m_pDDEClient(CDDEClient::Instance())
	, m_pDDEConv(NULL)
	, m_bInDDECall(false)
{

}

/******************************************************************************
** Method:		Destructor
**
** Description:	Cleanup.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CDDEQueryApp::~CDDEQueryApp()
{
	ASSERT(!m_bInDDECall);
}

/******************************************************************************
** Method:		OnOpen()
**
** Description:	Initialises the application.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CDDEQueryApp::OnOpen()
{
	// Set the app title.
	m_strTitle = "DDE Query";

	// Load settings.
	LoadConfig();
	
	try
	{
		// Initialise the DDE client.
		m_pDDEClient->Initialise();
		m_pDDEClient->AddListener(this);
	}
	catch (CException& e)
	{
		FatalMsg(e.ErrorText());
		return false;
	}

	// Load the toolbar bitmap.
//	m_rCmdControl.CmdBitmap().LoadRsc(IDR_APPTOOLBAR);

	// Create the main window.
	if (!m_AppWnd.Create())
		return false;

	// Initialise the MRU list.
	m_oMRUList.UpdateMenu(*m_AppWnd.Menu(), ID_SERVER_MRU_1);

	// Show it.
	if (ShowNormal() && !m_rcLastPos.Empty())
		m_AppWnd.Move(m_rcLastPos);

	// Show it.
	m_AppWnd.Show(m_iCmdShow);

	// Update UI.
	m_AppCmds.UpdateUI();

	return true;
}

/******************************************************************************
** Method:		OnClose()
**
** Description:	Cleans up the application resources.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CDDEQueryApp::OnClose()
{
	// Unnitialise the DDE client.
	m_pDDEClient->RemoveListener(this);
	m_pDDEClient->Uninitialise();

	// Save settings.
	SaveConfig();

	return true;
}

/******************************************************************************
** Method:		LoadConfig()
**
** Description:	Load the app configuration.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CDDEQueryApp::LoadConfig()
{
	// Read the file version.
	CString strVer = m_oIniFile.ReadString("Version", "Version", INI_FILE_VER_15);

	// Fix-up old .ini file settings.
	if (strVer == INI_FILE_VER_10)
	{
		// Read the window pos and size.
		m_rcLastPos.left   = m_oIniFile.ReadInt("UI", "Left",   0);
		m_rcLastPos.top    = m_oIniFile.ReadInt("UI", "Top",    0);
		m_rcLastPos.right  = m_oIniFile.ReadInt("UI", "Right",  0);
		m_rcLastPos.bottom = m_oIniFile.ReadInt("UI", "Bottom", 0);
	}

	// Read the MRU settings.
	m_strLastService = m_oIniFile.ReadString("Main", "LastService", m_strLastService);
	m_strLastTopic   = m_oIniFile.ReadString("Main", "LastTopic",   m_strLastTopic  );
	m_strLastFolder  = m_oIniFile.ReadString("Main", "LastFolder",  m_strLastFolder );

	// Read the window last positions.
	m_rcLastPos      = m_oIniFile.ReadRect("UI", "MainWindow", m_rcLastPos);
	m_rcFullValueDlg = m_oIniFile.ReadRect("UI", "FullValue",  m_rcFullValueDlg);

	// Load MRU list.
	m_oMRUList.Load(m_oIniFile);

	// Load advise settings.
	m_nFlashTime = m_oIniFile.ReadInt("UI", "FlashTime", m_nFlashTime);
}

/******************************************************************************
** Method:		SaveConfig()
**
** Description:	Save the app configuration.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CDDEQueryApp::SaveConfig()
{
	// Delete old settings, if old format file.
	if (m_oIniFile.ReadString("Version", "Version", INI_FILE_VER_15) == INI_FILE_VER_10)
	{
		m_oIniFile.DeleteEntry("UI", "Left");
		m_oIniFile.DeleteEntry("UI", "Top");
		m_oIniFile.DeleteEntry("UI", "Right");
		m_oIniFile.DeleteEntry("UI", "Bottom");
	}

	// Write the file version.
	m_oIniFile.WriteString("Version", "Version", INI_FILE_VER_15);

	// Write the MRU settings.
	m_oIniFile.WriteString("Main", "LastService", m_strLastService);
	m_oIniFile.WriteString("Main", "LastTopic",   m_strLastTopic  );
	m_oIniFile.WriteString("Main", "LastFolder",  m_strLastFolder );

	// Write the window pos and size.
	m_oIniFile.WriteRect("UI", "MainWindow", m_rcLastPos);
	m_oIniFile.WriteRect("UI", "FullValue",  m_rcFullValueDlg);

	// Save MRU list.
	m_oMRUList.Save(m_oIniFile);

	// Save advise settings.
	m_oIniFile.WriteInt("UI", "FlashTime", m_nFlashTime);
}

/******************************************************************************
** Method:		OnDisconnect()
**
** Description:	Conversation terminated by the DDE server.
**
** Parameters:	pConv	The conversation terminated.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CDDEQueryApp::OnDisconnect(CDDECltConv* /*pConv*/)
{
	ASSERT(m_pDDEConv != NULL);

	// Cleanup, if possible.
	if (!m_bInDDECall)
		m_AppCmds.DoServerDisconnect();
	else
		m_pDDEConv->Disconnect();

	// Notify user.
	AlertMsg("Lost connection to DDE server.");
}

/******************************************************************************
** Method:		OnAdvise()
**
** Description:	Link updated - forward to the main dialog.
**
** Parameters:	pLink	The link updated.
**				pData	The new value.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CDDEQueryApp::OnAdvise(CDDELink* pLink, const CDDEData* pData)
{
	ASSERT(pLink->Conversation() == m_pDDEConv);

	CBuffer oData;

	// Get value, if set.
	if (pData != NULL)
		oData = pData->GetBuffer();

	m_AppWnd.m_AppDlg.UpdateLink(pLink, oData, true);
}
