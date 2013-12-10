#	include "ScriptWrapper.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    static bool s_ConstString_compare( PyObject * _obj, ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
    {
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
    static PyObject * s_ConstString_repr( PyObject * _obj, ConstString * _cs )
    {
        (void)_obj;

        String str_repr = _cs->c_str();

        return pybind::ptr( str_repr );
    }
    //////////////////////////////////////////////////////////////////////////
    static size_t s_ConstString_hash( PyObject * _obj, ConstString * _cs )
    {
        (void)_obj;

		size_t hash = pybind::_get_string_hash( _cs->c_str(), _cs->size() );
			
		return hash;
    }
    //////////////////////////////////////////////////////////////////////////
    class ConstsScriptMethod
    {
    public:
        ConstsScriptMethod( ServiceProviderInterface * _serviceProvider )
            : m_serviceProvider(_serviceProvider)
        {
        }
    
    protected:
        ServiceProviderInterface * m_serviceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    static bool ConstString_convert( PyObject * _obj, void * _place, void * _user )
    {
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
    static ConstsScriptMethod * constScriptMethod = nullptr;
    //////////////////////////////////////////////////////////////////////////
	void ScriptWrapper::constsWrap( ServiceProviderInterface * _serviceProvider )
	{
        pybind::struct_<ConstString>("ConstString")
            .def_compare( &s_ConstString_compare )
            .def_convert( &ConstString_convert, _serviceProvider )
            .def_repr( &s_ConstString_repr )
            .def_hash( &s_ConstString_hash )
            ;

        //pybind::registration_type_cast<ConstString>( new extract_ConstString_type(_serviceProvider) );

        constScriptMethod = new ConstsScriptMethod(_serviceProvider);
        	
        PyObject * module = pybind::get_currentmodule();

        pybind::interface_<ConstsScriptMethod>("ConstsScriptMethod")
            ;

        PyObject * py_dc = pybind::ptr( constScriptMethod );
                
        pybind::set_attr( module, "Consts", py_dc );

#   define DEF_CONST_STRING( serviceProvider, S )\
        {\
        ConstString cstr = Helper::stringizeString(serviceProvider, S);\
        PyObject * py_obj = pybind::ptr( cstr );\
        pybind::set_attr( py_dc, S, py_obj );\
        }

        DEF_CONST_STRING( _serviceProvider, "Main" );
        DEF_CONST_STRING( _serviceProvider, "Scene" );
        DEF_CONST_STRING( _serviceProvider, "Node" );
        DEF_CONST_STRING( _serviceProvider, "Layer2D" );
        DEF_CONST_STRING( _serviceProvider, "TextField" );
        DEF_CONST_STRING( _serviceProvider, "Sprite" );
        DEF_CONST_STRING( _serviceProvider, "ParticleEmitter" );
        DEF_CONST_STRING( _serviceProvider, "HotSpot" );
        DEF_CONST_STRING( _serviceProvider, "Movie" );
        DEF_CONST_STRING( _serviceProvider, "Point" );
        DEF_CONST_STRING( _serviceProvider, "Video" );
        DEF_CONST_STRING( _serviceProvider, "Animation" );
        DEF_CONST_STRING( _serviceProvider, "Window" );
        DEF_CONST_STRING( _serviceProvider, "HotSpotImage" );
        DEF_CONST_STRING( _serviceProvider, "WhitePixel" );

        DEF_CONST_STRING( _serviceProvider, "AccountEnumerator" );
        DEF_CONST_STRING( _serviceProvider, "DefaultAccountID" );
        DEF_CONST_STRING( _serviceProvider, "SelectAccountID" );
        DEF_CONST_STRING( _serviceProvider, "Account" );

#   undef DEF_CONST_STRING
	}

    void ScriptWrapper::constsUnwrap( ServiceProviderInterface * _serviceProvider )
    {
        (void)_serviceProvider;

        delete constScriptMethod;

        PyObject * module = pybind::get_currentmodule();

        pybind::set_attr( module, "Consts", pybind::get_none() );
    }
}

