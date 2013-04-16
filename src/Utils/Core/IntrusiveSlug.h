#	pragma once

#	include "IntrusiveList.h"

namespace Menge
{
	template<class T>
	class IntrusiveSlug
		: public IntrusiveLinked<T>
	{
    public:
        typedef IntrusiveLinked<T> TLinked;

	public:
		inline IntrusiveSlug( IntrusiveList<T> & _list )
			: TLinked(EILT_SLUG)
			, m_end(*_list.end())
		{
			_list.push_front( this );
		}

	public:
		void next_shuffle()
		{
			TLinked * pos = this->right();

			pos = this->adapt_( pos );

			pos = pos->right();

			pos = this->adapt_( pos );

			this->unlink();
			pos->link_before( this );
		}

		inline bool eof() const
		{
			TLinked * pos = this->current();

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
		inline TLinked * current() const
		{
			TLinked * pos = this->right();

			TLinked * current = this->adapt_( pos );

			return current;
		}

		inline TLinked * adapt_( TLinked * _pos ) const
		{
			while( _pos->getIntrusiveTag() == EILT_SLUG )
			{
				_pos = _pos->right();
			}

			return _pos;
		}
		
	protected:
		const TLinked * m_end;
	};
}