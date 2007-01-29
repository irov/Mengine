#	pragma once

struct lua_State;

struct end_call_function_format{};

end_call_function_format end_args();

class lau_functor
{
public:
	lau_functor( lua_State * _state , const char * _name );

	template<class T>
	lau_functor & operator % ( T value )
	{ 
		apply( value ); 
		++m_args;
		return *this; 
	}

	void operator % ( end_call_function_format );

	void apply( bool b );
	void apply( int i );
	void apply( size_t u );
	void apply( float f );
	void apply( const char * s );

private:
	struct lua_State * m_state;
	const char * m_name;

	int m_args;
	int m_result;
};