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
	void OnServerExit();

	// Command menu.
	void OnCommandRequest();
	void OnCommandPoke();
	void OnCommandExecute();
	void OnCommandAdvise();
	void OnCommandUnadvise();

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
	void OnUICommandAdvise();
	void OnUICommandUnadvise();

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
