#include "PythonScriptWrapper.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#   include "pybind/pybind.hpp"
#include "pybind/stl_type_cast.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    static bool s_ConstString_compare( pybind::kernel_interface * _kernel, PyObject * _obj, ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
    {
        (void)_obj;

        ConstString cs_compare;
		if( pybind::extract_value( _kernel, _compare, cs_compare, false ) == false )
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
	static bool ConstString_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * _user )
    {
		(void)_kernel;
        (void)_user;

		ConstString & cstr = *(ConstString*)_place;
		
		if( pybind::string_check( _obj ) == true )
		{
			if( SCRIPT_SERVICE()
				->stringize( _obj, cstr ) == false )
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
	static bool s_FilePath_compare( pybind::kernel_interface * _kernel, PyObject * _obj, FilePath * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
	{
		(void)_obj;

		FilePath cs_compare;
		if( pybind::extract_value( _kernel, _compare, cs_compare, false ) == false )
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
	static const char * s_FilePath_repr( FilePath * _cs )
	{
		const char * str_repr = _cs->c_str();

		return str_repr;
	}
	//////////////////////////////////////////////////////////////////////////
	static ConstString::hash_type s_FilePath_hash( FilePath * _cs )
	{
		ConstString::hash_type hash = _cs->hash();

		return hash;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool FilePath_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * _user )
	{
		(void)_kernel;
        (void)_user;

		ConstString & cstr = *(ConstString*)_place;

		if( pybind::string_check( _obj ) == true )
		{
			if( SCRIPT_SERVICE()
				->stringize( _obj, cstr ) == false )
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
	void PythonScriptWrapper::constsWrap()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::registration_stl_vector_type_cast<ConstString, TVectorConstString>(kernel);

		pybind::structhash_<ConstString>( kernel, "ConstString" )
            .def_compare( &s_ConstString_compare )
            .def_convert( &ConstString_convert, nullptr )
            .def_repr( &s_ConstString_repr )
            .def_hash( &s_ConstString_hash )
            ;

		pybind::structhash_<FilePath>( kernel, "FilePath" )
			.def_compare( &s_FilePath_compare )
			.def_convert( &FilePath_convert, nullptr )
			.def_repr( &s_FilePath_repr )
			.def_hash( &s_FilePath_hash )
			;
	}
}

