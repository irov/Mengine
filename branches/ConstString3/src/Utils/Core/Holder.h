#	pragma once

namespace Menge
{ 
	//! Holder - keeper class template
	/*! 
	* template for storing copies of class that would later be used
	*/
	template<class T>
	class Holder
	{
	public:

		Holder()
		{
			keep( static_cast<T*>( this ) );
		}

		virtual ~Holder()
		{
			clear();
		}

		static void keep(T * _hostage)
		{
			m_hostage = _hostage;
		}
			
		static T * get()
		{
			return m_hostage;
		}

		static bool empty()
		{
			return m_hostage == 0;
		}

		static void clear()
		{
			m_hostage = 0;
		}

	private:
		static T * m_hostage;
	};
 
	template<class T>
	T * Holder<T>::m_hostage = 0;
}
