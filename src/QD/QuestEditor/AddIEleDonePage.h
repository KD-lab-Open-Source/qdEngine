#pragma once


// AddIEleDonePage dialog
#include "wizardbasepage.h"

/**
	Последняя страница в визардах добавления элеметов интерфейса.
	При активизации этой страницы происходит создание объектов.
 */
class AddIEleDonePage : public WizardBasePage
{
	DECLARE_DYNAMIC(AddIEleDonePage)

public:
	/// Интерфейс фабрики, создающей объект
	class obj_fabric{
	public:
		/// Метод создающий объект
		virtual bool create_obj() = 0;
	};
	AddIEleDonePage();
	virtual ~AddIEleDonePage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_IELE_ADD_DONE };

	/// Назначает фабрику
	void set_obj_fabric(obj_fabric* pof);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual LRESULT OnWizardBack();
	virtual BOOL OnWizardFinish();
	/// Пытается создать объект. Сообщает о результатах пользователю
	virtual BOOL OnSetActive();
private:
	/// Фабрика, создающая объект
	obj_fabric* m_ptrObjFabric;
	/// Информационное сообщение пользователю
	CString m_strMessage;
};
