#	pragma once

#	include "IntrusiveList.h"

namespace Menge
{
	template<class T>
	class IntrusiveSlug
		: public IntrusiveLinked
	{
	public:
		IntrusiveSlug( IntrusiveList<T> & _list )
			: IntrusiveLinked(IntrusiveLinkedSlug())
			, m_end(*_list.end())
			, m_eof(_list.empty())
		{
			typename IntrusiveList<T>::iterator it_begin = _list.begin();
			it_begin->link_before( this );
		}

	public:
		void next_shuffle()
		{
			IntrusiveLinked * pos = this->right();

			while( pos->slug() == true )
			{
				pos = pos->right();
			}

			pos = pos->right();

			while( pos->slug() == true )
			{
				pos = pos->right();
			}

			if( pos == m_end )
			{
				m_eof = true;
			}

			this->unlink();
			pos->link_before( this );
		}

		bool end() const
		{
			return m_eof;
		}

		inline T * operator -> () const
		{
			return static_cast<T *>(this->current());
		}

		inline T * operator * () const
		{
			return static_cast<T *>(this->current());
		}

	protected:
		IntrusiveLinked * current() const
		{
			IntrusiveLinked * pos = this->right();

			while( pos->slug() == true )
			{
				pos = pos->right();
			}

			return pos;
		}

	protected:
		IntrusiveLinked * m_end;
		bool m_eof;
	};
}