
#pragma	once


class qdGameObjectState;
/// \brief 	Интерфейс, который реализует класс(обычно класс диалога), желающий выполнять
/// стыковку состояния, редактирование размеров или редактирование КА состояния
struct IRequestingStateEdit
{
	/// Возвращает редактируемое состояние
	virtual qdGameObjectState* GetState() = 0;
	/// Вызывается, когда стыковка завершена
	virtual void EndStackState() = 0;
	/// Вызывается, когда редактирование размеров завершено
	virtual void EndEditStateDims() = 0;
	/// Вызывается, когда редактирование КА завершено
	virtual void EndEditCoordAnimation() = 0;
};