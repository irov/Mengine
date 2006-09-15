#	pragma once

#	include <vector>

#	include <algorithm>

namespace Utility
{
	template<typename T_Container> inline
		void vector_clear( T_Container &Container )
	{
		Container.erase(Container.begin(),Container.end());
	}

	template<typename T> inline
		void vector_smart_erase(std::vector<T> &Container,const typename std::vector<T>::iterator &It)
	{
		const T &Value = Container.back();
		(*It) = Value;
		Container.pop_back();
	}

	template<typename T> inline
		void vector_smart_erase(std::vector<T> &Container,size_t N)
	{
		const T &Value = Container.back();
		Container[N] = Value;
		Container.pop_back();
	}

	template<typename T> inline
		void vector_smart_erase(std::vector<T> &Container, const T &Value)
	{
		typename std::vector<T>::iterator It = 
			std::find(Container.begin(),Container.end(),Value);

		if( It != Container.end() )
		{
			vector_smart_erase(Container,It);
		}
	}


	template<typename T, typename T_Function>
		void vector_smart_remove_if(std::vector<T> &Container, T_Function Function)
	{
		typedef std::vector<T> TContainer;
		
		TContainer::iterator it_end = Container.end();
		TContainer::iterator it = Container.begin();

		while(it != it_end)
		{
			if( Function(*it) == true )
			{
				--it_end;
				*it = *it_end;
			}
			else
			{
				++it;
			}
		}
	}

	template<typename T, typename T_Function>
		void vector_smart_leave_if(std::vector<T> &Container, T_Function Function )
	{
		typedef std::vector<T> TContainer;

		TContainer::iterator it_end = Container.end();
		TContainer::iterator it = Container.begin();

		while(it != it_end)
		{
			if( Function(*it) == false )
			{
				--it_end;
				*it = *it_end;
			}
			else
			{
				++it;
			}
		}
	}
}