#pragma once
#include "multiwizard/mwwizard.h"
#include <memory>

class MWAddAnimFirst;
class MWConvertWayPage;
class MWAAProgressPage;
class MWSelectExstFilePage;
class MWAADone;
class MWAACreateObjPage;
class MWAACadrDuration;

class add_anim_wiz_data;

typedef std::auto_ptr<MWAddAnimFirst> MWAddAnimFirstPtr;
typedef std::auto_ptr<MWConvertWayPage> MWConvertWayPagePtr;
typedef std::auto_ptr<MWAAProgressPage> MWAAProgressPagePtr;
typedef std::auto_ptr<MWSelectExstFilePage> MWSelectExstFilePagePtr;
typedef std::auto_ptr<MWAACreateObjPage> MWAACreateObjPagePtr;
typedef std::auto_ptr<MWAACadrDuration> MWAACadrDurationPtr;
typedef std::auto_ptr<MWAADone> MWAADonePtr;
typedef std::auto_ptr<add_anim_wiz_data> add_anim_wiz_dataPtr;

/// Мульти-визард для сборки анимации
class AddAnimMWWizard :public MWWizard
{
public:
	AddAnimMWWizard(void);
	~AddAnimMWWizard(void);
	//останавливает визард, возвращает истину, 
	//если это удалось сделать
	bool SuspendWizard();
	//запускает по новой. возвращаемое значение не используется(пока)
	bool ResumeWizard();

	/// Конструирует странички и связывает их в цепочку
	bool Construct(qdGameDispatcher* ptrDisp, qdGameDispatcherBase* ptrGDB);

	add_anim_wiz_data* data() const{
		return m_ptrData.get();
	}
	using MWWizard::set_changes;
private:
	/// Зачищает данные
	void reset_data();
private://members
	/// Первая страница
	MWAddAnimFirstPtr m_ptrAAFirst;
	/// \name Набор страниц визарда
	//@{
	// АА - add animation
	/// Страничка, где решают собирать анимацию или добавлять существующую
	MWSelectExstFilePagePtr m_ptrAASelExistFiles;
	/// Страничка, на которой определяют способ конвератации
	MWConvertWayPagePtr m_ptrAAConvertWay;
	/// Страничка отражающая прогресс процесса сборки
	MWAAProgressPagePtr m_ptrAAProgress;
	/// Станичка, на которой решают создавать ли новые объекты
	MWAACreateObjPagePtr m_ptrAACrtObj;
	/// Страничка, на которой выбирают длительность кадра
	MWAACadrDurationPtr m_ptrAACadrDur;
	/// Последняя страничка
	MWAADonePtr	m_ptrAADone;
	//@}
	/// Данные для визарда
	add_anim_wiz_dataPtr m_ptrData;
};

typedef std::auto_ptr<AddAnimMWWizard> AddAnimMWWizardPtr;