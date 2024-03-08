/********************************************************************
	created:	2002/11/07
	created:	7:11:2002   14:55
	filename: 	d:\projects\quest\questeditor\addanimwizdata.h
	file path:	d:\projects\quest\questeditor
	file base:	addanimwizdata
	file ext:	h
	Powerd by:	�����
	
	purpose:	��������� ������ ��� ������� ����������� ��������
*********************************************************************/

#ifndef __ADD_ANIM_WIZ_DATA_H__
#define  __ADD_ANIM_WIZ_DATA_H__

/// ��� ����� ������ �������������� ��� ������������ ��������
enum ADD_ANIM_WAY {
	AAW_NONE = -1,
	AAW_SELECT_EXISTING, ///<  �������� ������������
	AAW_CONVERT,///< ��������������
};

/// ������� ��������� ���������� ���������
enum CONVERT_WAY{
	CW_NONE = -1,
	CW_FOLDERS, ///< ����� ���������
	CW_MOVE_TO_SCENES,///< ����� ���������. �������� ���������� � �������� � ����� � ������ ��������
	CW_SCENE,///< �������� ������� �����, �������� ��� ���, 
			///< � ������ � ����� � ������ ���������
	CW_STRICT_FOLDER_STRUCTURE,///< ������� �� ������� ����������
};
/// ��� ������ ����� ������
enum POST_ADD_WAY{
	PAW_NONE = -1,
	PAW_NOT_CREATE_OBJ,/// �� ������� �������
	PAW_CREATE_OBJ,/// ������� ������
};

//typedef std::vector<CString> CStringVect_t;
const float INVALID_CADR_DUR = 0.f;
/// �����, ���������� ������ ��� ������� ���������� ��������
class add_anim_wiz_data{
public:
	ADD_ANIM_WAY add_anim_way_;
	CONVERT_WAY convert_way_;
	POST_ADD_WAY post_add_way_;
	/// �������� ����(����), �� �������� ����� �������� ��������
	CStringVect_t files_;
	float cadr_duration_;

	//�������� "����������" ��������� �� ����������� ��������
	//����������� ������ ����������� ��� ��������� ����� �����
	bool m_bUseGlobalCatalog;
	add_anim_wiz_data():
		add_anim_way_(AAW_NONE), 
		convert_way_(CW_NONE),
		post_add_way_(PAW_NONE),
		m_bUseGlobalCatalog(false),
		cadr_duration_(INVALID_CADR_DUR)
	{
	}
};
#endif

