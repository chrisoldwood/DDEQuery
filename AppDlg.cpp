/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.CPP
** COMPONENT:	The Application.
** DESCRIPTION:	CAppDlg class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "AppDlg.hpp"
#include "FullValueDlg.hpp"
#include "DDEQueryApp.hpp"
#include <WCL/Clipboard.hpp>
#include <NCL/DDELink.hpp>
#include <WCL/StrCvt.hpp>
#include <WCL/ScreenDC.hpp>

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
	, m_font(ANSI_FIXED_FONT)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_ITEM_LABEL,	&m_txItem  )
		CTRL(IDC_ITEM,	        &m_ebItem  )
		CTRL(IDC_FORMAT,	    &m_cbFormat)
		CTRL(IDC_VALUE,	        &m_ebValue )
		CTRL(IDC_FULL_VALUE,	&m_btnValue)
		CTRL(IDC_LINKS_LABEL,	&m_txLinks )
		CTRL(IDC_LINKS,	        &m_lvLinks )
	END_CTRL_TABLE

	DEFINE_GRAVITY_TABLE
		CTRLGRAV(IDC_ITEM_LABEL,  LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_ITEM,		  LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_FORMAT,	  RIGHT_EDGE, TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_VALUE,		  LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_FULL_VALUE,  RIGHT_EDGE, TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_LINKS_LABEL, LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, TOP_EDGE   )
		CTRLGRAV(IDC_LINKS,       LEFT_EDGE,  TOP_EDGE, RIGHT_EDGE, BOTTOM_EDGE)
	END_GRAVITY_TABLE

	DEFINE_CTRLMSG_TABLE
		NFY_CTRLMSG(IDC_LINKS,      LVN_ITEMCHANGED, &CAppDlg::OnLinksSelchange)
		NFY_CTRLMSG(IDC_LINKS,      LVN_COLUMNCLICK, &CAppDlg::OnLinksClickColumn)
		NFY_CTRLMSG(IDC_LINKS,      NM_RCLICK,       &CAppDlg::OnLinksRightClick)
		NFY_CTRLMSG(IDC_LINKS,      NM_DBLCLK,       &CAppDlg::OnLinksDoubleClick)
		CMD_CTRLMSG(IDC_ITEM,       EN_CHANGE,       &CAppDlg::OnItemChanged)
		CMD_CTRLMSG(IDC_VALUE,      EN_CHANGE,       &CAppDlg::OnValueChanged)
		CMD_CTRLMSG(IDC_FULL_VALUE, BN_CLICKED,      &CAppDlg::OnFullValue)
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
	m_btnValue.Enable(false);

	m_font.Create(CLogFont(TXT("Lucida Console"), -CScreenDC().PointSizeToPixels(10)));

	// Set edit box styles.
	m_ebItem.Font(m_font);
	m_ebValue.Font(m_font);

	// Set links listview style.
	m_lvLinks.Font(m_font);
	m_lvLinks.FullRowSelect(true);
//	m_lvLinks.GridLines(true);

	// Set the ListView icons.
	m_lvLinks.ImageList(LVSIL_SMALL, IDB_LISTICONS, 16, RGB(255, 0, 255));

	// Create links lisview columns.
	m_lvLinks.InsertColumn(ITEM_NAME,    TXT("Item"),      150, LVCFMT_LEFT);
	m_lvLinks.InsertColumn(LAST_VALUE,   TXT("Value"),     200, LVCFMT_LEFT);
	m_lvLinks.InsertColumn(ADVISE_TIME,  TXT("Updated"),   175, LVCFMT_LEFT);
	m_lvLinks.InsertColumn(ADVISE_COUNT, TXT("# Advises"), 100, LVCFMT_RIGHT);

	// Populate formats combo with text-based formats.
	m_cbFormat.Add(CClipboard::FormatName(CF_TEXT),        CF_TEXT);
	m_cbFormat.Add(CClipboard::FormatName(CF_SYLK),        CF_SYLK);
	m_cbFormat.Add(CClipboard::FormatName(CF_DIF),         CF_DIF);
	m_cbFormat.Add(CClipboard::FormatName(CF_OEMTEXT),     CF_OEMTEXT);
	m_cbFormat.Add(CClipboard::FormatName(CF_UNICODETEXT), CF_UNICODETEXT);

	// Select CF_TEXT by default.
	m_cbFormat.CurSel(m_cbFormat.FindExact(CClipboard::FormatName(CF_TEXT)));

	UpdateTitle();

	// Start advise indicator timer.
	if (App.m_nFlashTime > 0)
		StartTimer(TIMER_ID, TIMER_FREQ);
}

/******************************************************************************
** Method:		OnDestroy()
**
** Description:	The dialog is being destroyed.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnDestroy()
{
	// Stop advise indicator timer.
	StopTimer(TIMER_ID);

	// Cleanup listview.
	RemoveAllLinks();
}

/******************************************************************************
** Method:		OnTimer()
**
** Description:	Advise indicator timer fired.
**
** Parameters:	nTimerID	The timer ID.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnTimer(uint /*nTimerID*/)
{
	DWORD dwNow = ::GetTickCount();

	// Clear all stale indicators.
	for (size_t i = 0; i < m_lvLinks.ItemCount(); ++i)
	{
		// Indicator on?
		if (m_lvLinks.ItemImage(i) == IMG_FLASH)
		{
			CDDELink* pDDELink  = GetLink(i);

			ASSERT(m_oLinksData.find(pDDELink) != m_oLinksData.end());

			LinkData* pLinkData = m_oLinksData[pDDELink];

			ASSERT(pLinkData != nullptr);

			// If stale, turn indicator off.
			if ((dwNow - pLinkData->m_dwLastAdvise) > App.m_nFlashTime)
				m_lvLinks.ItemImage(i, IMG_BLANK);
		}
	}
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
	// Add the new link.
	size_t n = m_lvLinks.AppendItem(pLink->Item(), IMG_BLANK);

	m_lvLinks.ItemText(n, LAST_VALUE,   TXT("(none)"));
	m_lvLinks.ItemText(n, ADVISE_TIME,  TXT("(none)"));
	m_lvLinks.ItemText(n, ADVISE_COUNT, TXT("0"));
	m_lvLinks.ItemPtr (n, pLink);

	// Duplicate link?
	if (m_lvLinks.FindItem(pLink) != n)
	{
		size_t o = m_lvLinks.FindItem(pLink);

		// Copy value from 1st link.
		m_lvLinks.ItemText(n, LAST_VALUE,   m_lvLinks.ItemText(o, LAST_VALUE));
		m_lvLinks.ItemText(n, ADVISE_TIME,  m_lvLinks.ItemText(o, ADVISE_TIME));
		m_lvLinks.ItemText(n, ADVISE_COUNT, TXT("0"));
	}

	// Add to link data map.
	if (m_oLinksData.find(pLink) == m_oLinksData.end())
		m_oLinksData.insert(std::make_pair(pLink, new LinkData(pLink)));

	UpdateTitle();
}

/******************************************************************************
** Method:		UpdateLink()
**
** Description:	Update the specified link.
**
** Parameters:	pLink		The link.
**				oValue		The new value.
**				bIsAdvise	Is from an advise?
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::UpdateLink(CDDELink* pLink, const CBuffer& oValue, bool bIsAdvise)
{

	// Find links advise data.
	// NB: Could have just been unadvised.
	CLinksData::iterator it = m_oLinksData.find(pLink);

	if (it == m_oLinksData.end())
		return;

	LinkData* pLinkData = it->second;

	ASSERT(pLinkData != nullptr);

	// Update link data entry.
	if (bIsAdvise)
		pLinkData->SetLastAdvise(oValue);
	else
		pLinkData->SetCurrentValue(oValue);

	// Format advise details.
	CString strTime  = pLinkData->m_dtLastAdvise.ToString();
	CString strCount = CStrCvt::FormatInt(pLinkData->m_nAdviseCount);

	//Template shorthands.
	typedef CListView::Items::const_iterator CIter;

	CListView::Items vItems;

	// Find all items referencing the link.
	m_lvLinks.FindAllItems(pLink, vItems);

	// Update items.
	for (CIter oIter = vItems.begin(); oIter != vItems.end(); ++oIter)
	{
		size_t nItem = *oIter;

		m_lvLinks.ItemImage(nItem, (bIsAdvise) ? IMG_FLASH : IMG_BLANK);
		m_lvLinks.ItemText (nItem, LAST_VALUE,   App.GetDisplayValue(oValue, pLink->Format(), true));
		m_lvLinks.ItemText (nItem, ADVISE_TIME,  strTime );
		m_lvLinks.ItemText (nItem, ADVISE_COUNT, strCount);
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

void CAppDlg::RemoveLink(size_t nItem)
{
	ASSERT(nItem != LB_ERR);

	// Save current selection.
	size_t nSel = m_lvLinks.Selection();

	// Delete listview item.
	CDDELink* pDDELink = GetLink(nItem);

	m_lvLinks.DeleteItem(nItem);

	// If just deleted, adjust selected item.
	if (nSel == nItem)
	{
		if (nSel >= m_lvLinks.ItemCount())
			--nSel;

		m_lvLinks.Select(nSel);
	}

	// Last reference to link?
	if (m_lvLinks.FindItem(pDDELink) == LB_ERR)
	{
		CLinksData::iterator it = m_oLinksData.find(pDDELink);

		// Remove entry from link data map.
		if (it != m_oLinksData.end())
		{
			LinkData* pLinkData = it->second;

			delete pLinkData;

			m_oLinksData.erase(it);
		}
	}

	ASSERT(m_lvLinks.ItemCount() >= m_oLinksData.size());

	UpdateTitle();
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
	// Clear link data map.
	for (CLinksData::iterator it = m_oLinksData.begin(); it != m_oLinksData.end(); ++it)
		delete it->second;

	m_oLinksData.clear();

	// Clear listview.
	m_lvLinks.DeleteAllItems();

	UpdateTitle();
}

/******************************************************************************
** Method:		GetSelFormat()
**
** Description:	Gets the selected clipboard format.
**
** Parameters:	None.
**
** Returns:		The format handle.
**
*******************************************************************************
*/

uint CAppDlg::GetSelFormat()
{
	uint nFormat = CF_NONE;

	// Standard format selected?
	if (m_cbFormat.CurSel() != CB_ERR)
		return static_cast<uint>(m_cbFormat.ItemData(m_cbFormat.CurSel()));

	CString strFormat = m_cbFormat.Text();

	// Standard format entered?
	if ((nFormat = CClipboard::FormatHandle(strFormat)) != CF_NONE)
		return nFormat;

	// Custom format.
	return CClipboard::RegisterFormat(strFormat);
}

/******************************************************************************
** Method:		SetItemValue()
**
** Description:	Sets the value edit box with the DDE result.
**
** Parameters:	oBuffer		The value.
**				nFormat		The values' format.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::SetItemValue(const CBuffer& oBuffer, uint nFormat)
{
	m_ebValue.Text(App.GetDisplayValue(oBuffer, nFormat, true));

	m_strLastItem = m_ebItem.Text();
	m_oLastValue  = oBuffer;
	m_nLastFormat = nFormat;

	EnableFullValue(true);
}

/******************************************************************************
** Method:		IsLinkUnadvised()
**
** Description:	Queries if the link has been advised of any changes.
**
** Parameters:	pLink	The link.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CAppDlg::IsLinkUnadvised(CDDELink* pLink)
{
	ASSERT(pLink != nullptr);
	ASSERT(m_oLinksData.find(pLink) != m_oLinksData.end());

	// Query advise count in link data.
	LinkData* pLinkData = m_oLinksData[pLink];

	ASSERT(pLinkData != nullptr);

	return (pLinkData->m_nAdviseCount == 0);
}

/******************************************************************************
** Method:		GetLinkLastValue()
**
** Description:	Gets the last advised value for the link.
**
** Parameters:	pLink	The link.
**
** Returns:		The value.
**
*******************************************************************************
*/

CBuffer CAppDlg::GetLinkLastValue(CDDELink* pLink)
{
	ASSERT(pLink != nullptr);
	ASSERT(m_oLinksData.find(pLink) != m_oLinksData.end());

	// Query advise count in link data.
	LinkData* pLinkData = m_oLinksData[pLink];

	ASSERT(pLinkData != nullptr);

	return pLinkData->m_oLastAdvise;
}

/******************************************************************************
** Method:		GetFirstSelLink()
**
** Description:	Get the first selected link.
**
** Parameters:	None.
**
** Returns:		The link or nullptr, if no selection.
**
*******************************************************************************
*/

CDDELink* CAppDlg::GetFirstSelLink()
{
	CDDELink* pLink = nullptr;

	size_t nSel = m_lvLinks.Selection();

	if (nSel != LB_ERR)
		pLink = GetLink(nSel);

	return pLink;
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

	// Selectied item changed?
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

	ASSERT(pDialog->m_oLinksData.find(pLink1) != pDialog->m_oLinksData.end());
	ASSERT(pDialog->m_oLinksData.find(pLink2) != pDialog->m_oLinksData.end());
	
	LinkData* pLinkData1 = pDialog->m_oLinksData[pLink1];
	LinkData* pLinkData2 = pDialog->m_oLinksData[pLink2];

	ASSERT((pLinkData1 != nullptr) && (pLinkData2 != nullptr));

	// Sort by link name?
	if (pDialog->m_nSortCol == ITEM_NAME)
	{
		if (pDialog->m_bSortAsc)
			return tstrcmp(pLink1->Item(), pLink2->Item());
		else
			return tstrcmp(pLink2->Item(), pLink1->Item());
	}
	// Sort by last advise time?
	else if (pDialog->m_nSortCol == ADVISE_TIME)
	{
		if (pDialog->m_bSortAsc)
			return pLinkData1->m_dwLastAdvise - pLinkData2->m_dwLastAdvise;
		else
			return pLinkData2->m_dwLastAdvise - pLinkData1->m_dwLastAdvise;
	}
	// Sort by advise count?
	else if (pDialog->m_nSortCol == ADVISE_COUNT)
	{
		if (pDialog->m_bSortAsc)
			return pLinkData1->m_nAdviseCount - pLinkData2->m_nAdviseCount;
		else
			return pLinkData2->m_nAdviseCount - pLinkData1->m_nAdviseCount;
	}

	return 0;
}

/******************************************************************************
** Method:		OnLinksRightClick()
**
** Description:	Right click on the links listview, show context menu.
**
** Parameters:	oHdr	See WM_NOTIFY.
**
** Returns:		0.
**
*******************************************************************************
*/

LRESULT CAppDlg::OnLinksRightClick(NMHDR& /*oHdr*/)
{
	// Only show menu, if a selection.
	if (m_lvLinks.IsSelection())
	{
		CPopupMenu oMenu(IDR_CONTEXT);

		// Get co-ordinates of mouse click.
		const MSG& oCurrMsg = App.m_MainThread.CurrentMsg();

		// Show menu.
		uint nCmdID = oMenu.TrackMenu(App.m_AppWnd, CPoint(oCurrMsg.pt.x, oCurrMsg.pt.y));

		// Dispatch command.
		if (nCmdID != 0)
			App.m_AppWnd.PostCommand(nCmdID);
	}

	return 0;
}

/******************************************************************************
** Method:		OnLinksDoubleClick()
**
** Description:	Double-click on the links listview.
**
** Parameters:	oHdr	See WM_NOTIFY.
**
** Returns:		0.
**
*******************************************************************************
*/

LRESULT CAppDlg::OnLinksDoubleClick(NMHDR& /*oHdr*/)
{
	// If a selection, exec "Display Full Value".
	if (m_lvLinks.IsSelection())
		App.m_AppWnd.PostCommand(ID_LINK_SHOW_VALUE);

	return 0;
}

/******************************************************************************
** Method:		EnableUI()
**
** Description:	Enable or Disable the UI.
**
** Parameters:	bEnable		New state.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::EnableUI(bool bEnable)
{
	m_ebItem.ReadOnly(!bEnable);
	m_cbFormat.Enable(bEnable);
	m_ebValue.ReadOnly(!bEnable);
	m_btnValue.Enable(bEnable && !m_strLastItem.Empty());
	m_lvLinks.Enable(bEnable);
}

/******************************************************************************
** Method:		UpdateTitle()
**
** Description:	Update the links title.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::UpdateTitle()
{
	CString strTitle = TXT("Active Links");

	size_t count = m_lvLinks.ItemCount();

	if (count != 0)
		strTitle += TXT(" [") + CStrCvt::FormatInt(static_cast<int>(count)) + TXT("]");

	m_txLinks.Title(strTitle);
}

/******************************************************************************
** Method:		EnableFullValue()
**
** Description:	Enable or Disable the "Full Value" button.
**
** Parameters:	bEnable		The new state.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::EnableFullValue(bool bEnable)
{
	// Check whole UI is not disabled.
	bEnable = bEnable && m_ebValue.IsEnabled();

	m_btnValue.Enable(bEnable);
}

/******************************************************************************
** Method:		OnFullValue()
**
** Description:	Show the Items value in full.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnFullValue()
{
	if (m_strLastItem.Empty())
		return;

	CFullValueDlg oDlg;

	oDlg.m_strItem = m_strLastItem;
	oDlg.m_oValue  = m_oLastValue;
	oDlg.m_nFormat = m_nLastFormat;

	oDlg.RunModal(*this);
}

/******************************************************************************
** Method:		OnItemChanged()
**
** Description:	The Item field has been changed.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnItemChanged()
{
	// Update state of full value button.
	if (!m_strLastItem.Empty())
	{
		m_strLastItem = TXT("");
		EnableFullValue(false);
	}
}

/******************************************************************************
** Method:		OnValueChanged()
**
** Description:	The Value field has been changed.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CAppDlg::OnValueChanged()
{
	// Update state of full value button.
	if (!m_strLastItem.Empty())
	{
		m_strLastItem = TXT("");
		EnableFullValue(false);
	}
}
