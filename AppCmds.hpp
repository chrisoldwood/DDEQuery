/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPCMDS.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CAppCmds class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef APPCMDS_HPP
#define APPCMDS_HPP

/******************************************************************************
** 
** Application command controller.
**
*******************************************************************************
*/

class CAppCmds : public CCmdControl
{
public:
	//
	// Constructors/Destructor.
	//
	CAppCmds();
	~CAppCmds();

	//
	// Commands.
	//

	// Server menu.
	void OnServerConnect();
	void OnServerDisconnect();
	void DoServerDisconnect();
	void OnServerMRU(int nCmdID);
	void OnServerExit();
	void OnServerConnect(const CString& strService, const CString& strTopic);

	// Command menu.
	void OnCommandRequest();
	void OnCommandPoke();
	void OnCommandExecute();

	// Link menu.
	void OnLinkAdvise();
	void OnLinkUnadvise();
	void OnLinkUnadviseAll();
	void OnLinkReqValues();
	void OnLinkCopyClipboard();
	void OnLinkCopyItem();
	void OnLinkPasteItem();
	void OnLinkOpenFile();
	void OnLinkSaveFile();
	void OnLinkShowValue();

	// Options menu.
	void OnOptionsPrefs();

	// Help menu.
	void OnHelpAbout();

	//
	// UI handlers.
	//
	void OnUIServerConnect();
	void OnUIServerDisconnect();

	void OnUICommandRequest();
	void OnUICommandPoke();
	void OnUICommandExecute();

	void OnUILinkAdvise();
	void OnUILinkUnadvise();
	void OnUILinkUnadviseAll();
	void OnUILinkReqValues();
	void OnUILinkCopyClipboard();
	void OnUILinkCopyItem();
	void OnUILinkPasteItem();
	void OnUILinkOpenFile();
	void OnUILinkSaveFile();
	void OnUILinkShowValue();

	//
	// Command methods.
	//
	virtual void UpdateUI();

protected:
	//
	// Internal methods.
	//
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif //APPCMDS_HPP
