#pragma once


/// Цвет сетки
const COLORREF NET_COLOR = RGB(192, 192, 192);
/// Цвет нормального элемента
COLORREF const TE_NORMAL_ELE_BACK = RGB(255, 255, 255);
/// Цвет выбранного элемента
COLORREF const TE_SELECTED_ELE_BACK = RGB(255, 0, 0);
/// Количество типов линков
int const LINK_TYPES_COUNT = 11;
/// Цвет связи с признаком "Авторестарт"
COLORREF const AUTO_RESTART_PEN_COLOR = 0;

/// Ширина части элемента, которая окрашивается. Например при выборе
const int RECT_INCREASE = 4;

const int INIT_X_WINDOW_EXT = 1000;
const int INIT_Y_WINDOW_EXT = 1000;
const int INIT_X_VIEWPORT_EXT = 1000;
const int INIT_Y_VIEWPORT_EXT = 1000;

/// Шаг, на который изменяется масштаб
const float SCALE_STEP = .85f;

/**
По-моему, нигде не используется
const int TOP_OFFSET	= 20;
const int LEFT_OFFSET	= 20;
*/
/**\name	Цвета для типов линков
 */
//@{
COLORREF const TE_TYPE0_COLOR = RGB(0, 255, 0);
COLORREF const TE_TYPE1_COLOR = RGB(0, 0, 255);
COLORREF const TE_TYPE2_COLOR = RGB(255, 0, 0);
COLORREF const TE_TYPE3_COLOR = RGB(255, 255, 0);
COLORREF const TE_TYPE4_COLOR = RGB(0, 255, 255);
COLORREF const TE_TYPE5_COLOR = RGB(255, 0, 128);
COLORREF const TE_TYPE6_COLOR = RGB(255, 128, 0);
COLORREF const TE_TYPE7_COLOR = RGB(198, 198, 0);
COLORREF const TE_TYPE8_COLOR = RGB(100, 0, 0);
COLORREF const TE_TYPE9_COLOR = RGB(0, 100, 0);
COLORREF const TE_TYPE10_COLOR = RGB(169, 151, 251);
//@}

/// Цвет отработанного элемента
COLORREF const TE_DONED_LINK_COLOR = RGB(0, 0, 0);
/// Цвет связи, когда её нельзя установить
COLORREF const TE_CANNT_COLOR = RGB(200, 0, 0);
const int RT_STOP_RECT = 0;
const int RT_NORMAL_RECT = 1;

/// Длинна стороны квадрата якоря у выбранного линка
const int ANCHOR_SZ		= 4;

/// Набор символов, разделяющий имена объектов в заголовке элемента
char const *const OBJ_NAME_DELIMETER = _T("::");
const int COLOR_PART_WIDTH = 20;

///отрисовка
const COLORREF ANCHOR_COLOR = RGB(0,0,200);
/// Цвет отработавшего линка в профайлере
const COLORREF DONED_LINK_CLR = RGB(0, 150, 0);
/// Цвет активного линка в профайлере
const COLORREF ACTIVE_LINK_CLR = RGB(250, 0, 0);
/// Цвет неактивного линка в профайлере
const COLORREF INACTIVE_LINK_CLR = RGB(128, 128, 128);

/// Цвет неактивного элемента в профайлере
const COLORREF INACTIVE_ELE_CLR = RGB(192, 192, 192);
/// Цвет ожидающего элемента в профайлере
const COLORREF WAITING_ELE_CLR = RGB(255, 128, 64);
/// Цвет работающего элемента в профайлере
const COLORREF WORKING_ELE_CLR = RGB(250, 0, 0);
/// Цвет отработавшего элемента в профайлере
const COLORREF DONED_ELE_CLR = RGB(0, 150, 0);

/// Цвет текста корневого элемента
COLORREF const START_ELE_TITLE_COLOR = RGB(255, 0, 0);
/// Цвет текста элемента сцены
COLORREF const SCENE_ELE_TITLE_COLOR = RGB(255, 0, 0);

/// Цвет исходящих связей
const COLORREF LINK_OUT_COLOR		= RGB(200, 200, 0);
/// Цвет входящих связей
const COLORREF LINK_IN_COLOR		= RGB(255, 0, 0);
/// Цвет остальных связей
const COLORREF OTHER_LINKS_COLOR	= RGB(128, 128, 128);