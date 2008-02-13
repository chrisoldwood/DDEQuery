/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		BINARYVIEWDLG.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CBinaryViewDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "BinaryViewDlg.hpp"
#include <WCL/Buffer.hpp>

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

CBinaryViewDlg::CBinaryViewDlg()
	: CDialog(IDD_BINARY_VIEW)
	, m_pValue(NULL)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_VALUE, &m_lbValue)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_VALUE, LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
	END_GRAVITY_TABLE
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

void CBinaryViewDlg::OnInitDialog()
{
	ASSERT(m_pValue != NULL);

	// Set control styles.
	m_lbValue.Font(CFont(ANSI_FIXED_FONT));

	// Display value.
	DisplayValue();
}

/******************************************************************************
** Method:		DisplayValue()
**
** Description:	Display the buffer as a text string.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CBinaryViewDlg::DisplayValue()
{
	// Clear old contexts.
	m_lbValue.Reset();

	const uint BYTES_PER_ROW = 8;

	// Get access to raw value.
	const byte* pBuffer = static_cast<const byte*>(m_pValue->Buffer());
	uint        nSize   = m_pValue->Size();

	// For each row of n bytes...
	for (uint r = 0; r < nSize; r += BYTES_PER_ROW)
	{
		CString strOffset, strBytes, strASCII;

		// Format the buffer offset.
		strOffset = CString::Fmt(TXT("%08X "), r*BYTES_PER_ROW);

		// Create the row of byte and ASCII values...
		for (uint c = 0, i = r; c < BYTES_PER_ROW; ++c, ++i)
		{
			if (i < nSize)
			{
				// Format the byte as a hex number and as printable ASCII.
				strBytes += CString::Fmt(TXT("%02X "), pBuffer[i]);
				strASCII += isprint(pBuffer[i]) ? pBuffer[i] : '.';
			}
			else
			{
				// Pad the row with spaces.
				strBytes += TXT("   ");
				strASCII += TXT(" ");
			}
		}

		m_lbValue.Add(strOffset + TXT("  ") + strBytes + TXT("  ") + strASCII);
	}
}
