#	pragma once

#   include "Config/Typedef.h"

namespace Menge
{
	template<class I, class F>
	I intrusive_find_if( I _begin, I _end, F _pred )
	{
		for( I
			it = _begin,
			it_end = _end;
		it != it_end;
		++it )
		{
			if( _pred( *it ) == true )
			{
				return it;
			}
		}

		return _end;
	}

	template<class I, class T>
	I intrusive_find( I _begin, I _end, T * _node )
	{
		I it_find(_node);

		for( I
			it = _begin,
			it_end = _end;
		it != it_end;
		++it )
		{
			if( it == it_find )
			{
				return it;
			}
		}

		return _end;
	}

    template<class I, class T>
    bool intrusive_has( I _begin, I _end, T * _node )
    {
        I it_find(_node);

        for( I
            it = _begin,
            it_end = _end;
        it != it_end;
        ++it )
        {
            if( it == it_find )
            {
                return true;
            }
        }

        return false;
    }

	template<class I>
	I intrusive_advance( I _it, I _end, size_t _count )
	{
		for( size_t i = 0; i != _count; ++i )
		{
			if( _it != _end )
			{
				++_it;
			}
			else
			{
				return _end;
			}
		}

		return _it;
	}

	template<class I>
	size_t intrusive_distance( I _begin, I _end )
	{
		size_t count = 0;
		for(; _begin != _end; ++_begin )
		{
			++count;
		}

		return count;
	}
}