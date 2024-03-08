/********************************************************************
	created:	2003/04/26
	created:	26:4:2003   15:53
	filename: 	d:\Projects\Quest\QuestEditor\TriggerJoiner.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TriggerJoiner
	file ext:	h
	Powerd by:	�����
	
	purpose:	��������� ������ �� ����������� ���������.
*********************************************************************/

#pragma once

#include <hash_map>

class qdTriggerChain;
class qdTriggerElement;
class qdTriggerLink;

/**
	���������� ��� ��������
 */
class TriggerJoiner
{
	/// ��� ��� ������� ���������
	struct hashik {
		/// ���������� ���������, �������� � �����-��������
		size_t operator() (qdTriggerElementPtr p) const{
			return reinterpret_cast<size_t>(boost::get_pointer(p));
		}
	};
	typedef std::hash_map<qdTriggerElementPtr, qdTriggerElementPtr, hashik> EleTable;
public:
	TriggerJoiner();
	~TriggerJoiner();

	/// ���������� ������� ���������
	bool join(qdTriggerChain const* from, 
			qdTriggerChain* to, qdTriggerElementList& lst_ele);

private:
	class ele_copier;
	friend class ele_copier;
	/// �������� �������� � ������� �������
	class ele_copier
	{
	public:
		ele_copier(TriggerJoiner::EleTable& ele_table, qdTriggerChain* ptrTo);
		/// ��������� ������� � ������ ������� � �������� ��� ��������������
		void operator() (qdTriggerElementPtr pele);
	private:
		/// �������� �������������� � ��������� ������ �� �������
		void layout_copy(qdTriggerElementConstPtr from, qdTriggerElementPtr to) const;
	private:
		/// ������ ���������
		TriggerJoiner::EleTable& m_EleTable;
		/// ������� �������
		qdTriggerChain* m_ptrTo;
	};

	class links_copier;
	friend class links_copier;
	/// �������� ����� ������������� ���������
	class links_copier
	{
		typedef EleTable::value_type value_type;
		friend class link_copier;
		/// �������� ����
		class link_copier
		{
		public:
			link_copier(EleTable& ele_tbl, qdTriggerChain* pto, qdTriggerElementPtr from);
			/// ��������� ���� ����� ���������� � ������� �������.
			void operator()(qdTriggerLink const& lnk) const;
		private:
			/// ������ �������������� ������ ����� �� ��������� (from)
			void layout_copy(qdTriggerLink const& from, 
										qdTriggerElementConstPtr to) const;
		private:
			EleTable& m_EleTable;
			qdTriggerChain* m_ptrTo;
			qdTriggerElementPtr m_ptrFrom;
		};
	public:
		links_copier(TriggerJoiner::EleTable& ele_table, qdTriggerChain* pto);
		/// ����������� �� ���� ������ �������� � �������� ��
		void operator()(value_type& vt);
	private:
		/// ������� ����������� ���������
		TriggerJoiner::EleTable& m_EleTable;
		/// �������, � ������� ����� ����� �����������
		qdTriggerChain* m_ptrTo;
	};
	friend struct select_2nd;
	/// ���������� second ��� EleTable::value_type
	struct select_2nd {
		typedef EleTable::value_type argument_type;
		typedef qdTriggerElementPtr const result_type;
		qdTriggerElementPtr const& operator()(EleTable::value_type const&vt) const{
			return vt.second;
		}
	};

private:

	/// ������ ������������ ��������� ��������� �������� ��������� ������.
	/**
		���� - �������� �������, �������� - ����� �������
		������������ ��� �������� ���������� ��������� �������� ��� ����� 
		� ����� �������
	 */
	EleTable m_EleTable;
};
