#pragma once

class qdGameObjectStateStatic;
class qdGameScene;

/// ��������� �������� � ����������, ������� ���������� �������� �� anim_lst 
/**
	������������ ��� ������� ��������, �������������� � ���������� ����������
 */
class StateOffsetApplyer
{
public:
	StateOffsetApplyer(void);
	~StateOffsetApplyer(void);
	/**
		��������� �� ���� ������������� �������� ����� � ����������, ������������
		�������� �� ������ ��������� ��������. query_state - �����������
	 */
	void apply(qdGameObjectStateStatic const* query_state, 
				Vect2i const& offset, 
				qdGameScene* pscene,
				std::vector<LPCTSTR> const& anim_lst);
};
