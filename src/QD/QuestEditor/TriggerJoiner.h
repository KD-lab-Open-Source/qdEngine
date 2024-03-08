/********************************************************************
	created:	2003/04/26
	created:	26:4:2003   15:53
	filename: 	d:\Projects\Quest\QuestEditor\TriggerJoiner.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TriggerJoiner
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Выполняет работу по объединению триггеров.
*********************************************************************/

#pragma once

#include <hash_map>

class qdTriggerChain;
class qdTriggerElement;
class qdTriggerLink;

/**
	Объединяет два триггера
 */
class TriggerJoiner
{
	/// Хэш для индекса элементов
	struct hashik {
		/// Возвращает указатель, хранимый в смарт-поинтере
		size_t operator() (qdTriggerElementPtr p) const{
			return reinterpret_cast<size_t>(boost::get_pointer(p));
		}
	};
	typedef std::hash_map<qdTriggerElementPtr, qdTriggerElementPtr, hashik> EleTable;
public:
	TriggerJoiner();
	~TriggerJoiner();

	/// Объединяет цепочки триггеров
	bool join(qdTriggerChain const* from, 
			qdTriggerChain* to, qdTriggerElementList& lst_ele);

private:
	class ele_copier;
	friend class ele_copier;
	/// Копирует элементы в целевую цепочку
	class ele_copier
	{
	public:
		ele_copier(TriggerJoiner::EleTable& ele_table, qdTriggerChain* ptrTo);
		/// Добавляет элемент в целвую цепочку и копирует его местоположение
		void operator() (qdTriggerElementPtr pele);
	private:
		/// Копирует местоположение и обновляет ссылки на объекты
		void layout_copy(qdTriggerElementConstPtr from, qdTriggerElementPtr to) const;
	private:
		/// Индекс элементов
		TriggerJoiner::EleTable& m_EleTable;
		/// Целевая цепочка
		qdTriggerChain* m_ptrTo;
	};

	class links_copier;
	friend class links_copier;
	/// Копирует линки скопированных элементов
	class links_copier
	{
		typedef EleTable::value_type value_type;
		friend class link_copier;
		/// Копирует линк
		class link_copier
		{
		public:
			link_copier(EleTable& ele_tbl, qdTriggerChain* pto, qdTriggerElementPtr from);
			/// Добавляет линк между элементами в целевой цепочке.
			void operator()(qdTriggerLink const& lnk) const;
		private:
			/// Задает местоположение нового линка по исходному (from)
			void layout_copy(qdTriggerLink const& from, 
										qdTriggerElementConstPtr to) const;
		private:
			EleTable& m_EleTable;
			qdTriggerChain* m_ptrTo;
			qdTriggerElementPtr m_ptrFrom;
		};
	public:
		links_copier(TriggerJoiner::EleTable& ele_table, qdTriggerChain* pto);
		/// Пробегается по всем линкам элемента и копирует их
		void operator()(value_type& vt);
	private:
		/// Таблица соответсвия элементов
		TriggerJoiner::EleTable& m_EleTable;
		/// Цепочка, в которой связи будут добавляться
		qdTriggerChain* m_ptrTo;
	};
	friend struct select_2nd;
	/// Возвращает second для EleTable::value_type
	struct select_2nd {
		typedef EleTable::value_type argument_type;
		typedef qdTriggerElementPtr const result_type;
		qdTriggerElementPtr const& operator()(EleTable::value_type const&vt) const{
			return vt.second;
		}
	};

private:

	/// Хранит соответствия элементов исходного триггера элементам нового.
	/**
		ключ - исходный элемент, значение - новый элемент
		Используется для быстрого нахождения дочернего элемента для линка 
		в новой цепочке
	 */
	EleTable m_EleTable;
};
