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
			: IntrusiveLinked(EILT_SLUG)
			, m_end(*_list.end())
		{
			_list.push_front( this );
		}

	public:
		void next_shuffle()
		{
			IntrusiveLinked * pos = this->right();

			pos = this->adapt_( pos );

			pos = pos->right();

			pos = this->adapt_( pos );

			this->unlink();
			pos->link_before( this );
		}

		bool end() const
		{
			IntrusiveLinked * pos = this->current();

			return pos == m_end;
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

			pos = this->adapt_( pos );

			return pos;
		}

		IntrusiveLinked * adapt_( IntrusiveLinked * _pos ) const
		{
			while( _pos->isTag( EILT_SLUG ) == true )
			{
				_pos = _pos->right();
			}

			return _pos;
		}
		
	protected:
		const IntrusiveLinked * m_end;
	};
}