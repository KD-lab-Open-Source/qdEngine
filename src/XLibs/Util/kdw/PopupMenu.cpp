#include "StdAfx.h"
#include "kdw/PopupMenu.h"
#include "kdw/Widget.h"
#include "kdw/Win32/Window.h"
#include <windows.h>

namespace kdw{

PopupMenu::PopupMenu(int maxItems)
{
    idRangeStart_ = ID_RANGE_MAX - maxItems;
    idRangeEnd_   = ID_RANGE_MAX;
}


PopupMenuItem* PopupMenu::nextItem(PopupMenuItem* item) const
{
    if(!item->children().empty())
        return item->children().front();
    else{
        PopupMenuItem0::Children& items = item->parent()->children();
        PopupMenuItem0::Children::iterator it = std::find(items.begin(), items.end(), item);
        xassert(it != items.end());
        while(item->parent() && *it == item->parent()->children().back()){
            item = item->parent();
			if(item->parent())
				it = std::find(item->parent()->children().begin(), item->parent()->children().end(), item);
        }
        if(item->parent()){
            ++it;
            item = *it;
            return item;
        }
        else
            return 0;
    }
}

void PopupMenu::assignIDs()
{
    DWORD currentID = idRangeStart_;
    PopupMenuItem* current = &root_;
    while(current){
        xassert(currentID < idRangeEnd_);
        if(current->children().empty())
            current->id_ = currentID++;
        else
            current->id_ = 0;

        current = nextItem(current);
    }
}

void PopupMenu::clear()
{
	root_.children().clear();
}

void PopupMenu::spawn(Widget* widget)
{
    POINT pt;
	GetCursorPos(&pt);
	spawn(Vect2i(pt.x, pt.y), widget);
}

void PopupMenu::spawn(Vect2i point, Widget* widget)
{
	if(root_.children().empty())
		return;

    assignIDs();
	root_.setMenuHandle(::CreatePopupMenu());

    PopupMenuItem* current = &root_;
    while(current = nextItem(current)){
        if(current->children().empty()){
			UINT_PTR id = current->menuID();
			std::string text = current->text();
			if(text == "-")
				AppendMenu(current->parent()->menuHandle(), MF_SEPARATOR, id, "");
			else{
				if(current->hotkey() != sKey()){
					text += "\t";
					text += current->hotkey().toString(true);
				}
				AppendMenu(current->parent()->menuHandle(), 
						   MF_STRING | (current->isChecked() ? MF_CHECKED : 0)
						   | (current->isEnabled() ? 0 : MF_GRAYED)
						   | (current->isDefault() ? MF_DEFAULT : 0),
						   id, text.c_str());			
			}
        }
		else{
			HMENU handle = ::CreatePopupMenu();
			current->setMenuHandle(handle);
			BOOL result = ::InsertMenu(current->parent()->menuHandle(), -1, MF_BYPOSITION | MF_POPUP, UINT_PTR(handle), current->text());
			xassert(result == TRUE);
		}
    }

	current = &root_;
    while(current = nextItem(current)){
        if(!current->children().empty()){
			UINT_PTR handle = UINT_PTR(current->menuHandle());
		}
    }

	HWND window = widget ? *_findWindow(widget) : 0;
	UINT id = ::TrackPopupMenu(root_.menuHandle(), TPM_LEFTBUTTON | TPM_LEFTALIGN | TPM_RETURNCMD, point.x, point.y, 0, window, 0);

	if(root_.children().empty())
		return;
	
    current = &root_;
    while(current = nextItem(current)){
        if(current->children().empty()){
			UINT current_id = current->menuID();
			if(current_id == id){
                current->call();
			}
        }
    }
}

// ---------------------------------------------------------------------------

PopupMenuItem::~PopupMenuItem()
{
	if(id_ && !children_.empty()){
		::DestroyMenu(menuHandle());
	}
}

PopupMenuItem0& PopupMenuItem::add(const char* text)
{
	PopupMenuItem0* item = new PopupMenuItem0(text);
	addChildren(item);
	return *item;
}


PopupMenuItem& PopupMenuItem::setHotkey(sKey key)
{
    hotkey_ = key;
	return *this;
}

PopupMenuItem& PopupMenuItem::addSeparator()
{
	return add("-");
}

PopupMenuItem* PopupMenuItem::find(const char* text)
{
    Children::iterator it;
    FOR_EACH(children_, it){
        PopupMenuItem* item = *it;
        if(strcmp(item->text(), text) == 0)
            return item;
    };
    return 0;
}

}
