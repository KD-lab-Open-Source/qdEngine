/********************************************************************
	created:	2002/12/09
	created:	9:12:2002   17:29
	filename: 	d:\projects\quest\questeditor\mwwizard.h
	file path:	d:\projects\quest\questeditor
	file base:	mwwizard
	file ext:	h
	Powered by:	�����
	
	purpose:	������� ����� ��� �������, ������������ � ������ �������
*********************************************************************/

#ifndef __MW_WIZARD_HEADER__
#define __MW_WIZARD_HEADER__

#include <algorithm>

class MWBasePage;

typedef std::list<MWBasePage*> MWPagesList_t;
/**
	������� ����� ��� �������, ������������ � ������ �������
 */
class MWWizard {
	/// ���������. ����� ��������� � ������ ��������
	CString strCaption;
	/// ������ ������� ������������� �������
	MWPagesList_t pages;
	/// ������� ��������
	MWBasePage* m_cur_page;
	/// ���� � ������
	HTREEITEM hItem;
	/// ���� �� ������� �������� � �������� ������
	bool m_bChanges;
public:
	MWWizard():m_cur_page(NULL), hItem(NULL),m_bChanges(false){
	}
	virtual ~MWWizard()
	{}

	/// ������������� ������
	/**
		������� �������� ��� ������ ���� ��������� ����������
		\return true ���� ��� ������� ����������
	 */
	virtual bool SuspendWizard(){return true;};
	/// ��������� ����� ���������. ������������ �������� �� ������������(����)
	virtual bool ResumeWizard(){
		if (pages.empty())
			return false;
		if (m_cur_page == NULL)
			m_cur_page = pages.front();
		return true;
	};

	/// ���������� ������ �������� � ������ �������
	int get_page_index(MWBasePage* pp) const{
		MWPagesList_t::const_iterator res = std::find(pages.begin(), pages.end(), pp);
		if (res == pages.end())
		{
			ASSERT(0);
			return -1;
		}
		return std::distance(pages.begin(), res);
	}
	void set_caption(const CString& strCap){
		strCaption = strCap;
	}
	const CString& caption()const{
		return strCaption;
	}
	/// ���������� ������ ��������
	const MWPagesList_t& pages_list(){
		return pages;
	}
	/// ��������� � ������ ������� ��������
	bool add_page(MWBasePage* pp){
		pages.push_back(pp);
		return true;
	}
	/// ��������� ���� ������, ��������������� �������
	void set_item(HTREEITEM hItem){
		this->hItem = hItem;
	}
	/// ��������� ���� ������, ��������������� �������
	HTREEITEM item() const{
		return hItem;
	}
	MWBasePage* cur_page() const{
		return m_cur_page;
	}
	void set_cur_page(MWBasePage* pp){
		m_cur_page = pp;
	}
	/// ���� �� ������� ��������� � �������� ������ � ��������
	bool was_change() const{
		return m_bChanges;
	}
	/// ����������/���������� ������� ���������
	void set_changes(bool bc){
		m_bChanges = bc;
	}
};

typedef std::auto_ptr<MWWizard> MWWizardPtr;
#endif //__MW_WIZARD_HEADER__