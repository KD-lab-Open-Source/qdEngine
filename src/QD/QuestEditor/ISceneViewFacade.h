
#pragma once

/*!
Уточнение для состояния редактора сцены. Т.е. что делаем в текущем состоянии
*/
enum STATE_SUBTOOLS_ENUM
{
	SSTE_NONE, ///< ничего
	SSTE_CREATE_ZONE,///< создаем зону
	SSTE_EDIT_ZONE,///< редактируем зону
	SSTE_EDIT_ZONE_AND_SHOW_ALL,///< редактируем и показываемой
	SSTE_CREATE_MASK,///< Создать маску
	SSTE_EDIT_MASK,///< Редктирование маску
	SSTE_EDIT_DIMS,///< редактирование размерностей
	SSTE_EDIT_DIR,///< Редактирование направления
	SSTE_EDIT_STATE_POS,///< Редактируем позицию состояния
	SSTE_EDIT_STATE_CENTER_OFFSET,///< Редактируем смещение анимации
};

enum eRedrawMode {RM_REDRAW_CONTENT, RM_NO_CONTENT_REDRAW};

/*!
Режим работы редактора сцены
*/
enum VIEW_STATE
{
	VS_NONE,///< Ничего не делаем и не рисуем
	VS_EDIT_SCENE,///< Редактируем сцену. Меняем положение объектов, удаляем их и т.д.
	VS_EDIT_NET,///< Редактируем сетку(создаем и редактируем зоны)
	VS_EDIT_COORD_ANIM,///< Редактируем координатную анимацию
	VS_EDIT_GAME_AREA,///< Редактируем игровую область
	VS_EDIT_SCREEN_LAYOUT, ///< Скроллируем экран рукой
	VS_EDIT_MASK,///< Редактируем маску
	VS_EDIT_ANIMATED,///< Настраиваем боунд и ориентацию
	VS_EDIT_STACK_STATE,///< Стыкуем состояние
	VS_EDIT_STATE,///< Редактируем размеры
	VS_EDIT_INVENTORY,///< Редактируем инвентори
	VS_EDIT_INTERFACE,//! Редактируем интерфейс
};

interface IProjectFacade;
class qdCoordsAnimation;
class StateCoordsStacker;
class qdGameObjectState;
class CoordAnimDlg;
interface IWorkOwner;
//! Интерфасе доступа к редактору сцены
interface ISceneViewFacade
{
	virtual void setProject(IProjectFacade* project) = 0;
	virtual IProjectFacade* getProject() const = 0;
	//! Устанавливает значение CChildView::m_bDrawNetProjs
	virtual void SetDrawNetProjs(bool bval) = 0;
	//! Возвращает значение CChildView::m_bDrawNetProjs
	virtual bool GetDrawNetProjs() const = 0;
	//! Устанавливает значение CChildView::m_bDrawBounds
	virtual void SetDrawBounds(bool bval) = 0;
	//! Возвращает значение CChildView::m_bDrawBounds
	virtual bool GetDrawBounds() const = 0;

	//! Начинает создавать зону
	virtual void OnCreateGridZone() = 0;
	//! Начинает редактировать зону
	virtual void OnEditGridZone(class qdGridZone* ptrZone, IWorkOwner* workOwner) = 0;
	//! На прекращение редактирования сеточной зоны
	virtual void OnAbortGridZoneEdit(bool bSave) = 0;

	//! На начало редактирования размеров состояния
	virtual void OnEditStateDims(class StateBoundDlg* pdlg, 
								class qdGameObjectState* pstate) = 0;
	//! На прекращение редктирования размреов
	virtual void OnEndEditStateDims(bool bSave) = 0;

	//! Устанавливает состояние(рабочий режим) редактора сцены
	virtual void SetViewState(VIEW_STATE eVal) = 0;
	//! Возвращает рабочий режим
	virtual VIEW_STATE GetViewState() = 0;
	//! Устанавливает уточнение для текущего состояния(режима работы)
	virtual void SetSubTools(STATE_SUBTOOLS_ENUM ssteVal) = 0;
	//! Возвращает уточнение для режима работы
	virtual STATE_SUBTOOLS_ENUM GetSubTools() = 0;
	//! Выравнивает размер бмп так, чтобы в него влазил контент сцены с напуском
	virtual bool AdjustBitmapSize() = 0;

	virtual void reset() = 0;

	virtual void Redraw(eRedrawMode emode) = 0;
	virtual void SelectObj( class qdGameObject* obj) = 0;

	//! На начало создание маски
	virtual bool OnCreateStateMask(qdGameObject* ptrGO) = 0;
	//! На начало редактирования маски
	virtual bool OnEditStateMask(qdGameObjectStateMask* ptrMask) = 0;
	//! На окончание редактирования маски
	virtual void OnAbortStateMaskEdit(bool bSave) = 0;
	//! Устанавливает активное состояние
	virtual void SetActiveState(qdGameObjectState* pstate) = 0;
	//! Устанавливает активный инвентори
	virtual void SelectInventory(qdInventory* ptrInv) = 0;

	//! Запуск проигрывания видео
	virtual void PlayStopVideo(qdVideo* video) = 0;
	//! Остановка проигрывания видео
	virtual void StopVideo(qdVideo* video) = 0;
	//! Проигрывается ли видео в текущий момент
	virtual bool isVideoPlaying() const = 0;

	//! Запуск проигрывания анимации
	virtual void StartAnimation(qdGameObjectState* state) = 0;
	//! Остановка проигрывания анимации
	virtual void StopAnimation(qdGameObjectState* state) = 0;
	virtual void PauseAnimation() = 0;

	virtual void activate() = 0;
	virtual void deactivate() = 0;

	virtual void BeginEditCoordAnim(qdCoordsAnimation* pca) = 0;
	virtual qdCoordsAnimation& GetCoordsAnimation() = 0;
	virtual void UpdateCoordsAnimation() = 0;
	virtual void EndEditCoordAnim() = 0;

	virtual void SetStateStackerDlg(StateCoordsStacker*) = 0;
	virtual void SetStackedState(qdGameObjectState* ) = 0;

	virtual void SetCoordsAnimationDlg(CoordAnimDlg* ) = 0;

	virtual bool OnEditAnimated(qdGameObjectAnimated* ) = 0;
};