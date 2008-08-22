#	include "XmlParserCast.h"

//////////////////////////////////////////////////////////////////////////
namespace XmlParserCast
{
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static bool attribute_value_cast_format( T & _var, const Menge::TChar * _value, const Menge::TChar * _format )
	{
		int result = STDSSCANF( _value, _format, &_var);

		return result == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( bool & _var, const Menge::TChar * _value )
	{
		unsigned int wrapp_value;
		if( attribute_value_cast_format( wrapp_value, _value, MENGE_TEXT("%u") ) == false )
		{
			return false;
		}

		_var = (wrapp_value != 0);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( int & _var, const Menge::TChar * _value )
	{
		return attribute_value_cast_format( _var, _value, MENGE_TEXT("%d") );
	}

	////////////////////////////////////////////////////////////////////////////
	//bool attribute_value_cast( unsigned int & _var, const Menge::TChar * _value )
	//{
	//	return attribute_value_cast_format( _var, _value, "%u" );
	//}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( std::size_t & _var, const Menge::TChar * _value )
	{
		return attribute_value_cast_format( _var, _value, MENGE_TEXT("%d") );
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( float & _var, const Menge::TChar * _value )
	{
		return attribute_value_cast_format( _var, _value, MENGE_TEXT("%f") );
	}
	//////////////////////////////////////////////////////////////////////////
	bool attribute_value_cast( Menge::String& _var, const Menge::TChar * _value )
	{
		_var.assign( _value );

		return true;
	}
}