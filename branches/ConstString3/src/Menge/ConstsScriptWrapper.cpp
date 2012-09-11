#	include "ScriptWrapper.h"

#	include "ScriptEngine.h"

#	include "Consts.h"

namespace Menge
{
    static bool s_ConstString_compare( PyObject * _obj, ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
    {
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
    std::string s_ConstString_repr( PyObject * _obj, ConstString * _cs )
    {
        return _cs->to_str();
    }

    class DefConsts
    {
    public:
        static PyObject * s_createConstString( const String & _str )
        {        
            TMapConstStrings::const_iterator it_found = s_constStrings.find( _str );

            if( it_found == s_constStrings.end() )
            {
                ConstString cs_str(_str);
                PyObject * py_cs = pybind::ptr( cs_str );

                DefObject df;
                df.cs_ref = cs_str;
                df.py_obj = py_cs;

                it_found = s_constStrings.insert( std::make_pair(_str, df) ).first;
            }

            const DefObject & df = it_found->second;       

            PyObject * py_cs = df.py_obj;
            pybind::incref( py_cs );
            
            return py_cs;
        }

    protected:
        struct DefObject
        {
            ConstString cs_ref;
            PyObject * py_obj;
        };

        typedef std::map<String, DefObject> TMapConstStrings;
        static TMapConstStrings s_constStrings;
    };

    DefConsts::TMapConstStrings DefConsts::s_constStrings;
    
	void ScriptWrapper::constsWrap()
	{
        pybind::struct_<ConstString>("ConstString")
            .def_compare( &s_ConstString_compare )
            //.def_convert( &ScriptMethod::vec2f_convert )
            .def_repr( &s_ConstString_repr )
            ;
        	
        pybind::def_function( "createConstString", &DefConsts::s_createConstString );

        PyObject * module = pybind::get_currentmodule();

        pybind::interface_<DefConsts>("DefConsts")
            ;

        DefConsts * dc = new DefConsts;

        PyObject * py_dc = pybind::ptr(dc);
                
        pybind::set_attr( module, "Consts", py_dc );

        int i = sizeof(ConstString);

#   define DEF_CONST_STRING( S )\
        PyObject * py_cs_##S = pybind::ptr( CONST_STRING( S ) );\
        pybind::set_attr( py_dc, #S, py_cs_##S );

        DEF_CONST_STRING( Main );
        DEF_CONST_STRING( Scene );
        DEF_CONST_STRING( Node );
        DEF_CONST_STRING( Layer2D );
        DEF_CONST_STRING( TextField );
        DEF_CONST_STRING( Sprite );
        DEF_CONST_STRING( ParticleEmitter );
        DEF_CONST_STRING( HotSpot );
        DEF_CONST_STRING( Movie );
        DEF_CONST_STRING( Point );
        DEF_CONST_STRING( Video );
        DEF_CONST_STRING( Window );
        DEF_CONST_STRING( HotSpotImage );
        DEF_CONST_STRING( WhitePixel );

#   undef DEF_CONST_STRING
	}
}

