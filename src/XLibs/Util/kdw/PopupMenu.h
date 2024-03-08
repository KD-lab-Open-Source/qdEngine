#ifndef __KDW_POPUP_MENU_H_INCLUDED__
#define __KDW_POPUP_MENU_H_INCLUDED__

#include "kdw/API.h"
#include <string>
#include <vector>
#include "Handle.h"
#include "XTL/sigslot.h"
#include "XMath/XMath.h"
#include "sKey.h"

struct HMENU__;
typedef HMENU__* HMENU;

namespace kdw{

class Widget;

class PopupMenu;
class PopupMenuItem0;
template<class Arg1> class PopupMenuItem1;
template<class Arg1, class Arg2> class PopupMenuItem2;

class KDW_API PopupMenuItem: public ShareHandleBase{
public:
	friend PopupMenu;
    typedef std::vector<ShareHandle<PopupMenuItem> > Children;

    PopupMenuItem(const char* text = "")
    : id_(0)
	, parent_(0)
	, text_(text)
	, checked_(false)
	, enabled_(true)
	, default_(false)
    {}

	virtual ~PopupMenuItem();

	PopupMenuItem& check(bool checked = true){ checked_ = checked; return *this; }
	bool isChecked() const{ return checked_; }

	PopupMenuItem& enable(bool enabled = true){ enabled_ = enabled; return *this; }
	bool isEnabled() const{ return enabled_; }

	PopupMenuItem& setHotkey(sKey key);
	sKey hotkey() const{ return hotkey_; }

	void setDefault(bool defaultItem = true){ default_ = defaultItem; }
	bool isDefault() const{ return default_; }

    const char* text() { return text_.c_str(); }
	PopupMenuItem& addSeparator();
    PopupMenuItem0& add(const char* text);

    template<class Arg1>
    PopupMenuItem1<Arg1>& add(const char* text, const Arg1& arg1);

    template<class Arg1, class Arg2>
    PopupMenuItem2<Arg1, Arg2>& add(const char* text, const Arg1& arg1, const Arg2& arg2);

	PopupMenuItem* find(const char* text);

	PopupMenuItem* parent() { return parent_; }
	Children& children() { return children_; };
    const Children& children() const { return children_; }
	bool empty() const { return children_.empty(); }
private:
    void addChildren(PopupMenuItem* item){
        children_.push_back(item);
        item->parent_ = this;
    }

    HMENU menuHandle(){
        xassert(!children_.empty());
        return HMENU(id_);
    }
	void setMenuHandle(HMENU menu){
        xassert(!children_.empty());
		id_ = unsigned int(menu);
	}
    unsigned int menuID(){
        xassert(children_.empty());
        return id_;
    }
	void setMenuID(unsigned int id){
        xassert(children_.empty());
		id_ = unsigned int(id);
	}

    virtual void call() = 0;

    unsigned int id_;

	bool default_;
	bool checked_;
	bool enabled_;
    std::string text_;
    PopupMenuItem* parent_;
    Children children_;
	sKey hotkey_;
};

class PopupMenuItem0 : public PopupMenuItem, public sigslot::signal0{
public:
    PopupMenuItem0(const char* text = "")
    : PopupMenuItem(text)
    {}

    void call(){ emit(); }
	template<class desttype>
	PopupMenuItem0& connect(desttype* pclass, void (desttype::*pmemfun)()){
		sigslot::signal0::connect(pclass, pmemfun);
		return *this;
	}
protected:
};


/*class PopupMenuItemDynamic : public PopupMenuItem, public sigslot::signal1<PopupMenuItem&>{
public:
    PopupMenuItem0(const char* text = "")
    : PopupMenuItem(text)
    {}

    void call(){ emit(); }
	template<class desttype>
	PopupMenuItem0& connect(desttype* pclass, void (desttype::*pmemfun)(PopupMenuItem&)){
		sigslot::signal0::connect(pclass, pmemfun);
		return *this;
	}
	void generate(){
		clear();
		emit(*this);
	}
protected:
};
*/


template<class Arg1>
class PopupMenuItem1 : public PopupMenuItem, public sigslot::signal1<Arg1>{
public:
    PopupMenuItem1(const char* text, Arg1 arg1)
    : PopupMenuItem(text)
    , arg1_(arg1)
    {}

    void call(){ emit(arg1_); }

	template<class desttype>
	PopupMenuItem1& connect(desttype* pclass, void (desttype::*pmemfun)(Arg1)){
		sigslot::signal1<Arg1>::connect(pclass, pmemfun);
		return *this;
	}
protected:
    Arg1 arg1_;
};

template<class Arg1, class Arg2>
class PopupMenuItem2 : public PopupMenuItem, public sigslot::signal2<Arg1, Arg2>{
public:
    PopupMenuItem2(const char* text, Arg1 arg1, Arg2 arg2)
    : PopupMenuItem(text)
    , arg1_(arg1)
    , arg2_(arg2)
    {}

    void call(){ emit(arg1_, arg2_); }

	template<class desttype>
	PopupMenuItem2& connect(desttype* pclass, void (desttype::*pmemfun)(Arg1, Arg2)){
		sigslot::signal2<Arg1, Arg2>::connect(pclass, pmemfun);
		return *this;
	}
protected:
    Arg1 arg1_;
    Arg2 arg2_;
};

template<class Arg1>
PopupMenuItem1<Arg1>& PopupMenuItem::add(const char* text, const Arg1& arg1){
	PopupMenuItem1<Arg1>* item = new PopupMenuItem1<Arg1>(text, arg1);
	addChildren(item);
	return *item;
}

template<class Arg1, class Arg2>
PopupMenuItem2<Arg1, Arg2>& PopupMenuItem::add(const char* text, const Arg1& arg1, const Arg2& arg2){
	PopupMenuItem2<Arg1, Arg2>* item = new PopupMenuItem2<Arg1, Arg2>(text, arg1, arg2);
	addChildren(item);
	return *item;
}



class KDW_API PopupMenu{
public:
    enum {
        ID_RANGE_MIN = 32768,
        ID_RANGE_MAX = 32767 + 16384
    };
    PopupMenu(int maxItems);
    PopupMenuItem0& root() { return root_; };
    const PopupMenuItem0& root() const { return root_; };
    void spawn(Widget* widget);
    void spawn(Vect2i screenPoint, Widget* widget);
	void clear();

	//BOOL onCommand(WPARAM wParam, LPARAM lParam);
private:
    PopupMenuItem* nextItem(PopupMenuItem* item) const;

    PopupMenuItem0 root_;

    void assignIDs();

    unsigned int idRangeStart_;
    unsigned int idRangeEnd_;
};

}

#endif
