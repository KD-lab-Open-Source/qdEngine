#ifndef __LIST_CTRL_INSERT_ITERATOR__

#define __LIST_CTRL_INSERT_ITERATOR__
namespace listCtrlInserter{

/**
	»спользуетс€ совместно с listCrl_insert_iterator. ќпредел€ет как тип 
	будет вставл€ть себ€ в CListCtrl
 */
template<class _InsertData>
struct data_traits {
	static int insert(CListCtrl* ctrl,int pos, const _InsertData& data);
};
/************************************************************************/
/* пример дл€ обычной строки                                            */
/************************************************************************/

/*
template<>
struct data_traits<LPCTSTR>{
	static int insert(CListCtrl* ctrl, int pos, const LPCTSTR& data){
		return ctrl->InsertItem(pos, data);
	}
};
*/

/**
	ѕредназначен дл€ вставки вс€кого в CListCtrl. 
	ƒл€ того чтобы это работало надо определить дл€ своего типа специализацию
	data_traits
 */
class listCtrl_insert_iterator {
protected:
	CListCtrl* container;
	int iter;
public:
	typedef CListCtrl          container_type;
	typedef std::output_iterator_tag iterator_category;
	typedef void                value_type;
	typedef void                difference_type;
	typedef void                pointer;
	typedef void                reference;
	
	listCtrl_insert_iterator(CListCtrl& __x, int __i) 
		: container(&__x), iter(__i) {}
	template<class _InsertData>
	const listCtrl_insert_iterator&
		operator=(const _InsertData& __value) { 

		iter = data_traits<_InsertData>::insert(container, iter, __value);
		++iter;
		return *this;
	}
	listCtrl_insert_iterator& operator*() { return *this; }
	listCtrl_insert_iterator& operator++() { return *this; }
	listCtrl_insert_iterator& operator++(int) { return *this; }
};

inline 
	listCtrl_insert_iterator listCtrl_inserter(CListCtrl& __x, int __i)
	{
		typedef int __iter;
		return listCtrl_insert_iterator(__x, __iter(__i));
	}

}//namespace listCtrlInserter
namespace std{
inline output_iterator_tag
	iterator_category(const listCtrlInserter::listCtrl_insert_iterator&)
	{
		return output_iterator_tag();
	}
}  
#endif//__LIST_CTRL_INSERT_ITERATOR__