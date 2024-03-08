#if !defined _TEXT_ID_SELECTOR_H

#define _TEXT_ID_SELECTOR_H

class qdTextDB;
/**
	Класс-утилита. Позволяет выбрать идентификатор для строки текста из базы 
	тестов
 */
class TextIdSelector{
public:
	/// Выполняет всю необходимую работу
	static bool run(HWND hParent, qdTextDB const* ptrTextDB, CString& textID);
};
#endif//_TEXT_ID_SEwLECTOR_H