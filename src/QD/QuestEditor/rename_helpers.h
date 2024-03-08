#ifndef _RENAME_HELPERS_HEADER_
#define _RENAME_HELPERS_HEADER_


namespace rename_h{
	template<class _CondPred, class _CondAction>
	class enum_conds{
		_CondPred& pred_;
		_CondAction& action_;
	public:
		enum_conds(_CondPred& pred, _CondAction& action):pred_(pred), action_(action){}

		void operator()(qdConditionalObject* pCondObj){
			const int condsCount = pCondObj->conditions_count();
			for(int i = 0; i < condsCount; ++i)
			{
				const qdCondition* pCond = pCondObj->get_condition(i);
				if (pred_(pCond))
					action_(pCondObj, i);
			}
		}
	};

	template<class _CondPred, class _CondAction>
	class	enum_states{
		_CondPred& pred_;
		_CondAction& action_;
	public:
		enum_states(_CondPred& pred, _CondAction& action):pred_(pred), action_(action){}
		void operator()(const qdGameObject* pObj) const{
			if (ptree_helpers::is_obj_static(pObj))
				return;

			const qdGameObjectAnimated* pAnimObj = 
				static_cast<const qdGameObjectAnimated*>(pObj);
			const qdGameObjectStateVector& v = pAnimObj->state_vector();
			std::for_each(v.begin(), v.end(),enum_conds<_CondPred, _CondAction>(pred_, action_));
		}
	};

	template<class _CondPred, class _CondAction>
	class enum_objects
	{
		_CondPred& pred_;
		_CondAction& action_;
	public:
		enum_objects(_CondPred& pred, _CondAction& action):pred_(pred), action_(action){}
		void operator() (qdGameScene* pScene) {
			enum_conds<_CondPred, _CondAction>(pred_, action_)(pScene);

			const qdGameObjectList& l = pScene->object_list();
			std::for_each(l.begin(), l.end(), 
				enum_states<_CondPred, _CondAction>(pred_, action_));
		}
	};

	class state_cond_pred
	{
		const qdGameObjectState* pState_;
		LPCTSTR oldName_;
	public:
		state_cond_pred(const qdGameObjectState* pState,LPCTSTR oldName)
			:pState_(pState), oldName_(oldName){}

		bool operator()(const qdCondition* p){
			ASSERT(p);
			if (p->type() == qdCondition::CONDITION_OBJECT_STATE)
			{
				LPCTSTR name = NULL;
				p->get_value(qdCondition::OBJECT_NAME, name);
				if (_tcscmp(name, pState_->owner()->name())) 
					return false;

				p->get_value(qdCondition::OBJECT_STATE_NAME, name);
				if (!_tcscmp(name, oldName_)) 
					return true;
			}
			return false;
		}
	};

	class state_cond_action
	{
		const qdGameObjectState* pState_;
		qdCondition cond_;
	public:
		state_cond_action(const qdGameObjectState* pState):pState_(pState),
			cond_(qdCondition::CONDITION_OBJECT_STATE)
		{
			cond_.put_value(qdCondition::OBJECT_NAME, pState_->owner()->name());
			cond_.put_value(qdCondition::OBJECT_STATE_NAME, pState_->name());
		}
		void operator()(qdConditionalObject* pObj, int idx){
			ASSERT(pObj);
			pObj->update_condition(idx, cond_);
		}
	};

}
#endif//_RENAME_HELPERS_HEADER_