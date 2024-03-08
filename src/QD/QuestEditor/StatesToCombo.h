#pragma once

#include <boost/function.hpp>
class qdGameScene;
class qdGameObjectState;
class qdGameObject;

/**
	����� ������� ����������� ��������� ���������� ������� �� ������� ��� �� �����.

 */
class StatesToCombo
{
public:
	/// ���������� ��� �������� ��������������
	typedef boost::function1<CString, qdGameObjectState const*> Formater;

	/**
		����� �������� �������������� �� ���������. � ���������� ����������
		������, ����������� ��� ��������� � ��� ��������� ���������
	 */
	class DefFormat
	{
		/// ������������� ��������� ������
		UINT_PTR m_uiId;
	public:
		DefFormat();
		DefFormat(UINT_PTR uiId);
		/// �������� ���� ��������
		CString operator()(qdGameObjectState const* pstate) const;
	};

	/**
		����� ��������, � ���������� ������ �������� ���������� ������, ���������� 
		��� ���������
	 */
	class FormatStateName
	{
	public:
		FormatStateName();
		/// �������� ���� ��������
		CString operator()(qdGameObjectState const* pstate) const;
	};
private:
	/**
		��������� ��� ��������� ������� � ���������, ��������� m_formater ��� 
		��������� � ��������� ������� ���� ������. 
	 */
	class ObjStates2Combo
	{
		Formater const& m_formater;
		CCJFlatComboBox& m_combo;
	public:
		ObjStates2Combo(CCJFlatComboBox& combo, Formater const& formater);
		/// �������� ��� ��������� ������� � ��������� �� � ���������
		void operator()(qdGameObject const* pobj);
	};

	/**
		����� ��������� ���������� ��� ��������� � ���������, ��������� m_formater ��� 
		��������� � ��������� ������� ���� ������.
	 */
	class StateToCombo
	{
		Formater const& m_formater;
		CCJFlatComboBox& m_combo;
	public:
		StateToCombo(CCJFlatComboBox& combo, Formater const& formater);
		void operator()(qdGameObjectState const* pstate);
	};
protected:
	~StatesToCombo(void);
public:
	StatesToCombo(void);
	/// ��������� ��������� ��� �����
	static bool loadFromScene(CCJFlatComboBox& combo, qdGameScene const* pscene, 
		Formater formater = DefFormat());
	/// ��������� ��������� ��� �������
	static bool loadFromObject(CCJFlatComboBox& combo, qdGameObjectAnimated const* pobj,
		Formater formater = FormatStateName());
};
