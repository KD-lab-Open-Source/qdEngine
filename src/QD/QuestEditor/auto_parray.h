#ifndef AXL_AUTO_POINTER_TO_ARRAY_INCLUDE_H__
#define AXL_AUTO_POINTER_TO_ARRAY_INCLUDE_H__


namespace axl
{
	template<class _Tp>
		class auto_parray
	{
public:
		typedef _Tp	element_type;
		typedef auto_parray<_Tp>	_Self;
		

		auto_parray()
		{
			reset();
		}

		auto_parray(_Tp* _ptr)
		{
			reset();
			attach(_ptr);
		}

		auto_parray(const _Self& _apa)
		{
			reset();
			*this = _ptr;
		}

		_Self& operator=(const _Self& _apa)
		{
			attach(detach());
			return *this;
		}
		
		~auto_parray()
		{
			destroy();
		}
				
		bool is_valid() const	{return m_ptr==0?false:true;}
		
		void destroy()
		{
			if(!is_valid())  return;
			delete[] m_ptr;
			reset();
		}

		_Tp* detach()
		{
			_Tp* __ptr = this->get();
			this->reset();
			return __ptr;
		}

		void attach(_Tp* _ptr)
		{
			if(is_valid())
			{
				destroy();
				reset();
			}
			m_ptr = _ptr;
		}
		
		void reset()
		{
			m_ptr	= 0;
		}
		
		const _Tp* get()	const {return m_ptr;}
		_Tp* get()	{return m_ptr;}
		
		_Tp& operator [] (const long _i) {return m_ptr[_i];}
		const _Tp& operator [] (const long _i) const {return m_ptr[_i];}
				
private:
		_Tp*	m_ptr;
	
	};
}

#endif //AXL_AUTO_POINTER_TO_ARRAY_INCLUDE_H__

