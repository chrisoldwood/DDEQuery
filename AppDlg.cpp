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
	, m_nSortCol(0)
	, m_bSortAsc(false)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_ITEM_LABEL,	&m_txItem  )
		CTRL(IDC_ITEM,	        &m_ebItem  )
		CTRL(IDC_FORMAT,	    &m_cbFormat)
		CTRL(IDC_VALUE,	        &m_ebValue )
		CTRL(IDC_LINKS_LABEL,	&m_txLinks )
		CTRL(IDC_LINKS,	        &m_lvLinks )
	END_CTRL_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_ITEM_LABEL,  LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_ITEM,		  LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_FORMAT,	  RIGHT_EDGE, TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_VALUE,		  LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_LINKS_LABEL, LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_LINKS,       LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
	END_GRAVITY_TABLE

	DEFINE_CTRLMSG_TABLE
		NFY_CTRLMSG(IDC_LINKS, LVN_ITEMCHANGED, OnLinksSelchange)
		NFY_CTRLMSG(IDC_LINKS, LVN_COLUMNCLICK, OnLinksClickColumn)
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
	m_lvLinks.InsertColumn(0, "Item",    150, LVCFMT_LEFT);
	m_lvLinks.InsertColumn(1, "Value",   200, LVCFMT_LEFT);
	m_lvLinks.InsertColumn(2, "Updated", 150, LVCFMT_LEFT);

	// Populate formats combo with text-based formats.
	m_cbFormat.Add(CClipboard::FormatName(CF_TEXT),    CF_TEXT);
	m_cbFormat.Add(CClipboard::FormatName(CF_SYLK),    CF_SYLK);
	m_cbFormat.Add(CClipboard::FormatName(CF_DIF),     CF_DIF);
	m_cbFormat.Add(CClipboard::FormatName(CF_OEMTEXT), CF_OEMTEXT);

	// Select CF_TEXT by default.
	m_cbFormat.CurSel(m_cbFormat.FindExact(CClipboard::FormatName(CF_TEXT)));
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
	m_lvLinks.ItemText(n, 2, "(none)");
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

	// Format the update time.
	CString strTime = CDateTime::Current().ToString();

	// Find all items referencing the link.
	m_lvLinks.FindAllItems(pLink, vItems);

	// Update items.
	for (int i = 0; i < vItems.Size(); ++i)
	{
		m_lvLinks.ItemText(vItems[i], 1, strValue);
		m_lvLinks.ItemText(vItems[i], 2, strTime );
	}
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

/******************************************************************************
** Method:		OnLinksClickColumn()
**
** Description:	Grid column clicked, change sort column or reverse sort order.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

LRESULT CAppDlg::OnLinksClickColumn(NMHDR& rMsgHdr)
{
	NMLISTVIEW& oMsgHdr = reinterpret_cast<NMLISTVIEW&>(rMsgHdr);

	// Reverse order?
	if (m_nSortCol == oMsgHdr.iSubItem)
		m_bSortAsc = !m_bSortAsc;

	// Get the column clicked.
	m_nSortCol = oMsgHdr.iSubItem;

	// Resort.
	m_lvLinks.Sort(Compare, (LPARAM)this);

	return 0;
}

/******************************************************************************
** Method:		Compare()
**
** Description:	Compare function for sorting the listview.
**
** Parameters:	See LVM_SORTITEMS.
**
** Returns:		See LVM_SORTITEMS.
**
*******************************************************************************
*/

int CALLBACK CAppDlg::Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CDDELink* pLink1  = reinterpret_cast<CDDELink*>(lParam1);
	CDDELink* pLink2  = reinterpret_cast<CDDELink*>(lParam2);
	CAppDlg*  pDialog = reinterpret_cast<CAppDlg*>(lParamSort);
	
	// Sort by link name?
	if (pDialog->m_nSortCol == 0)
	{
		if (pDialog->m_bSortAsc)
			return strcmp(pLink1->Item(), pLink2->Item());
		else
			return strcmp(pLink2->Item(), pLink1->Item());
	}

	return 0;
}
