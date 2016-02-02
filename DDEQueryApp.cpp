/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		DDEQUERYAPP.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CDDEQueryApp class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "DDEQueryApp.hpp"
#include <NCL/DDEClient.hpp>
#include <NCL/DDEException.hpp>
#include <NCL/DDELink.hpp>
#include <NCL/DDECltConv.hpp>
#include <NCL/DDEData.hpp>
#include <WCL/Clipboard.hpp>
#include <Core/AnsiWide.hpp>
#include <malloc.h>

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
const tchar* CDDEQueryApp::VERSION = TXT("v1.5 [Debug]");
#else
const tchar* CDDEQueryApp::VERSION = TXT("v1.5");
#endif
const tchar* CDDEQueryApp::INI_FILE_VER_10 = TXT("1.0");
const tchar* CDDEQueryApp::INI_FILE_VER_15 = TXT("1.5");
const uint   CDDEQueryApp::DEF_FLASH_TIME  = 1000;
const DWORD  CDDEQueryApp::DEF_DDE_TIMEOUT = 30000;

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
	, m_AppWnd()
	, m_AppCmds(m_AppWnd)
	, m_strLastFolder(CPath::ApplicationDir())
	, m_oMRUList(5)
	, m_nFlashTime(DEF_FLASH_TIME)
	, m_dwDDETimeOut(DEF_DDE_TIMEOUT)
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
	m_strTitle = TXT("DDE Query");

	// Load settings.
	LoadConfig();
	
	try
	{
		// Initialise the DDE client.
		m_pDDEClient = DDE::ClientPtr(new CDDEClient);
		m_pDDEClient->AddListener(this);
	}
	catch (const Core::Exception& e)
	{
		FatalMsg(TXT("%s"), e.twhat());
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
	if (!m_rcLastPos.Empty())
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
	m_pDDEClient.reset();

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
	CString strVer = m_oIniFile.ReadString(TXT("Version"), TXT("Version"), INI_FILE_VER_15);

	// Fix-up old .ini file settings.
	if (strVer == INI_FILE_VER_10)
	{
		// Read the window pos and size.
		m_rcLastPos.left   = m_oIniFile.ReadInt(TXT("UI"), TXT("Left"),   0);
		m_rcLastPos.top    = m_oIniFile.ReadInt(TXT("UI"), TXT("Top"),    0);
		m_rcLastPos.right  = m_oIniFile.ReadInt(TXT("UI"), TXT("Right"),  0);
		m_rcLastPos.bottom = m_oIniFile.ReadInt(TXT("UI"), TXT("Bottom"), 0);
	}

	// Read the MRU settings.
	m_strLastService = m_oIniFile.ReadString(TXT("Main"), TXT("LastService"), m_strLastService);
	m_strLastTopic   = m_oIniFile.ReadString(TXT("Main"), TXT("LastTopic"),   m_strLastTopic  );
	m_strLastFolder  = m_oIniFile.ReadString(TXT("Main"), TXT("LastFolder"),  m_strLastFolder );

	// Read the window last positions.
	m_rcLastPos      = m_oIniFile.ReadRect(TXT("UI"), TXT("MainWindow"), m_rcLastPos);
	m_rcFullValueDlg = m_oIniFile.ReadRect(TXT("UI"), TXT("FullValue"),  m_rcFullValueDlg);

	// Load DDE settings.
	m_dwDDETimeOut = m_oIniFile.ReadUInt(TXT("DDE"), TXT("TimeOut"), m_dwDDETimeOut);

	// Load MRU conversation list.
	m_oMRUList.Load(m_oIniFile);

	// Load advise settings.
	m_nFlashTime = m_oIniFile.ReadInt(TXT("UI"), TXT("FlashTime"), m_nFlashTime);
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
	if (m_oIniFile.ReadString(TXT("Version"), TXT("Version"), INI_FILE_VER_15) == INI_FILE_VER_10)
	{
		m_oIniFile.DeleteEntry(TXT("UI"), TXT("Left"));
		m_oIniFile.DeleteEntry(TXT("UI"), TXT("Top"));
		m_oIniFile.DeleteEntry(TXT("UI"), TXT("Right"));
		m_oIniFile.DeleteEntry(TXT("UI"), TXT("Bottom"));
	}

	// Write the file version.
	m_oIniFile.WriteString(TXT("Version"), TXT("Version"), INI_FILE_VER_15);

	// Write the MRU settings.
	m_oIniFile.WriteString(TXT("Main"), TXT("LastService"), m_strLastService);
	m_oIniFile.WriteString(TXT("Main"), TXT("LastTopic"),   m_strLastTopic  );
	m_oIniFile.WriteString(TXT("Main"), TXT("LastFolder"),  m_strLastFolder );

	// Write the window pos and size.
	m_oIniFile.WriteRect(TXT("UI"), TXT("MainWindow"), m_rcLastPos);
	m_oIniFile.WriteRect(TXT("UI"), TXT("FullValue"),  m_rcFullValueDlg);

	// Save DDE settings.
	m_oIniFile.WriteUInt(TXT("DDE"), TXT("TimeOut"), m_dwDDETimeOut);

	// Save MRU conversation list.
	m_oMRUList.Save(m_oIniFile);

	// Save advise settings.
	m_oIniFile.WriteInt(TXT("UI"), TXT("FlashTime"), m_nFlashTime);
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
	ASSERT(m_pDDEConv.get() != nullptr);

	// Cleanup, if possible.
	if (!m_bInDDECall)
		m_AppCmds.DoServerDisconnect();
	else
		m_pDDEConv.Release();

	// Notify user.
	AlertMsg(TXT("Lost connection to DDE server."));
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
	ASSERT(pLink->Conversation() == m_pDDEConv.get());

	CBuffer oData;

	// Get value, if set.
	if (pData != NULL)
		oData = pData->GetBuffer();

	m_AppWnd.m_AppDlg.UpdateLink(pLink, oData, true);
}

/******************************************************************************
** Method:		GetFormatType()
**
** Description:	Gets the basic type of the clipboard format.
**
** Parameters:	nFormat		The values' format.
**
** Returns:		The type.
**
*******************************************************************************
*/

FmtType CDDEQueryApp::GetFormatType(uint nFormat)
{
	// Standard Windows format?
	if (CClipboard::IsStdFormat(nFormat))
	{
		switch (nFormat)
		{
			case CF_TEXT:			return CF_TYPE_ANSI;
			case CF_BITMAP:			return CF_TYPE_BINARY;
			case CF_METAFILEPICT:	return CF_TYPE_BINARY;
			case CF_SYLK:			return CF_TYPE_ANSI;
			case CF_DIF:			return CF_TYPE_ANSI;
			case CF_TIFF:			return CF_TYPE_BINARY;
			case CF_OEMTEXT:		return CF_TYPE_ANSI;
			case CF_DIB:			return CF_TYPE_BINARY;
			case CF_PALETTE:		return CF_TYPE_BINARY;
			case CF_PENDATA:		return CF_TYPE_BINARY;
			case CF_RIFF:			return CF_TYPE_BINARY;
			case CF_WAVE:			return CF_TYPE_BINARY;
			case CF_UNICODETEXT:	return CF_TYPE_UNICODE;
			case CF_ENHMETAFILE:	return CF_TYPE_BINARY;
			case CF_HDROP:			return CF_TYPE_BINARY;
			case CF_LOCALE:			return CF_TYPE_BINARY;
			case CF_DIBV5:			return CF_TYPE_BINARY;
			default:				ASSERT_FALSE();
		}
	}
	// Custom format?
	else
	{
	}

	// By default, assume binary.
	return CF_TYPE_BINARY;
}

/******************************************************************************
** Method:		GetDisplayValue()
**
** Description:	Convert the buffer data into a text value for display.
**
** Parameters:	oValue		The value.
**				nFormat		The values' format.
**				bSimple		Simple conversion?
**
** Returns:		The value as text.
**
*******************************************************************************
*/

CString CDDEQueryApp::GetDisplayValue(const CBuffer& oValue, uint nFormat, bool bSimple)
{
	CString strValue;

	// Empty value?
	if (oValue.Size() == 0)
		return strValue;

	FmtType eType = GetFormatType(nFormat);

	// Simple ANSI text?
	if (eType == CF_TYPE_ANSI)
	{
		strValue = A2T(static_cast<const char*>(oValue.Buffer()));
	}
	// Unicode text?
	else if (eType == CF_TYPE_UNICODE)
	{
		strValue = W2T(static_cast<const wchar_t*>(oValue.Buffer()));
	}
	// Binary.
	else
	{
		// Single-line display?
		if (bSimple)
		{
			strValue.Format(TXT("(%u bytes)"), oValue.Size());
		}
		// Multi-line display.
		else
		{
			char* pszValue = static_cast<char*>(alloca(oValue.Size()+1));

			// Copy value to temporary buffer.
			memcpy(pszValue, oValue.Buffer(), oValue.Size());

			// Convert non-printable chars.
			for (uint i = 0; i < oValue.Size(); ++i)
			{
				uchar cChar = pszValue[i];

				// Non-printable AND also NOT Tab/CR/LF?
				if ( (!isprint(cChar)) && (cChar != '\t') && (cChar != '\r') && (cChar != '\n') )
				{
					// Replace, unless it looks like an EOS marker.
					if ( (cChar != '\0') || (i != (oValue.Size()-1)) )
						pszValue[i] = '.';
				}
			}

			// Terminate string.
			pszValue[oValue.Size()] = '\0';

			strValue = A2T(pszValue);
		}
	}

	return strValue;
}
