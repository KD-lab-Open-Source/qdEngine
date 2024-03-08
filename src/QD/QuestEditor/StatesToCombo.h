#pragma once

#include <boost/function.hpp>
class qdGameScene;
class qdGameObjectState;
class qdGameObject;

/**
	Класс утилита заполняющий комбобокс состяниями взятыми из объекта или из сцены.

 */
class StatesToCombo
{
public:
	/// Определяет тип функтора форматирования
	typedef boost::function1<CString, qdGameObjectState const*> Formater;

	/**
		Класс проводит форматирование по умолчанию. В результате получается
		строка, содеражащая имя состояния и имя владельца состояния
	 */
	class DefFormat
	{
		/// Идентификатор ресурсной строки
		UINT_PTR m_uiId;
	public:
		DefFormat();
		DefFormat(UINT_PTR uiId);
		/// Содержит тело функтора
		CString operator()(qdGameObjectState const* pstate) const;
	};

	/**
		Класс форматер, в результате работы которого получается строка, содержащая 
		имя состояния
	 */
	class FormatStateName
	{
	public:
		FormatStateName();
		/// Содержит тело функтора
		CString operator()(qdGameObjectState const* pstate) const;
	};
private:
	/**
		Загружает все состояния объекта в комбобокс, используя m_formater для 
		получения в кобобоксе нужного вида строки. 
	 */
	class ObjStates2Combo
	{
		Formater const& m_formater;
		CCJFlatComboBox& m_combo;
	public:
		ObjStates2Combo(CCJFlatComboBox& combo, Formater const& formater);
		/// Выбирает все состояния объекта и загружает их в комбобокс
		void operator()(qdGameObject const* pobj);
	};

	/**
		Класс загружает переданное ему состояние в комбобокс, используя m_formater для 
		получения в кобобоксе нужного вида строки.
	 */
	class StateToCombo
	{
		Formater const& m_formater;
		CCJFlatComboBox& m_combo;
	public:
		StateToCombo(CCJFlatComboBox& combo, Formater const& formater);
		void operator()(qdGameObjectState const* pstate);
	};
protected:
	~StatesToCombo(void);
public:
	StatesToCombo(void);
	/// Загружает состояния для сцены
	static bool loadFromScene(CCJFlatComboBox& combo, qdGameScene const* pscene, 
		Formater formater = DefFormat());
	/// Загружает состояния для объекта
	static bool loadFromObject(CCJFlatComboBox& combo, qdGameObjectAnimated const* pobj,
		Formater formater = FormatStateName());
};
