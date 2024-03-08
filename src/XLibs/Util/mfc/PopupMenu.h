#ifndef __POPUP_MENU_H_INCLUDED__
#define __POPUP_MENU_H_INCLUDED__

#include <list>
#include "XTL\Functor.h"

class PopupMenu;
class PopupMenuItem : public ShareHandleBase{
public:
	friend PopupMenu;
    typedef std::list<ShareHandle<PopupMenuItem> > Children;

    PopupMenuItem(const char* text = "")
    : id_(0)
	, parent_(0)
	, text_(text)
	, checked_(false)
	, enabled_(true)
	, default_(false)
    {}

    PopupMenuItem(const char* text, Functor0<void> callback)
    : callback_(callback)
	, id_(0)
	, parent_(0)
	, text_(text)
	, checked_(false)
	, enabled_(true)
	, default_(false)
    {}

	~PopupMenuItem(){
		if(id_ && !children_.empty()){
			::DestroyMenu(menuHandle());
		}
	}
	PopupMenuItem& check(bool checked = true){ checked_ = checked; return *this; }
	bool isChecked() const{ return checked_; }

	PopupMenuItem& enable(bool enabled = true){ enabled_ = enabled; return *this; }
	bool isEnabled() const{ return enabled_; }

	void setDefault(bool defaultItem = true){ default_ = defaultItem; }
	bool isDefault() const{ return default_; }

    const char* text() { return text_.c_str(); }
	PopupMenuItem& addSeparator()
	{
		return add("-");
	}

    PopupMenuItem& add(const char* text){
		return add(PopupMenuItem(text));
	}
	/*
    PopupMenuItem& add(const char* text, Functor func){
		return add(PopupMenuItem(text, func));
	}
	*/
	PopupMenuItem& connect(Functor0<void> func){
		callback_ = func;
		return *this;
	}
	PopupMenuItem* find(const char* text);

	Functor0<void> callback() { return callback_; }
	PopupMenuItem* parent() { return parent_; }
	Children& children() { return children_; };
    const Children& children() const { return children_; }
	bool empty() const { return children_.empty(); }
private:
    PopupMenuItem& add(PopupMenuItem& item){
        children_.push_back(new PopupMenuItem(item));
        children_.back()->parent_ = this;
		return *children_.back();
    }

    HMENU menuHandle(){
        xassert(!children_.empty());
        return HMENU(id_);
    }
	void setMenuHandle(HMENU menu){
        xassert(!children_.empty());
		id_ = DWORD(menu);
	}
    UINT menuID(){
        xassert(children_.empty());
        return UINT(id_);
    }
	void setMenuID(UINT id){
        xassert(children_.empty());
		id_ = DWORD(id);
	}

    DWORD id_;
    Functor0<void> callback_;

	bool default_;
	bool checked_;
	bool enabled_;
    std::string text_;
    PopupMenuItem* parent_;
    Children children_;
};


class PopupMenu{
public:
    enum {
        ID_RANGE_MIN = 32768,
        ID_RANGE_MAX = 32767 + 16384
    };
    PopupMenu(int maxItems);
    PopupMenuItem& root() { return root_; };
    const PopupMenuItem& root() const { return root_; };
    void spawn(POINT pt, HWND window);
	void clear();

	BOOL onCommand(WPARAM wParam, LPARAM lParam);
private:
    PopupMenuItem* nextItem(PopupMenuItem* item) const;

    PopupMenuItem root_;

    void assignIDs();

    UINT idRangeStart_;
    UINT idRangeEnd_;
};

#endif
