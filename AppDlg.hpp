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

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/MainDlg.hpp>
#include <WCL/Buffer.hpp>
#include <WCL/DateTime.hpp>
#include <map>
#include <WCL/CommonUI.hpp>

// Forward declarations.
class CDDELink;

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
	CString GetItemName();
	CString GetItemValue();
	uint    GetSelFormat();

	void SetItemName(const CString& strName);
	void SetItemValue(const CBuffer& oBuffer, uint nFormat);

	void SetItemFocus();
	void SetFormatFocus();

	void AddLink(CDDELink* pLink);
	void UpdateLink(CDDELink* pLink, const CBuffer& oValue, bool bIsAdvise);
	void RemoveLink(size_t nItem);
	void RemoveAllLinks();

	bool    IsLinkSelected();
	bool    IsLinkUnadvised(CDDELink* pLink);
	CBuffer GetLinkLastValue(CDDELink* pLink);

	CDDELink* GetFirstSelLink();
	CDDELink* GetLink(size_t nItem);
	size_t    GetAllSelLinks(CListView::Items& vItems);

	void EnableUI(bool bEnable);

protected:
	/**************************************************************************
	** Structure used to hold link and last advise details.
	*/

	struct LinkData
	{
		//
		// Methods.
		//
		LinkData(CDDELink* pLink);

		void SetCurrentValue(const CBuffer& oValue);
		void SetLastAdvise(const CBuffer& oValue);

		//
		// Members.
		//
		CDDELink*	m_pLink;			// The link.
		DWORD		m_dwLastAdvise;		// Tick count at last advise.
		CDateTime	m_dtLastAdvise;		// The time of last advise.
		CBuffer		m_oLastAdvise;		// The last advise value.
		uint		m_nAdviseCount;		// The number of advises.
	};

	// Template shorthands.
	typedef std::map<CDDELink*, LinkData*> CLinksData;

	//
	// Members.
	//
	int			m_nSortCol;
	bool		m_bSortAsc;
	CLinksData	m_oLinksData;
	CString		m_strLastItem;
	CBuffer		m_oLastValue;
	uint		m_nLastFormat;
	CFont		m_font;				//!< The font to use for the controls.

	//
	// Controls.
	//
	CLabel		m_txItem;
	CComboBox	m_cbFormat;
	CEditBox	m_ebItem;
	CEditBox	m_ebValue;
	CButton		m_btnValue;
	CLabel		m_txLinks;
	CListView	m_lvLinks;

	//
	// Constants.
	//
	static const uint TIMER_ID   = 1;			// ID of "flash" timer.
	static const uint TIMER_FREQ = 100;			// Frequency of "flash" timer.

	enum Column
	{
		ITEM_NAME		= 0,
		LAST_VALUE		= 1,
		ADVISE_TIME		= 2,
		ADVISE_COUNT	= 3,
	};

	enum LinkImage
	{
		IMG_BLANK	= 0,
		IMG_FLASH	= 1,
	};

	//
	// Message processors.
	//
	virtual void OnInitDialog();
	virtual void OnDestroy();
	virtual void OnTimer(uint nTimerID);
	LRESULT OnLinksSelchange(NMHDR& oHdr);
	LRESULT OnLinksClickColumn(NMHDR& rMsgHdr);
	LRESULT OnLinksRightClick(NMHDR& oHdr);
	LRESULT OnLinksDoubleClick(NMHDR& oHdr);

	void OnItemChanged();
	void OnValueChanged();
	void OnFullValue();

	// Sort function.
	static int CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	//
	// Internal methods.
	//
	void UpdateTitle();
	void EnableFullValue(bool bEnable);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CString CAppDlg::GetItemName()
{
	return m_ebItem.Text();
}

inline CString CAppDlg::GetItemValue()
{
	return m_ebValue.Text();
}

inline void CAppDlg::SetItemName(const CString& strName)
{
	m_ebItem.Text(strName);
}

inline void CAppDlg::SetItemFocus()
{
	m_ebItem.Focus();
}

inline void CAppDlg::SetFormatFocus()
{
	m_cbFormat.Focus();
}

inline bool CAppDlg::IsLinkSelected()
{
	return m_lvLinks.IsSelection();
}

inline size_t CAppDlg::GetAllSelLinks(CListView::Items& vItems)
{
	return m_lvLinks.Selections(vItems);
}

inline CDDELink* CAppDlg::GetLink(size_t nItem)
{
	ASSERT(nItem != LB_ERR);

	return static_cast<CDDELink*>(m_lvLinks.ItemPtr(nItem));
}

inline CAppDlg::LinkData::LinkData(CDDELink* pLink)
	: m_pLink(pLink)
	, m_dwLastAdvise(::GetTickCount())
	, m_nAdviseCount(0)
{
}

inline void CAppDlg::LinkData::SetCurrentValue(const CBuffer& oValue)
{
	m_dwLastAdvise = ::GetTickCount();
	m_dtLastAdvise = CDateTime::Current();
	m_oLastAdvise  = oValue;
}

inline void CAppDlg::LinkData::SetLastAdvise(const CBuffer& oValue)
{
	m_dwLastAdvise = ::GetTickCount();
	m_dtLastAdvise = CDateTime::Current();
	m_oLastAdvise  = oValue;
	m_nAdviseCount++;
}

#endif //APPDLG_HPP
