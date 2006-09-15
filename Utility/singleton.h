#	pragma once


namespace Utility
{

	template<class T> 
		inline T* fSngl()
	{
		static T *Value = new T;
		return Value;
	}

	template<
		class T_VALUE, 
		class T_NAMESPACE = T_VALUE
	>
	class CSingleton
	{

	public:

		CSingleton()
		{
		}

		CSingleton(const T_VALUE& value)
		{
			CSingleton::Instance(value);
		}

	public:
		T_VALUE * operator -> ()
		{
			return CSingleton::GetInstance();
		}

		T_VALUE & operator * ()
		{
			return *CSingleton::GetInstance();
		}

	public:
		static T_VALUE* GetInstance(const T_VALUE& value)
		{
			CSingleton::Instance(value);
			return CSingleton::m_Value;
		}

		static T_VALUE* GetInstance()
		{
			CSingleton::Instance();
			return CSingleton::m_Value;
		}

		static void Destroy()
		{
			if( CSingleton::m_Value )
			{
				delete CSingleton::m_Value;
				CSingleton::m_Value = 0;
			}
		}

		template<class T_DESTROYER>
			static void Destroy(T_DESTROYER &Destroyer)
		{
			if( CSingleton::m_Value )
			{
				Destroyer(CSingleton::m_Value);
				CSingleton::m_Value = 0;
			}
		}

	private:
		static void Instance()
		{
			if( CSingleton::m_Value == 0 )
			{
				CSingleton::m_Value = new T_VALUE();
			}
		}

		static void Instance( const T_VALUE& value )
		{
			if( CSingleton::m_Value == 0 )
			{
				CSingleton::m_Live = true;
				CSingleton::m_Value = new T_VALUE(value);
			}
		}
	private:
		static T_VALUE *m_Value;
	};

	template<
		class T_VALUE, 
		class T_NAMESPACE
	>
	T_VALUE *CSingleton<T_VALUE,T_NAMESPACE>::m_Value = 0;

}