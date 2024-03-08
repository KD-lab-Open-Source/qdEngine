#pragma once

class qdGameObjectState;
class qdGameObjectAnimated;

///  \brief ��������������� ��������� ����������� ��� �����������(��������) ��������� ��
/// ������ ������� � ������.
class RestoreImageLayout
{
public:
	RestoreImageLayout(void);
	~RestoreImageLayout(void);

	/*!
		��������������� ��������� ����������� ��� �����������(��������) ��������� ��
		������ ������� � ������.
		�������� �������� ���������� ��������� ������� � ������������� �������� ����������.
		�������� �������� ���������� ������� ������� � ������������� ����� ����������.
		��������� � �������� ������ ��������� ������� ����� ����������� ���� ��������.
		\remarks \a pnew �������� ������ \a psource
		\param psource	- �������� ���������
		\param po_src	- �������� ������
		\param pmo		- ������, � ������� ��������� ���������
		\param pnew		- ���������, ������� ���������
	*/
	static void restore(qdGameObjectState* psource, qdGameObjectAnimated * po_src,
		qdGameObjectAnimated* pmo, qdGameObjectState* pnew);
	/*!
 		�������� ��������� ��������� �����(�������� \a pnew) � �������� ����������
		RestoreImageLayout(qdGameObjectState* psource, qdGameObjectAnimated * po_src,
		qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
	*/

	static void restore(qdGameObjectState* psource, 
		qdGameObjectAnimated* pmo, qdGameObjectState* pnew);

	/*!
		����������� ��������� ����������� ��� ��������� ��� ����������� ������������� 
		������� � ��������
		\remarks \a pobj - ������ ���� ������ \a psrc
	*/
	static void restore(qdGameObjectAnimated * psrc, qdGameObjectMoving *pobj);
};
