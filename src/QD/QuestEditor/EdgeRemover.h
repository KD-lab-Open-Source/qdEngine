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

/// ��������� ���������� ��� ������� ������ � �����������
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

	//! ��������� ����������� ��������
	static void remove_edges_interface(qdInterfaceObjectBase* pobj);
	//! ��������� ������� ��������
	static void remove_edges(qdGameDispatcher* gameDisp, qdNamedObject* pobj);

	//! �������� ������ ��� ������������� ����
	static void remove_edges_back(qdInterfaceBackground* pobj);
	//! �������� ������ ��� ��������� ������������� ��������
	static void remove_edges_element_state(qdInterfaceElementState* pobj);
	//! �������� ������ ��� �����
	static void remove_edges_save(qdInterfaceSave* pobj);
	//! �������� ������ �������
	static void remove_edges_slider(qdInterfaceSlider* pobj);
	//! �������� ������ ������
	static void remove_edges_screen(qdInterfaceScreen* pobj);
	//! �������� ������ ������������ ������
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
