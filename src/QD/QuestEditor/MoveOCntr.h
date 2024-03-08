#pragma once

class MoveOCntr
{
public:
	MoveOCntr(Vect2s off, qdGameObjectState* pstate, bool transform_correction = true);
	~MoveOCntr(void);

	//! выполняет работу по определению смещения
	//! возвращает дополнение к текущему смещению состояния
	//! смещает остальные состояния, если надо
	Vect2i operator()(const qdCamera* pcam);

private:
	Vect2i offset_;
	qdGameObjectState* pstate_;

	bool transformCorrection_;

	/// смещает все состояния, кроме текущего,
	/// чтобы скомпенсировать смещение центра объекта
	void adjust_state_offsets(const Vect2i& object_offset);
};
