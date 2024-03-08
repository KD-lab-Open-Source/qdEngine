/********************************************************************
	created:	2002/12/09
	created:	9:12:2002   17:29
	filename: 	d:\projects\quest\questeditor\mwwizard.h
	file path:	d:\projects\quest\questeditor
	file base:	mwwizard
	file ext:	h
	Powered by:	»люха
	
	purpose:	Ѕазовый класс дл€ визарда, участвующего в мульти визарде
*********************************************************************/

#ifndef __MW_WIZARD_HEADER__
#define __MW_WIZARD_HEADER__

#include <algorithm>

class MWBasePage;

typedef std::list<MWBasePage*> MWPagesList_t;
/**
	Ѕазовый класс дл€ визарда, участвующего в мульти визарде
 */
class MWWizard {
	/// «аголовок. Ѕудет отображен в дереве визардов
	CString strCaption;
	/// —писок страниц принадлежащих визраду
	MWPagesList_t pages;
	/// “екуща€ страница
	MWBasePage* m_cur_page;
	/// узел в дереве
	HTREEITEM hItem;
	/// были ли сделаны изменеи€ в процессе работы
	bool m_bChanges;
public:
	MWWizard():m_cur_page(NULL), hItem(NULL),m_bChanges(false){
	}
	virtual ~MWWizard()
	{}

	/// останавливает визард
	/**
		“екуща€ страница уже должна быть правильно выставлена
		\return true если это удалось остановить
	 */
	virtual bool SuspendWizard(){return true;};
	/// запускает после остановку. возвращаемое значение не используетс€(пока)
	virtual bool ResumeWizard(){
		if (pages.empty())
			return false;
		if (m_cur_page == NULL)
			m_cur_page = pages.front();
		return true;
	};

	/// ќпредел€ет индекс страницы в списке старниц
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
	/// ¬озвращает список стараниц
	const MWPagesList_t& pages_list(){
		return pages;
	}
	/// ƒобавл€ет к списку страниц страницу
	bool add_page(MWBasePage* pp){
		pages.push_back(pp);
		return true;
	}
	/// Ќазначает узел дерева, соответствующий визарду
	void set_item(HTREEITEM hItem){
		this->hItem = hItem;
	}
	/// ¬озвращет узел дерева, соответствующий визарду
	HTREEITEM item() const{
		return hItem;
	}
	MWBasePage* cur_page() const{
		return m_cur_page;
	}
	void set_cur_page(MWBasePage* pp){
		m_cur_page = pp;
	}
	/// были ли сделаны изменени€ в процессе работы с визардом
	bool was_change() const{
		return m_bChanges;
	}
	/// ¬ыставл€ет/сбрасывает признак изменений
	void set_changes(bool bc){
		m_bChanges = bc;
	}
};

typedef std::auto_ptr<MWWizard> MWWizardPtr;
#endif //__MW_WIZARD_HEADER__