#include "JSONScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "ResourceJSON.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
                        d[key] = Helper::jpp2pybind( _kernel, value );
                    }

                    return d;
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
                    int32_t value = _obj;

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

            MENGINE_ERROR_FATAL( "invalid jpp type '%u'"
                , static_cast<std::underlying_type<jpp::e_type>::type>(type)
            );

            return pybind::make_invalid_object_t();
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::object s_getJSON( pybind::kernel_interface * _kernel, const ResourceJSON * _resource )
        {
            const jpp::object & j = _resource->getJSON();
            
            pybind::object ob = jpp2pybind( _kernel, j );

            return ob;
        }
        //////////////////////////////////////////////////////////////////////////
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
    bool JSONScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "JSON", true );

        pybind::interface_<ResourceJSON, pybind::bases<Resource>>( _kernel, "ResourceJSON", false )
            .def_static_kernel( "getJSON", &Helper::s_getJSON )
            ;

        Helper::registerScriptWrapping<ResourceJSON>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceJSON>();

        Helper::unregisterScriptWrapping<ResourceJSON>();
    }
    //////////////////////////////////////////////////////////////////////////
}
