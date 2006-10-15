#	pragma once

namespace Menge
{
	template<class T>
	class Keeper
	{
	public:
		static void keep( T * _hostage)
		{
			m_hostage = _hostage;
		}
			
		static T * hostage()
		{
			return m_hostage;
		}
	private:
		static T * m_hostage;
	};

	template<class T>
	T * Keeper<T>::m_hostage = 0;
}