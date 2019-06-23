#include "ConstsScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "pybind/pybind.hpp"
#include "pybind/stl/stl_type_cast.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_ConstString_compare( pybind::kernel_interface * _kernel, PyObject * _obj, ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
    {
        MENGINE_UNUSED( _obj );

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
    static const Char * s_ConstString_repr( ConstString * _cs )
    {
        const Char * str_repr = _cs->c_str();

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
        MENGINE_UNUSED( _kernel );
        MENGINE_UNUSED( _user );

        ConstString & cstr = *(ConstString*)_place;

        if( _kernel->string_check( _obj ) == true )
        {
            if( SCRIPT_SERVICE()
                ->stringize( _obj, cstr ) == false )
            {
                return false;
            }

            return true;
        }
        else if( _kernel->is_none( _obj ) == true )
        {
            cstr = ConstString::none();

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_FilePath_compare( pybind::kernel_interface * _kernel, PyObject * _obj, FilePath * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
    {
        MENGINE_UNUSED( _obj );

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
    static const Char * s_FilePath_repr( FilePath * _cs )
    {
        const Char * str_repr = _cs->c_str();

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
        MENGINE_UNUSED( _kernel );
        MENGINE_UNUSED( _user );

        FilePath & cstr = *(FilePath*)_place;

        if( _kernel->string_check( _obj ) == true )
        {
            if( SCRIPT_SERVICE()
                ->stringize( _obj, cstr ) == false )
            {
                return false;
            }

            return true;
        }
        else if( _kernel->is_none( _obj ) == true )
        {
            cstr = FilePath( ConstString::none() );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstsScriptEmbedding::ConstsScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstsScriptEmbedding::~ConstsScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConstsScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::registration_stl_vector_type_cast<ConstString, VectorConstString>(_kernel);

        pybind::structhash_<ConstString>( _kernel, "ConstString" )
            .def_compare( &s_ConstString_compare )
            .def_convert( &ConstString_convert, nullptr )
            .def_repr( &s_ConstString_repr )
            .def_hash( &s_ConstString_hash )
            ;

        pybind::structhash_<FilePath>( _kernel, "FilePath" )
            .def_compare( &s_FilePath_compare )
            .def_convert( &FilePath_convert, nullptr )
            .def_repr( &s_FilePath_repr )
            .def_hash( &s_FilePath_hash )
            ;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstsScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<ConstString, VectorConstString>(_kernel);

        _kernel->remove_scope<ConstString>();
        _kernel->remove_scope<FilePath>();
    }
}

