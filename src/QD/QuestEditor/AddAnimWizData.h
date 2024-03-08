/********************************************************************
	created:	2002/11/07
	created:	7:11:2002   14:55
	filename: 	d:\projects\quest\questeditor\addanimwizdata.h
	file path:	d:\projects\quest\questeditor
	file base:	addanimwizdata
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	структура данных для визарда собирающего анимацию
*********************************************************************/

#ifndef __ADD_ANIM_WIZ_DATA_H__
#define  __ADD_ANIM_WIZ_DATA_H__

/// Что будем делать конвертировать или существующие собирать
enum ADD_ANIM_WAY {
	AAW_NONE = -1,
	AAW_SELECT_EXISTING, ///<  Собирать существующие
	AAW_CONVERT,///< Конвретировать
};

/// Способы обработки переданных каталогов
enum CONVERT_WAY{
	CW_NONE = -1,
	CW_FOLDERS, ///< Набор каталогов
	CW_MOVE_TO_SCENES,///< Набор каталогов. Анимация собирается и кладется в сцену с именем каталога
	CW_SCENE,///< Выбираем каталог сцены, собираем там все, 
			///< и кладем в сцену с именем какталога
	CW_STRICT_FOLDER_STRUCTURE,///< Каталог со строгой структурой
};
/// что длеаем после сборки
enum POST_ADD_WAY{
	PAW_NONE = -1,
	PAW_NOT_CREATE_OBJ,/// Не создаем объекты
	PAW_CREATE_OBJ,/// создаем объект
};

//typedef std::vector<CString> CStringVect_t;
const float INVALID_CADR_DUR = 0.f;
/// Класс, содержащий данные для визарда добавления анимации
class add_anim_wiz_data{
public:
	ADD_ANIM_WAY add_anim_way_;
	CONVERT_WAY convert_way_;
	POST_ADD_WAY post_add_way_;
	/// Передает путь(пути), по которому нужно собирать анимацию
	CStringVect_t files_;
	float cadr_duration_;

	//операцию "добавления" запустили из глобального каталога
	//испльзуется только страничками для установки этого факта
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

