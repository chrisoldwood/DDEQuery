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

	//
	// Controls.
	//
	CLabel		m_txItem;
	CEditBox	m_ebItem;
	CEditBox	m_ebValue;
	CLabel		m_txLinks;
	CListView	m_lvLinks;

protected:
	//
	// Message processors.
	//
	virtual void OnInitDialog();
	LRESULT OnLinksSelchange(NMHDR& oHdr);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif //APPDLG_HPP
