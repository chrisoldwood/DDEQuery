/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		PREFSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CPrefsDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"
#include "PrefsDlg.hpp"

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

/******************************************************************************
** Method:		Default constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CPrefsDlg::CPrefsDlg()
	: CDialog(IDD_PREFS)
	, m_ebFlashTime(false, 5, 0)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_FLASH_TIME, &m_ebFlashTime)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE
}

/******************************************************************************
** Method:		OnInitDialog()
**
** Description:	Initialise the dialog.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CPrefsDlg::OnInitDialog()
{
	// Initialise controls.
	m_ebFlashTime.IntValue(App.m_nFlashTime);
}

/******************************************************************************
** Method:		OnOk()
**
** Description:	The OK button was pressed.
**
** Parameters:	None.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CPrefsDlg::OnOk()
{
	// Validate controls.
	if (m_ebFlashTime.TextLength() == 0)
	{
		AlertMsg("Please provide the 'Advise Indication Time'");
		m_ebFlashTime.Focus();
		return false;
	}

	// Save new settings.
	App.m_nFlashTime = m_ebFlashTime.IntValue();

	return true;
}
