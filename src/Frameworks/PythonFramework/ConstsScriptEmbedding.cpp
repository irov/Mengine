#include "ConstsScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool ConstString_compare( pybind::kernel_interface * _kernel, PyObject * _obj, ConstString * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
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
        static const Char * ConstString_repr( ConstString * _cs )
        {
            const Char * str_repr = _cs->c_str();

            return str_repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static ConstString::hash_type ConstString_hash( ConstString * _cs )
        {
            ConstString::hash_type hash = _cs->hash();

            return hash;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool ConstString_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * _user )
        {
            MENGINE_UNUSED( _kernel );
            MENGINE_UNUSED( _user );

            ConstString * cstr = (ConstString *)_place;

            if( _kernel->string_check( _obj ) == true )
            {
                SCRIPT_SERVICE()
                    ->stringize( _obj, cstr );

                return true;
            }
            else if( _kernel->is_none( _obj ) == true )
            {
                *cstr = ConstString::none();

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool FilePath_compare( pybind::kernel_interface * _kernel, PyObject * _obj, FilePath * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool & _result )
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
        static const Char * FilePath_repr( FilePath * _cs )
        {
            const Char * str_repr = _cs->c_str();

            return str_repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static ConstString::hash_type FilePath_hash( FilePath * _cs )
        {
            ConstString::hash_type hash = _cs->hash();

            return hash;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool FilePath_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * _user )
        {
            MENGINE_UNUSED( _kernel );
            MENGINE_UNUSED( _user );

            FilePath * cstr = (FilePath *)_place;

            if( _kernel->string_check( _obj ) == true )
            {
                SCRIPT_SERVICE()
                    ->stringize( _obj, cstr );

                return true;
            }
            else if( _kernel->is_none( _obj ) == true )
            {
                *cstr = FilePath::none();

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
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
    bool ConstsScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::structhash_<ConstString>( _kernel, "ConstString" )
            .def_compare( &Detail::ConstString_compare )
            .def_convert( &Detail::ConstString_convert, nullptr )
            .def_repr( &Detail::ConstString_repr )
            .def_hash( &Detail::ConstString_hash )
            ;

        pybind::structhash_<FilePath>( _kernel, "FilePath" )
            .def_compare( &Detail::FilePath_compare )
            .def_convert( &Detail::FilePath_convert, nullptr )
            .def_repr( &Detail::FilePath_repr )
            .def_hash( &Detail::FilePath_hash )
            ;

        pybind::registration_stl_vector_type_cast<VectorConstString>(_kernel);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstsScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<VectorConstString>(_kernel);

        _kernel->remove_scope<ConstString>();
        _kernel->remove_scope<FilePath>();
    }
    //////////////////////////////////////////////////////////////////////////
}

