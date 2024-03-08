#pragma once

#include "qd_fwd.h"
#include <boost/rtl.hpp>

interface ITreeNodeFactory;
interface ITreeNode;

class CTreeListItem;
class TreeLogic;
class qdInterfaceObjectBase;
/**
	@namespace load_tree_assist
	�������� ��������, ���������� ��������� �������.
 */
namespace load_tree_assist
{
	/// ��������� ������ � ������
	class InsertObject{
	public:
		/// �������������� ����������-�����
		InsertObject(ITreeNodeFactory* factory, 
					TreeLogic& treeLogic, 
					ITreeNode * parent,
					qdNamedObjectBase const* owner,
					ITreeNode* insertAfter);
			/// ��������� ������ � ������. ��������� ������� parent_
		  bool operator() (qdNamedObject* object);
		  /// ��������� ������ � ������. �������� ����� ���� ������ ��� ������
		  bool operator()(qdNamedObject* object, ITreeNode * parent);
	private:
		/// ������� ��������
		ITreeNodeFactory* factory_;
		/// ������ ������ ������
		TreeLogic& treeLogic_;
		/// ������������ ����
		ITreeNode * parent_;
		/// ��������
		qdNamedObjectBase const* owner_;
		/// ����, �� ������� ����� �������� �����
		ITreeNode* insertAfter_;
	};

	class InsertInterfaceObject{
	public:
		/// �������������� ����������-�����
		InsertInterfaceObject(ITreeNodeFactory* factory, 
			TreeLogic& treeLogic, 
			ITreeNode * parent,
			qdNamedObjectBase const* owner,
			ITreeNode* insertAfter);
		/// ��������� ������������ ������ � ������
		bool operator() (qdInterfaceObjectBase* object);
	private:
		/// ������� ��������
		ITreeNodeFactory* factory_;
		/// ������ ������ ������
		TreeLogic& treeLogic_;
		/// ������������ ����
		ITreeNode * parent_;
		/// ��������
		qdNamedObjectBase const* owner_;
		/// ����, �� ������� ����� �������� �����
		ITreeNode* insertAfter_;
	};

	/// ��������� ������� ����� � ������
	class InsertGameObject
	{
	public:
		InsertGameObject(ITreeNodeFactory* factory, TreeLogic& treeLogic,
			ITreeNode * staticObjs, 
			ITreeNode * animatedObjs, 
			ITreeNode * movingObjs,
			qdNamedObjectBase const* owner,
			ITreeNode* insertAfter);
		/// @brief ��������� ������ � ������. 
		/// � ����������� �� ���� ������ � ��������������� �������
		void operator()(qdGameObject* object);
	private:
		/// �������, ������������ ������� �������� � ������
		InsertObject objInserter_;
		/// ����-������� ��� ����������� ��������
		ITreeNode * staticObjs_;
		/// ����-������� ��� ������������� ��������
		ITreeNode * animatedObjs_;
		/// ����-������� ��� ����������
		ITreeNode * movingObjs_;
	};


	/// ��������� �������� ��������.
	/**
		������� ����-�������. � ���� ���������� ������� �� ������ lst
	 */
	template<class List>
	bool loadSimpleCatalog(DWORD id, 
						ITreeNodeFactory* factory, 
						TreeLogic& treeLogic,
						ITreeNode * parent, 
						qdNamedObjectBase const* owner,
						ITreeNode* insertAfter, List const& lst)
	{
		std::auto_ptr<ITreeNode> p = factory->createFolderNode(id);
		if (!p.get()||!p->load(factory, treeLogic, parent, owner, insertAfter))
			return false;
		ITreeNode* folder = p.release();
		boost::rtl::rng::for_each(lst, 
			InsertObject(factory, treeLogic, folder, owner, TN_INSERT_SORT));
		return true;
	}

	/// ������������� ������� ��������. 
	/**
		����� �����������. ��� ���, ��� ��� ���� � ������ ���������� ������������
	 */
	template<typename Range>
	bool reloadSimpleCatalog(ITreeNodeFactory* factory, 
									TreeLogic& logic,
									ITreeNode* folder,
									qdNamedObjectBase* owner,
									ITreeNode* insertAfter, 
									Range r)
	{
		InsertObject inserter(factory, logic, folder, owner, insertAfter);
		for(; r; ++r)
		{
			typedef boost::rtl::irange<TreeLogic::NodeIterator> Nodes;
			Nodes ir(logic.getObjectNodes(*r));
			if (ir)
				(*ir)->reload(factory, logic, folder, owner);
			else
				inserter(*r);
		}

		return true;
	}

	//! �������, ������������ �������� �� ��������� ������������ ����������
	class own_walk{
		qdGameObjectAnimated const* const powner;
	public:
		own_walk(const qdGameObjectAnimated* po):powner(po){}
		bool operator()(qdGameObjectState const* ps) const;
	};

	//! �������, ������������ �������� �� ��������� ������������ ������������
	class own_not_walk{
		qdGameObjectAnimated const* const powner;
	public:
		own_not_walk(qdGameObjectAnimated const* po):powner(po){}

		bool operator()(qdGameObjectState const* ps) const;
	};

};
