/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppDlg class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

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

CAppDlg::CAppDlg()
	: CMainDlg(IDD_MAIN)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_ITEM_LABEL,	&m_txItem )
		CTRL(IDC_ITEM,	        &m_ebItem )
		CTRL(IDC_VALUE,	        &m_ebValue)
		CTRL(IDC_LINKS_LABEL,	&m_txLinks)
		CTRL(IDC_LINKS,	        &m_lvLinks)
	END_CTRL_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_ITEM_LABEL,  LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_ITEM,		  LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_VALUE,		  LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_LINKS_LABEL, LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_LINKS,       LEFT_EDGE, TOP_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
	END_GRAVITY_TABLE

	DEFINE_CTRLMSG_TABLE
		NFY_CTRLMSG(IDC_LINKS, LVN_ITEMCHANGED, OnLinksSelchange)
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

void CAppDlg::OnInitDialog()
{
	// Set edit box styles.
	m_ebItem.Font(CFont(ANSI_FIXED_FONT));
	m_ebValue.Font(CFont(ANSI_FIXED_FONT));

	// Set links listview style.
	m_lvLinks.Font(CFont(ANSI_FIXED_FONT));
	m_lvLinks.FullRowSelect(true);
//	m_lvLinks.GridLines(true);

	// Create links lisview columns.
	m_lvLinks.InsertColumn(0, "Item",  150, LVCFMT_LEFT);
	m_lvLinks.InsertColumn(1, "Value", 200, LVCFMT_LEFT);
}

/******************************************************************************
** Method:		AddLink()
**
** Description:	Adds a link to the view.
**
** Parameters:	pLink	The link to add.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::AddLink(CDDELink* pLink)
{
	int n = m_lvLinks.AppendItem(pLink->Item());

	m_lvLinks.ItemText(n, 1, "(none)");
	m_lvLinks.ItemPtr (n,    pLink);
}

/******************************************************************************
** Method:		UpdateLink()
**
** Description:	Update the specified link.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::UpdateLink(CDDELink* pLink, const CString& strValue)
{
	CListView::CUIntArray vItems;

	// Find all items referencing the link.
	m_lvLinks.FindAllItems(pLink, vItems);

	// Update items.
	for (int i = 0; i < vItems.Size(); ++i)
		m_lvLinks.ItemText(vItems[i], 1, strValue);
}

/******************************************************************************
** Method:		RemoveLink()
**
** Description:	Remove a single link.
**
** Parameters:	nItem	The item to remove.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::RemoveLink(int nItem)
{
	ASSERT(nItem != LB_ERR);

	m_lvLinks.DeleteItem(nItem);
}

/******************************************************************************
** Method:		RemoveAllLinks()
**
** Description:	Remove all links.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::RemoveAllLinks()
{
	m_lvLinks.DeleteAllItems();
}

/******************************************************************************
** Method:		GetLink()
**
** Description:	Get the link referenced by the listview item.
**
** Parameters:	nItem	The item to query.
**
** Returns:		The link.
**
*******************************************************************************
*/

CDDELink* CAppDlg::GetLink(int nItem)
{
	ASSERT(nItem != LB_ERR);

	return static_cast<CDDELink*>(m_lvLinks.ItemPtr(nItem));
}

/******************************************************************************
** Method:		OnLinksSelchange()
**
** Description:	Link (de)selected, update the main UI.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

LRESULT CAppDlg::OnLinksSelchange(NMHDR& oNMHdr)
{
	NMLISTVIEW& oMsgHdr = reinterpret_cast<NMLISTVIEW&>(oNMHdr);

	if (oMsgHdr.uChanged & LVIF_STATE)
		App.m_AppCmds.UpdateUI();

	return 0;
}
