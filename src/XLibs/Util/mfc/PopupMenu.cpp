#include "StdAfx.h"
#include "PopupMenu.h"

PopupMenu::PopupMenu(int maxItems)
{
    idRangeStart_ = ID_RANGE_MAX - maxItems;
    idRangeEnd_   = ID_RANGE_MAX;
    //xassert(idRangeStart_ > _APS_NEXT_COMMAND_VALUE);
}


PopupMenuItem* PopupMenu::nextItem(PopupMenuItem* item) const
{
    if(!item->children().empty())
        return item->children().front();
    else{
        PopupMenuItem::Children& items = item->parent()->children();
        PopupMenuItem::Children::iterator it = std::find(items.begin(), items.end(), item);
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

void PopupMenu::spawn(POINT point, HWND window)
{
	if(root_.children().empty())
		return;

    assignIDs();
	root_.setMenuHandle(::CreatePopupMenu());

    PopupMenuItem* current = &root_;
    while(current = nextItem(current)){
        if(current->children().empty()){
			UINT_PTR id = current->menuID();
			const char* text = current->text();
			if(strcmp(text, "-") == 0)
				AppendMenu(current->parent()->menuHandle(), MF_SEPARATOR, id, "");
			else
				AppendMenu(current->parent()->menuHandle(), 
						   MF_STRING | (current->isChecked() ? MF_CHECKED : 0)
						   | (current->isEnabled() ? 0 : MF_GRAYED)
						   | (current->isDefault() ? MF_DEFAULT : 0),
						   id, current->text());			
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

	::TrackPopupMenu(root_.menuHandle(), TPM_LEFTBUTTON | TPM_LEFTALIGN, point.x, point.y, 0, window, 0);
}

BOOL PopupMenu::onCommand(WPARAM wParam, LPARAM lParam)
{
	UINT id = wParam;

	if(root_.children().empty())
		return FALSE;
	
    PopupMenuItem* current = &root_;
    while(current = nextItem(current)){
        if(current->children().empty()){
			UINT current_id = current->menuID();
			if(current_id == id){
				if(current->callback())
					current->callback()();
			}
        }
    }

	return TRUE;
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
