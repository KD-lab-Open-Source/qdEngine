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
	Содержит функторы, помогающие загружать объекты.
 */
namespace load_tree_assist
{
	/// Вставляет объект в дерево
	class InsertObject{
	public:
		/// инициализирует переменные-члены
		InsertObject(ITreeNodeFactory* factory, 
					TreeLogic& treeLogic, 
					ITreeNode * parent,
					qdNamedObjectBase const* owner,
					ITreeNode* insertAfter);
			/// Вставляет объект в дерево. Родителем берется parent_
		  bool operator() (qdNamedObject* object);
		  /// Вставляет объект в дерево. Родитель может быть указан при вызове
		  bool operator()(qdNamedObject* object, ITreeNode * parent);
	private:
		/// Фабрика объектов
		ITreeNodeFactory* factory_;
		/// Логика работы дерева
		TreeLogic& treeLogic_;
		/// Родительский узле
		ITreeNode * parent_;
		/// Владелец
		qdNamedObjectBase const* owner_;
		/// Узел, за которым будет вставлен новый
		ITreeNode* insertAfter_;
	};

	class InsertInterfaceObject{
	public:
		/// инициализирует переменные-члены
		InsertInterfaceObject(ITreeNodeFactory* factory, 
			TreeLogic& treeLogic, 
			ITreeNode * parent,
			qdNamedObjectBase const* owner,
			ITreeNode* insertAfter);
		/// Вставляет интрефейсный объект в дерево
		bool operator() (qdInterfaceObjectBase* object);
	private:
		/// Фабрика объектов
		ITreeNodeFactory* factory_;
		/// Логика работы дерева
		TreeLogic& treeLogic_;
		/// Родительский узел
		ITreeNode * parent_;
		/// Владелец
		qdNamedObjectBase const* owner_;
		/// Узел, за которым будет вставлен новый
		ITreeNode* insertAfter_;
	};

	/// Вставляет объекты сцены в дерево
	class InsertGameObject
	{
	public:
		InsertGameObject(ITreeNodeFactory* factory, TreeLogic& treeLogic,
			ITreeNode * staticObjs, 
			ITreeNode * animatedObjs, 
			ITreeNode * movingObjs,
			qdNamedObjectBase const* owner,
			ITreeNode* insertAfter);
		/// @brief Вставляет объект в дерево. 
		/// В зависимости от типа кладет в соответствующий каталог
		void operator()(qdGameObject* object);
	private:
		/// Функтор, производящий вставку объектов в дерево
		InsertObject objInserter_;
		/// Узел-каталог для статических объектов
		ITreeNode * staticObjs_;
		/// Узел-каталог для анимированных объектов
		ITreeNode * animatedObjs_;
		/// Узел-каталог для персонажей
		ITreeNode * movingObjs_;
	};


	/// Загружает каталого объектов.
	/**
		Создает узел-каталог. В него вствляются объекты из списка lst
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

	/// Перезагружает каталог объектов. 
	/**
		Новые добавляются. Для тех, что уже есть в дереве вызывается перезагрузка
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

	//! функтор, определяющий является ли состояние неглобальным походочным
	class own_walk{
		qdGameObjectAnimated const* const powner;
	public:
		own_walk(const qdGameObjectAnimated* po):powner(po){}
		bool operator()(qdGameObjectState const* ps) const;
	};

	//! функтор, определяющий является ли состояние неглобальным непоходочным
	class own_not_walk{
		qdGameObjectAnimated const* const powner;
	public:
		own_not_walk(qdGameObjectAnimated const* po):powner(po){}

		bool operator()(qdGameObjectState const* ps) const;
	};

};
