/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		APPDLG.HPP
** COMPONENT:	The Application.
** DESCRIPTION:	The CAppDlg class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef APPDLG_HPP
#define APPDLG_HPP

/******************************************************************************
** 
** This is the main application dialog.
**
*******************************************************************************
*/

class CAppDlg : public CMainDlg
{
public:
	//
	// Constructors/Destructor.
	//
	CAppDlg();
	
	//
	// Methods.
	//
	void AddLink(CDDELink* pLink);
	void UpdateLink(CDDELink* pLink, const CString& strValue);
	void RemoveLink(int nItem);
	void RemoveAllLinks();

	CDDELink* GetLink(int nItem);

	uint GetSelFormat();

	//
	// Controls.
	//
	CLabel		m_txItem;
	CComboBox	m_cbFormat;
	CEditBox	m_ebItem;
	CEditBox	m_ebValue;
	CLabel		m_txLinks;
	CListView	m_lvLinks;

protected:
	//
	// Members.
	//
	int		m_nSortCol;
	bool	m_bSortAsc;

	//
	// Message processors.
	//
	virtual void OnInitDialog();
	LRESULT OnLinksSelchange(NMHDR& oHdr);
	LRESULT OnLinksClickColumn(NMHDR& rMsgHdr);

	// Sort function.
	static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline uint CAppDlg::GetSelFormat()
{
	ASSERT(m_cbFormat.CurSel() != CB_ERR);

	return m_cbFormat.ItemData(m_cbFormat.CurSel());
}

#endif //APPDLG_HPP
