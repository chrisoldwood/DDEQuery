/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPTOOLBAR.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppToolBar class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "AppToolBar.hpp"

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

CAppToolBar::CAppToolBar()
{
	// Define the toolbar.
	DEFINE_TOOLBAR
		TBCTL(&m_btnHelp,	ID_HELP_ABOUT)
	END_TOOLBAR
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

CAppToolBar::~CAppToolBar()
{
}
