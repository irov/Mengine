#	include "pybind/constructor.hpp"
#	include "pybind/system.hpp"

#	include <stdio.h>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	constructor::constructor( std::size_t _arity )
		: m_arity(_arity)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	constructor::~constructor()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	bool constructor::valid( PyObject * _args ) const
	{
		if( pybind::tuple_check( _args ) == false )
		{
			return false;
		}

		size_t args_arity = pybind::tuple_size( _args );

		if( m_arity != args_arity )
		{
			printf("construct have %d args, but you call %d params"
				, m_arity
				, args_arity
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t constructor::arity() const
	{
		return m_arity;
	}
}
