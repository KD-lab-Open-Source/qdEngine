#if !defined _TEXT_ID_SELECTOR_H

#define _TEXT_ID_SELECTOR_H

class qdTextDB;
/**
	�����-�������. ��������� ������� ������������� ��� ������ ������ �� ���� 
	������
 */
class TextIdSelector{
public:
	/// ��������� ��� ����������� ������
	static bool run(HWND hParent, qdTextDB const* ptrTextDB, CString& textID);
};
#endif//_TEXT_ID_SEwLECTOR_H