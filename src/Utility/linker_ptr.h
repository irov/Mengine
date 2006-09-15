#	pragma once

namespace Utility
{
	template<class T>
	class linker_ptr
	{
		typedef unsigned int tick_t;

	public:
		inline linker_ptr(T * Object)
			: m_Object(Object)
			, m_Tick(0)
		{
		}

		inline linker_ptr(T * Object,tick_t Tick)
			: m_Object(Object)
			, m_Tick(Tick)
		{
		}

	public:
		inline bool check(tick_t Tick)
		{
			return m_Tick == Tick;
		}

		inline void update(tick_t Tick)
		{
			m_Tick = Tick;
		}

	public:
		inline T * operator -> ()
		{
			return m_Object;
		}

		inline T & operator * ()
		{
			return *m_Object;
		}

		inline operator T *()
		{
			return m_Object;
		}

	private:
		T * m_Object;
		tick_t m_Tick;	
	};
}