#	include "PythonScriptWrapper.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    static bool s_ConstString_compare( PyObject * _obj, ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
    {
        (void)_obj;

        ConstString cs_compare;
        if( pybind::extract_value( _compare, cs_compare, false ) == false )
        {
            return false;
        }

        switch( _op )
        {
        case pybind::POC_Less:
            {
                _result = *_self < cs_compare;
            }break;
        case pybind::POC_Lessequal:
            {
                _result = *_self <= cs_compare;
            }break;
        case pybind::POC_Equal:
            {
                _result = *_self == cs_compare;
            }break;
        case pybind::POC_Notequal:
            {
                _result = *_self != cs_compare;
            }break;
        case pybind::POC_Great:
            {
                _result = *_self > cs_compare;
            }break;
        case pybind::POC_Greatequal:
            {
                _result = *_self >= cs_compare;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static const char * s_ConstString_repr( ConstString * _cs )
    {
        const char * str_repr = _cs->c_str();

        return str_repr;
    }
    //////////////////////////////////////////////////////////////////////////
	static ConstString::hash_type s_ConstString_hash( ConstString * _cs )
    {
 		ConstString::hash_type hash = _cs->hash();
			
		return hash;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool ConstString_convert( PyObject * _obj, void * _place, void * _user )
    {
		ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(_user);

		ConstString & cstr = *(ConstString*)_place;
		
		if( pybind::string_check( _obj ) == true )
		{
			if( SCRIPT_SERVICE( serviceProvider )
				->stringize( (ScriptObject *)_obj, cstr ) == false )
			{
				return false;
			}

			return true;
		}
		else if( pybind::is_none( _obj ) == true )
		{
			cstr = ConstString::none();

			return true;
		}
        
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
	void PythonScriptWrapper::constsWrap( ServiceProviderInterface * _serviceProvider )
	{
		pybind::structhash_<ConstString>( "ConstString" )
            .def_compare( &s_ConstString_compare )
            .def_convert( &ConstString_convert, _serviceProvider )
            .def_repr( &s_ConstString_repr )
            .def_hash( &s_ConstString_hash )
			.def_as_string()
            ;
	}
}

