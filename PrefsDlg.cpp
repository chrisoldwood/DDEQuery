/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		PREFSDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CPrefsDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "PrefsDlg.hpp"
#include "DDEQueryApp.hpp"

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
	, m_ebDDETimeOut(false, 8, 0)
	, m_ebFlashTime(false, 8, 0)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_DDE_TIMEOUT, &m_ebDDETimeOut)
		CTRL(IDC_FLASH_TIME,  &m_ebFlashTime)
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
	m_ebDDETimeOut.IntValue(App.m_dwDDETimeOut);
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
	if (m_ebDDETimeOut.TextLength() == 0)
	{
		AlertMsg(TXT("Please provide the 'DDE Timeout Time'"));
		m_ebDDETimeOut.Focus();
		return false;
	}

	if (m_ebFlashTime.TextLength() == 0)
	{
		AlertMsg(TXT("Please provide the 'Advise Indication Time'"));
		m_ebFlashTime.Focus();
		return false;
	}

	// Save new settings.
	App.m_dwDDETimeOut = m_ebDDETimeOut.IntValue();
	App.m_nFlashTime   = m_ebFlashTime.IntValue();

	return true;
}
