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

		//! Holder normal method.
		/*!
		\sa hostage() destroy()
		\param c1 the first argument.
		*/
		static void keep(T * _hostage)
		{
			m_hostage = _hostage;
		}
			
		//! Holder normal method.
		/*!
		\sa keep() destroy()
		\return a pointer object
		*/
		static T * hostage()
		{
			return m_hostage;
		}

		//! Holder normal method, destroy keeping object
		/*!
		\sa keep() hostage()
		*/
		static void destroy()
		{
			delete m_hostage;
			m_hostage = 0;
		}

		static bool empty()
		{
			return m_hostage == 0;
		}

	private:
		//! Holder private variable.
		/*!
		* a pointer object
		*/
		static T * m_hostage;
	};
 
	template<class T>
	T * Holder<T>::m_hostage = 0;
}