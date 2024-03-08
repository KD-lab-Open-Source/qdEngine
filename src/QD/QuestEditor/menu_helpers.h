#pragma once

namespace menu_h{
	/// ������ ����������� ��������� ���� ����
inline void disableMenuItem(CMenu* pm, UINT id){
	pm->EnableMenuItem(id, MF_BYCOMMAND | MF_DISABLED| MF_GRAYED);
}
/// ������ ��������� ��������� ����� ����
inline void enableMenuItem(CMenu* pm, UINT id){
	pm->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
}

/// ������/������� �������� � ���������� ������ ����
inline void checkMenuItem(CMenu* pm, UINT id, bool bCheck){
	if (bCheck)
		pm->CheckMenuItem(id, MF_BYCOMMAND|MF_CHECKED);
	else
		pm->CheckMenuItem(id, MF_BYCOMMAND|MF_UNCHECKED);
}

/// ������ ������������ ��� ������ ����
inline void disableAllMenuItems(CMenu* pm)
{
	int count = pm->GetMenuItemCount();
	for(int i = 0; i < count; ++i)
		pm->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
}

}//end of menu_h