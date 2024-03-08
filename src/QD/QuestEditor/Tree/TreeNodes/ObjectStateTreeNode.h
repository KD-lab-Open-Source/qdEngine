#pragma once
#include ".\conditionalobjecttreenode.h"

class qdGameObjectState;
/// Узел состояния анимированного объекта
class ObjectStateTreeNode : public ConditionalObjectTreeNode
{
protected:
public:
	ObjectStateTreeNode(qdNamedObject* object);
	~ObjectStateTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! Возвращает идентификатор ресурса меню для узла
	DWORD getMenuResId() const;
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	bool showProperties(TreeLogic& logic, int iActive);
	bool showConditions(TreeLogic& logic);

	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);

	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief Вызывается, когда нужно удалить объекты, содержащиеся в другом,
	//! при его удаление(сотояния, когда удаляется анимированный объект. 
	//! Все объекты, когда удаляется сцена)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	//! Можно ли кинуть узел на тот, над которым тянем
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
						ITreeNode* dropTarget, 
						DROPEFFECT dropEffect);
	//! Возвращает true, если удалось переимновать, false в противном случае
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
	//! Можно ли сменить имя узлу
	virtual bool canRename(TreeLogic& logic);

	void* operator new(size_t size);
	void operator delete(void* p);

	void* operator new(size_t size, LPCTSTR file, unsigned line);
	void operator delete(void* p, LPCTSTR file, unsigned line);

	//! Найти и установить новую дефолтную иконку, учитывая все соседние состояния
	void resetDefaultStateIcon(TreeLogic& logic);
protected:
	/// Выбрали(сняли выбор) при активном редакторе сцены
	bool onSelectInSceneView(TreeLogic& logic, bool bSelect);
	/// Выбрали(сняли выбор) при активном редакторе триггеров
	bool onSelectInTriggerView(TreeLogic& logic, bool bSelect);
	/// Определяет картинку для узла состояния
	static int selectPicture(qdGameObjectState const& state, qdNamedObject const& owner);
	/// Определяет картинку для узла состояния мыши
	static int selectMouseStatePicture(qdGameObjectMouse const& mouse, 
										qdNamedObject const& state);

	//! Используется, если владелец состояния мышь
	bool deleteMouseState(TreeLogic& logic);
	//! Используется, если владелец состояния анимированный объект
	bool deleteAnimatedObjectState(TreeLogic& logic, bool ask);
	virtual bool preDelete(TreeLogic& logic, bool ask, bool statDepends);
	/// Делает действия после удаления немышового состояния 
	virtual bool postDeleteNotMouseState(TreeLogic& logic);

	/// Можем ли бросить глобальное состоняие
	DROPEFFECT cdGlobalState(TreeLogic& logic, ITreeNode* overNode);
	/// Можем ли бросить неглобальное состоняие
	DROPEFFECT cdOwnState(TreeLogic& logic, ITreeNode* overNode);

	/// Можем ли бросить состояние под анимированный объект
	bool canDropUnderAnim(TreeLogic& logic, ITreeNode* node);
	/// Можем ли бросить неглобальное состояние под персонаж
	DROPEFFECT cdOwnStateUnderMoving(TreeLogic& logic,
//	DROPEFFECT ObjectStateTreeNode::cdOwnStateUnderMoving(TreeLogic& logic,
															ITreeNode* overNode,
															ITreeNode* animatedNode);
	/// Можем ли бросить состояние под персонаж
	bool canDropUnderMoving(TreeLogic& logic, ITreeNode* movingNode);

	/// Бросили неглобальное состояние под мышь
	DROPEFFECT oddOwnStateMouseObj(TreeLogic& logic, 
							ITreeNode* animated,
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	/// Бросили неглобальное состояние под анимированный объект
	DROPEFFECT oddOwnStateAnimObj(TreeLogic& logic, 
							ITreeNode* animated,
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	/// Бросили неглобальное состояние под персонаж
	DROPEFFECT oddOwnStateMovingObj(TreeLogic& logic, 
									ITreeNode* animated,
									ITreeNode* dropTarget, 
									DROPEFFECT dropEffect);
	/// Возвращает true, если newName уникально
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
