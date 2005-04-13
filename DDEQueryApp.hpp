/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		DDEQUERYAPP.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CDDEQueryApp class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef DDEQUERYAPP_HPP
#define DDEQUERYAPP_HPP

/******************************************************************************
** 
** The application class.
**
*******************************************************************************
*/

class CDDEQueryApp : public CApp, public CDefDDEClientListener
{
public:
	//
	// Constructors/Destructor.
	//
	CDDEQueryApp();
	~CDDEQueryApp();

	//
	// Members
	//
	CAppWnd			m_AppWnd;			// Main window.
	CAppCmds		m_AppCmds;			// Command handler.

	CIniFile		m_oIniFile;			// .INI FIle
	CRect			m_rcLastPos;		// Main window position.
	CString			m_strLastService;	// Last service used.
	CString			m_strLastTopic;		// Last topic used.
	CPath			m_strLastFolder;	// The last folder used.
	CMRUList		m_oMRUList;			// The MRU list of conversations.
	uint			m_nFlashTime;		// Advise indicator "on" time.
	CRect			m_rcFullValueDlg;	// Last window size of "Full Value" dialog.
	DWORD			m_dwDDETimeOut;		// DDE call timeout.

	CDDEClient*		m_pDDEClient;		// DDE Client.
	CDDECltConv*	m_pDDEConv;			// Current conversation.

	bool			m_bInDDECall;		// Inside a DDE call?

	//
	// Format/Value helper methods.
	//
	FmtType GetFormatType(uint nFormat);
	CString GetDisplayValue(const CBuffer& oValue, uint nFormat, bool bSimple);

	//
	// Constants.
	//
	static const char* VERSION;

protected:
	//
	// Startup and Shutdown template methods.
	//
	virtual	bool OnOpen();
	virtual	bool OnClose();

	//
	// Internal methods.
	//
	void LoadConfig();
	void SaveConfig();

	//
	// Constants.
	//
	static const char* INI_FILE_VER_10;
	static const char* INI_FILE_VER_15;
	static const uint  DEF_FLASH_TIME;
	static const DWORD DEF_DDE_TIMEOUT;

	//
	// IDDEClientListener methods.
	//
	virtual void OnDisconnect(CDDECltConv* pConv);
	virtual void OnAdvise(CDDELink* pLink, const CDDEData* pData);
};

/******************************************************************************
**
** Global variables.
**
*******************************************************************************
*/

// The application object.
extern CDDEQueryApp App;

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/


#endif //DDEQUERYAPP_HPP
