#ifndef __AUTO_OBJECT_H_INCLUDED__
#define __AUTO_OBJECT_H_INCLUDED__

#include "object_eraser.h"

namespace axl
{
	template <class _Handle, class _Eraser = object_eraser<_Handle>, long __invalid_handle_value = 0>
	class auto_object {
		
		typedef auto_object<_Handle, _Eraser, __invalid_handle_value> _Self;
		
	protected:
		_Handle m_handle;
		
	public:
		typedef _Handle handle_type;
		typedef _Eraser eraser_type;
		
		auto_object() : m_handle((_Handle)__invalid_handle_value) { }
		explicit auto_object(_Handle __handle) : m_handle(__handle) { }
		auto_object(_Self &__x) : m_handle(__x.detach()) { }
		
		_Self &operator = (_Self &__x) {
			attach(__x.detach());
			return *this;
		}
		
		~auto_object() {
			destroy();
		}
		
		_Handle get() const {
			return m_handle;
		}
		
		void attach(_Handle __handle = (_Handle)__invalid_handle_value) {
			if (m_handle != __handle) {
				if (is_valid())
					_Eraser()(m_handle);
				m_handle = __handle;
			}
		}

		void destroy() {
			if (is_valid())
				_Eraser()(m_handle);
			reset();
		}
			
		_Handle detach() {
			_Handle __handle = m_handle;
			reset();
			return __handle;
		}
		
		bool is_valid() {
			return m_handle!=(_Handle)__invalid_handle_value;
		}

		operator _Handle() const {
			return m_handle;
		}

		void reset() {
			m_handle=(_Handle)__invalid_handle_value;
		}
	};
}

#endif // __AUTO_OBJECT_H_INCLUDED__
