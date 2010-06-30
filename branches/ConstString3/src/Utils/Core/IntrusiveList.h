#	pragma once

namespace Menge
{
	template<class T>
	class IntrusiveList
	{
	public:
		IntrusiveList()
			: m_head()
		{

		}

	protected:
		IntrusiveLinked<T> m_head;
	};
}