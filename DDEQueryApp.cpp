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
const char* CDDEQueryApp::VERSION      = "v1.0 [Debug]";
#else
const char* CDDEQueryApp::VERSION      = "v1.0";
#endif
const char* CDDEQueryApp::INI_FILE_VER = "1.0";

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
	CString strVer = m_oIniFile.ReadString("Version", "Version", INI_FILE_VER);

	// Read the MRU settings.
	m_strLastService = m_oIniFile.ReadString("Main", "LastService", m_strLastService);
	m_strLastTopic   = m_oIniFile.ReadString("Main", "LastTopic",   m_strLastTopic  );
	m_strLastFolder  = m_oIniFile.ReadString("Main", "LastFolder",  m_strLastFolder );

	// Read the window pos and size.
	m_rcLastPos.left   = m_oIniFile.ReadInt("UI", "Left",   0);
	m_rcLastPos.top    = m_oIniFile.ReadInt("UI", "Top",    0);
	m_rcLastPos.right  = m_oIniFile.ReadInt("UI", "Right",  0);
	m_rcLastPos.bottom = m_oIniFile.ReadInt("UI", "Bottom", 0);

	// Load MRU list.
	m_oMRUList.Load(m_oIniFile);
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
	// Write the file version.
	m_oIniFile.WriteString("Version", "Version", INI_FILE_VER);

	// Write the MRU settings.
	m_oIniFile.WriteString("Main", "LastService", m_strLastService);
	m_oIniFile.WriteString("Main", "LastTopic",   m_strLastTopic  );
	m_oIniFile.WriteString("Main", "LastFolder",  m_strLastFolder );

	// Write the window pos and size.
	m_oIniFile.WriteInt("UI", "Left",   m_rcLastPos.left  );
	m_oIniFile.WriteInt("UI", "Top",    m_rcLastPos.top   );
	m_oIniFile.WriteInt("UI", "Right",  m_rcLastPos.right );
	m_oIniFile.WriteInt("UI", "Bottom", m_rcLastPos.bottom);

	// Save MRU list.
	m_oMRUList.Save(m_oIniFile);
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
//	ASSERT(pConv == m_pDDEConv);

	// Cleanup.
	m_AppCmds.OnServerDisconnect();

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

	CString strData;

	// Get value, if set.
	if (pData != NULL)
		strData = pData->GetString();

	m_AppWnd.m_AppDlg.UpdateLink(pLink, strData);
}
