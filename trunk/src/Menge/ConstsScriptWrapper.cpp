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

        size_t hash = _cs->hash();

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
    
    public:
        PyObject * s_createConstString( PyObject * _str )
        {       
            bool valid = false;

            ConstString cstr = SCRIPT_SERVICE(m_serviceProvider)
                ->stringize( _str, valid );
            //ConstString cstr = Helper::StringizeString(m_serviceProvider, _str);

            if( valid == false )
            {
                pybind::throw_exception();

                return pybind::ret_none();
            }

            PyObject * py_obj = pybind::ptr( cstr );

            return py_obj;
        }

    protected:
        ServiceProviderInterface * m_serviceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    static ConstsScriptMethod * constScriptMethod = 0;
    //////////////////////////////////////////////////////////////////////////
	void ScriptWrapper::constsWrap( ServiceProviderInterface * _serviceProvider )
	{
        pybind::struct_<ConstString>("ConstString")
            .def_compare( &s_ConstString_compare )
            //.def_convert( &ScriptMethod::vec2f_convert )
            .def_repr( &s_ConstString_repr )
            .def_hash( &s_ConstString_hash )
            ;

        constScriptMethod = new ConstsScriptMethod(_serviceProvider);
        	
        pybind::def_functor( "createConstString", constScriptMethod, &ConstsScriptMethod::s_createConstString );

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

