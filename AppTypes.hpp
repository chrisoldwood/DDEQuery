/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPTYPES.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	Application specific simple types.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef APPTYPES_HPP
#define APPTYPES_HPP

#if _MSC_VER > 1000
#pragma once
#endif

/******************************************************************************
**
** Clipboard format categories.
**
*******************************************************************************
*/

enum FmtType
{
	CF_TYPE_ANSI	= 1,	// ANSI text.
	CF_TYPE_UNICODE	= 2,	// Unicode text.
	CF_TYPE_BINARY	= 3,	// Binary.
};

#endif //APPTYPES
