#pragma once

#include "IObjectList.h"
#include <boost/scoped_ptr.hpp>

class CCoolDialogBar;
class ObjectListDlg;
class ObjectListImpl;

/// Создает компоненты, необходимые чтобы отобразить список в плавающем окне
class ObjectListMngr : public IObjectList
{
public:
	ObjectListMngr(void);
	~ObjectListMngr(void);

	/// Создает все окна.
	bool create(CFrameWnd* pParent, DWORD dwStyle = WS_CHILD |CBRS_FLYBY | 
								CBRS_RIGHT | CBRS_SIZE_DYNAMIC);

	/// Видим ли
	bool isVisible() const;
	virtual void show();
	virtual void hide();
	virtual bool showObjectInList(qdNamedObjectReference const* );
	virtual HITEM addObjectToList(qdNamedObjectReference const*, HITEM hItemRoot);
	virtual bool removeObjectFromList(qdNamedObjectReference const* );
	virtual bool addSubObjectFromList(HITEM hObjectItem, qdNamedObjectReference const* );
	virtual void clearList();
	virtual void setListCaption(CString const& str);
private:
	/// Окно приложения
	CFrameWnd* parentFrame_;
	/// Объект, реализующий прилипающее окно
	boost::scoped_ptr<CCoolDialogBar> dlgBar_;
	/// Диалог, содержащий необходимые контролы для отображения списка
	boost::scoped_ptr<ObjectListDlg> dlg_;
	/// Класс реализующий логику работы со списоком
	boost::scoped_ptr<ObjectListImpl> objectListImpl_;
};
