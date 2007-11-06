/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPTOOLBAR.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CAppToolBar class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef APPTOOLBAR_HPP
#define APPTOOLBAR_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/ToolBar.hpp>
#include <WCL/CmdBtn.hpp>

/******************************************************************************
** 
** This is the applications toolbar.
**
*******************************************************************************
*/

class CAppToolBar : public CToolBar
{
public:
	//
	// Constructors/Destructor.
	//
	CAppToolBar();
	~CAppToolBar();
	
	//
	// Members.
	//
	CCmdButton	m_btnHelp;
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif //APPTOOLBAR_HPP
