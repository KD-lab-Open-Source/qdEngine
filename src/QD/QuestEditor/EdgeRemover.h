#pragma once

#include "disk_op.h"

class qdGameObjectStatic;
class qdGameObjectStateStatic;
class qdGameObjectAnimated;

class qdInterfaceScreen;
class qdInterfaceSave;
class qdInterfaceButton;
class qdInterfaceElement;
class qdInterfaceElementState;
class qdInterfaceSlider;
class qdInterfaceBackground;

/// Реализует функционал для обрезки пустот в изображених
class EdgeRemover
{
	static disk_op::eFileType file_type(CString const& strFile);

	static Vect2i const remove_edges_tga(CString const& strFile);
	static Vect2i const remove_edges_qda(CString const& strFile);

#ifdef USE_NEW_PROJ
	static bool remove_edges(qdGameDispatcher* gameDisp,qdGameObjectStatic* object);
	static bool remove_edges(qdGameDispatcher* gameDisp, qdGameObjectAnimated* object);
	static bool remove_edges(qdGameDispatcher* gameDisp, qdGameObjectStateStatic* object);
	static bool remove_edges(qdGameDispatcher* gameDisp, qdGameScene* scene);

	static void applayStateOffsetToOtherUsers(
								qdGameDispatcher* gameDisp,
								qdGameObjectStateStatic* query_src,
								Vect2i const& offset);

	//! Обработка интерфесных объектов
	static void remove_edges_interface(qdInterfaceObjectBase* pobj);
	//! Обработка игровых объектов
	static void remove_edges(qdGameDispatcher* gameDisp, qdNamedObject* pobj);

	//! Удаление пустот для интерфейсного фона
	static void remove_edges_back(qdInterfaceBackground* pobj);
	//! Удаление пустот для состояния интерфейсного элемента
	static void remove_edges_element_state(qdInterfaceElementState* pobj);
	//! Удаление пустот для сейва
	static void remove_edges_save(qdInterfaceSave* pobj);
	//! Удаление пустот бегунка
	static void remove_edges_slider(qdInterfaceSlider* pobj);
	//! Удаление пустот экрана
	static void remove_edges_screen(qdInterfaceScreen* pobj);
	//! Удаление пустот интерфейсной кнопки
	static void remove_edges_button(qdInterfaceButton* pobj);
#endif // USE_NEW_PROJ

public:
	EdgeRemover(void);
	~EdgeRemover(void);
	Vect2i const operator()(CString const& strFile) const;
	static Vect2i const remove_edges(CString const& strFile, bool bAsk = true);
#ifdef USE_NEW_PROJ
	static void remove_edges(qdGameDispatcher* gameDisp, qdNamedObjectBase* obj);
#endif // USE_NEW_PROJ
};
