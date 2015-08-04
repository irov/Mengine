#	include "ScriptWrapper.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    static bool s_ConstString_compare( pybind::kernel_interface * _kernel, PyObject * _obj, ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
    {
		(void)_kernel;
        (void)_obj;

        ConstString cs_compare;
        if( pybind::extract_value( _compare, cs_compare ) == false )
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
    static const char * s_ConstString_repr( pybind::kernel_interface * _kernel, ConstString * _cs )
    {
		(void)_kernel;

        const char * str_repr = _cs->c_str();

        return str_repr;
    }
    //////////////////////////////////////////////////////////////////////////
	static ConstString::hash_type s_ConstString_hash( pybind::kernel_interface * _kernel, ConstString * _cs )
    {
		(void)_kernel;

 		ConstString::hash_type hash = _cs->hash();
			
		return hash;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool ConstString_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * _user )
    {
		(void)_kernel;

        if( pybind::string_check( _obj ) == false )
        {
            return false;
        }

        ServiceProviderInterface * serviceProvider = static_cast<ServiceProviderInterface *>(_user);

        ConstString & cstr = *(ConstString*)_place;

        if( SCRIPT_SERVICE(serviceProvider)
            ->stringize( _obj, cstr ) == false )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void ScriptWrapper::constsWrap( ServiceProviderInterface * _serviceProvider )
	{
        pybind::struct_<ConstString>("ConstString")
            .def_compare( &s_ConstString_compare )
            .def_convert( &ConstString_convert, _serviceProvider )
            .def_repr( &s_ConstString_repr )
            .def_hash( &s_ConstString_hash )
            ;
	}
	//////////////////////////////////////////////////////////////////////////
    void ScriptWrapper::constsUnwrap( ServiceProviderInterface * _serviceProvider )
    {
        (void)_serviceProvider;
    }
}

