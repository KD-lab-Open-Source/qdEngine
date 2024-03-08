#pragma once

/********************************************************************
	created:	2003/05/13
	created:	13:5:2003   17:28
	filename: 	d:\Projects\Quest\QuestEditor\iscreen2combo.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	iscreen2combo
	file ext:	h
	Powerd by:	�����
	
	purpose:	������ ������� ��� ������������� ��������� � 
				function_output_iterator.hpp. ���� �������� ���������-
				��� ������ � ���������.
*********************************************************************/

///������� ��� ������������� ��������� � function_output_iterator
/**
	���� �������� ������������ ������ � ���������.
 */
class iscreens2combo{
	CCJFlatComboBox& box_;
public:
	iscreens2combo(CCJFlatComboBox& box):box_(box){}
	void operator()(qdInterfaceScreen* pscr) const{
		int const i = box_.AddString(pscr->name());
		if (i != CB_ERR)
			box_.SetItemDataPtr(i, pscr);
	}
};
