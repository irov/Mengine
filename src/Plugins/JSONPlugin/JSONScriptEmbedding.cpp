#include "JSONScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "ResourceJSON.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static pybind::object jpp2pybind( pybind::kernel_interface * _kernel, const jpp::object & _obj )
        {
            jpp::e_type type = _obj.type();

            switch( type )
            {
            case jpp::e_type::JPP_OBJECT:
                {
                    pybind::dict d( _kernel );
                    
                    for( auto && [key, value] : _obj )
                    {
                        d[key] = jpp2pybind( _kernel, value );
                    }

                    pybind::object ob = pybind::dictobject_new( _kernel, d );

                    return ob;
                }break;
            case jpp::e_type::JPP_ARRAY:
                {
                    pybind::list a( _kernel );
                    
                    for( const jpp::object & value : jpp::array( _obj ) )
                    {
                        a += jpp2pybind( _kernel, value );                        
                    }

                    return a;
                }break;
            case jpp::e_type::JPP_STRING:
                {
                    const char * value = _obj;
                    
                    return pybind::make_object_t( _kernel, value );
                }break;
            case jpp::e_type::JPP_INTEGER:
                {
                    int value = _obj;

                    return pybind::make_object_t( _kernel, value );
                }break;
            case jpp::e_type::JPP_REAL:
                {
                    float value = _obj;
                    
                    return pybind::make_object_t( _kernel, value );
                }break;
            case jpp::e_type::JPP_TRUE:
                {
                    return pybind::make_true_t( _kernel );
                }break;
            case jpp::e_type::JPP_FALSE:
                {
                    return pybind::make_false_t( _kernel );
                }break;
            case jpp::e_type::JPP_NULL:
                {
                    return pybind::make_none_t( _kernel );
                }break;
            }

            ERROR_FATAL( "invalid jpp type '%d'"
                , type
            );

            return pybind::make_invalid_object_t();
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::object s_getJSON( pybind::kernel_interface * _kernel, ResourceJSON * _resource )
        {
            const JSONStorageInterfacePtr & storage = _resource->getJSONStorage();

            const jpp::object & root = storage->getJSON();
            
            pybind::object ob = jpp2pybind( _kernel, root );

            return ob;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JSONScriptEmbedding::JSONScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONScriptEmbedding::~JSONScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceJSON, pybind::bases<Resource>>( _kernel, "ResourceJSON", false )
            .def_static_kernel( "getJSON", &Helper::s_getJSON )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceJSON> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceJSON>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceJSON" ) );
    }
}
