#pragma once

class qdGameObjectStateStatic;
class qdGameScene;

/// ѕримен€ет смещение к состо€ни€м, которые используют анимицию из anim_lst 
/**
	»спользуетс€ при обрезке анимации, использующейс€ в нескольких состо€ни€х
 */
class StateOffsetApplyer
{
public:
	StateOffsetApplyer(void);
	~StateOffsetApplyer(void);
	/**
		ѕробегает по всем анимированным объектам сцены и состо€ни€м, использующим
		анимицию из списка примен€ет смещение. query_state - исключаетс€
	 */
	void apply(qdGameObjectStateStatic const* query_state, 
				Vect2i const& offset, 
				qdGameScene* pscene,
				std::vector<LPCTSTR> const& anim_lst);
};
