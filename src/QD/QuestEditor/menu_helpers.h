#pragma once

namespace menu_h{
	/// ƒелает недоступным указанный пунк меню
inline void disableMenuItem(CMenu* pm, UINT id){
	pm->EnableMenuItem(id, MF_BYCOMMAND | MF_DISABLED| MF_GRAYED);
}
/// ƒелает доступным указанный пункт меню
inline void enableMenuItem(CMenu* pm, UINT id){
	pm->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
}

/// —тавит/снимает поментку с указанного пункта меню
inline void checkMenuItem(CMenu* pm, UINT id, bool bCheck){
	if (bCheck)
		pm->CheckMenuItem(id, MF_BYCOMMAND|MF_CHECKED);
	else
		pm->CheckMenuItem(id, MF_BYCOMMAND|MF_UNCHECKED);
}

/// делает недостпуными все пункты меню
inline void disableAllMenuItems(CMenu* pm)
{
	int count = pm->GetMenuItemCount();
	for(int i = 0; i < count; ++i)
		pm->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
}

}//end of menu_h