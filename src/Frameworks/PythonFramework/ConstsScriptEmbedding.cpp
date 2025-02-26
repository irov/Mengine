#include "ConstsScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/StringView.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool StringView_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * const _place )
        {
            StringView * sv = (StringView *)_place;

            if( _kernel->string_check( _obj ) == true )
            {
                size_t size;
                const Char * str = _kernel->string_to_char_and_size( _obj, &size );

                sv->setup( str, size );

                return true;
            }

            return false;
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
        static bool ConstString_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * const _place )
        {
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
        static bool FilePath_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * const _place )
        {
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
        pybind::structhash_<StringView>( _kernel, "StringView" )
            .def_convert( &Detail::StringView_convert )
            ;

        pybind::structhash_<ConstString>( _kernel, "ConstString" )
            .def_compare_default()
            .def_convert( &Detail::ConstString_convert )
            .def_repr( &Detail::ConstString_repr )
            .def_hash( &Detail::ConstString_hash )
            ;

        pybind::structhash_<FilePath>( _kernel, "FilePath" )
            .def_compare_default()
            .def_convert( &Detail::FilePath_convert )
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

