#pragma once

class qdGameObjectAnimated;
class qdGameObjectState;
class qdCamera;

///  \brief Смещает центр объекта, а состояниям добавляется смещение так, 
/// чтобы анимация состояний визуально оставалась на том же месте
class MoveObjectCenter
{
public:
	MoveObjectCenter(void);
	~MoveObjectCenter(void);

	void operator()(qdGameObjectAnimated* pobj, 
		qdGameObjectState const* pLeadingState,
		const Vect2i& off, qdCamera const* pcam) const;
};
