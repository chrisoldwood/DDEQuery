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

	CDDEClient*		m_pDDEClient;		// DDE Client.
	CDDECltConv*	m_pDDEConv;			// Current conversation.

	bool			m_bInDDECall;		// Inside a DDE call?

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
	static const char* INI_FILE_VER;

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
