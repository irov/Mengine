#	pragma once

namespace Menge
{ 
	template<class T>
	class Holder
	{
	public:
		static void keep(T * _hostage)
		{
			m_hostage = _hostage;
		}
			
		static T * hostage()
		{
			return m_hostage;
		}

		static void destroy()
		{
			delete m_hostage;
		}

	private:
		static T * m_hostage;
	};
 
	template<class T>
	T * Holder<T>::m_hostage = 0;
}