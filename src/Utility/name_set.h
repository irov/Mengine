#	pragma once

#	include <set>

namespace Utility
{
	class name_set
	{
	public:
		inline bool insert(const std::string &Name)
		{
			if( find(Name) == false )
			{
				m_SetName.insert(Name);
				return false;
			}
			return true;
		}

		inline bool find(const std::string & Name)
		{
			TSetName::const_iterator it_find = m_SetName.find(Name);
			if( it_find != m_SetName.end() )
			{
				return true;
			}
			return false;
		}

		inline void erase(const std::string & Name)
		{
			m_SetName.erase(Name);
		}

		inline void clear()
		{
			m_SetName.clear();
		}

		inline size_t size()
		{
			m_SetName.size();
		}

	private:
		typedef std::set<std::string> TSetName;
		TSetName	m_SetName;
	};
}