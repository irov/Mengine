#	include "MoviePlugin.h"

#	include "Interface/PrototypeManagerInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/ConfigInterface.h"

#   include "Kernel/ScriptWrapper.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"

#   include "PythonScriptWrapper/PythonAnimatableEventReceiver.h"

#	include "Core/ModuleFactory.h"

#	include "Logger/Logger.h"

#   include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Menge::MoviePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void * stdex_movie_alloc( void * _data, size_t _size )
	{
		(void)_data;

		return stdex_malloc( _size );
		//return new uint8_t[_size];
		//return malloc( _size );
	}
	//////////////////////////////////////////////////////////////////////////
	static void * stdex_movie_alloc_n( void * _data, size_t _size, size_t _count )
	{
		(void)_data;

		size_t total = _size * _count;

		return stdex_malloc( total );
		//return new uint8_t[total];
		//return malloc( total );
	}
	//////////////////////////////////////////////////////////////////////////
	static void stdex_movie_free( void * _data, const void * _ptr )
	{
		(void)_data;

		stdex_free( (void *)_ptr );
		//delete[] _ptr;
		//free( (void *)_ptr );
	}
	//////////////////////////////////////////////////////////////////////////
	static void stdex_movie_free_n( void * _data, const void * _ptr )
	{
		(void)_data;

		stdex_free( (void *)_ptr );
		//delete[] _ptr;
		//free( (void *)_ptr );
	}
	//////////////////////////////////////////////////////////////////////////
	static void stdex_movie_logerror( void * _data, aeMovieErrorCode _code, const char * _format, ... )
	{
        switch( _code )
        {
        case AE_ERROR_STREAM:
            {
                return;
            }break;
        default:
            {
            }break;
        }

		va_list argList;

		va_start( argList, _format );
		char msg[4096];
		vsprintf( msg, _format, argList );
		va_end( argList );

		MoviePlugin * plugin = static_cast<MoviePlugin *>(_data);
		ServiceProviderInterface * serviceProvider = plugin->getServiceProvider();

		LOGGER_ERROR( serviceProvider )("MoviePlugin error '%s' code '%d'"
			, msg
			, _code			
			);
	}
	//////////////////////////////////////////////////////////////////////////
	MoviePlugin::MoviePlugin()
		: m_instance( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{		
		class ResourceMovie2PrototypeGenerator
			: public ResourcePrototypeGenerator<ResourceMovie2, 128>
		{
		public:
			ResourceMovie2PrototypeGenerator( aeMovieInstance * _instance )
				: m_instance( _instance )
			{
			}

            ~ResourceMovie2PrototypeGenerator()
            {
            }

		protected:
            PointerFactorable generate() override
            {
                ResourceMovie2 * resource = m_factory->createObject();

                if( resource == nullptr )
                {
                    LOGGER_ERROR( m_serviceProvider )("ResourcePrototypeGenerator can't generate %s %s"
                        , m_category.c_str()
                        , m_prototype.c_str()
                        );

                    return nullptr;
                }

                resource->setServiceProvider( m_serviceProvider );
                resource->setType( m_prototype );
                resource->setScriptWrapper( m_scriptWrapper );

                resource->setMovieInstance( m_instance );

                return resource;
            }

		protected:
			aeMovieInstance * m_instance;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool MoviePlugin::_avaliable()
	{
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    class PythonMovie2EventReceiver
        : public PythonAnimatableEventReceiver<Movie2EventReceiver>
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    PyObject * MoviePlugin::Movie2_setEventListener( pybind::kernel_interface * _kernel, Movie2 * _node, PyObject * _args, PyObject * _kwds )
    {
        (void)_args;

        if( _kwds == nullptr )
        {
            return pybind::ret_none();
        }

        pybind::dict py_kwds( _kernel, _kwds );
        Helper::registerAnimatableEventReceiver<PythonMovie2EventReceiver>( py_kwds, _node );

#	ifdef _DEBUG
        if( py_kwds.empty() == false )
        {
            for( pybind::dict::iterator
                it = py_kwds.begin(),
                it_end = py_kwds.end();
                it != it_end;
                ++it )
            {
                std::string k = it.key();

                LOGGER_ERROR( m_serviceProvider )("Movie2::setEventListener invalid kwds '%s'\n"
                    , k.c_str()
                    );
            }

            throw;
        }
#	endif

        return pybind::ret_none();
    }
	//////////////////////////////////////////////////////////////////////////
	bool MoviePlugin::_initialize()
	{
        String hashkey = CONFIG_VALUE( m_serviceProvider, "MoviePlugin", "HASHKEY", "" );

        if( hashkey.empty() == true )
        {
            LOGGER_ERROR( m_serviceProvider )("MoviePlugin::_initialize not setup HASHKEY");

            return false;
        }

        if( hashkey.size() != 40 )
        {
            LOGGER_ERROR( m_serviceProvider )("MoviePlugin::_initialize invalid HASHKEY '%s'"
                , hashkey.c_str()
                );

            return false;
        }

        m_hashkey = hashkey;

		pybind::kernel_interface * kernel = pybind::get_kernel();
		
		m_instance = ae_create_movie_instance( m_hashkey.c_str(), &stdex_movie_alloc, &stdex_movie_alloc_n, &stdex_movie_free, &stdex_movie_free_n, 0, &stdex_movie_logerror, this );

		pybind::interface_<Movie2, pybind::bases<Node, Animatable> >( kernel, "Movie2", false )
			.def( "setResourceMovie2", &Movie2::setResourceMovie2 )
			.def( "getResourceMovie2", &Movie2::getResourceMovie2 )
			.def( "setCompositionName", &Movie2::setCompositionName )
			.def( "getCompositionName", &Movie2::getCompositionName )
			.def( "getDuration", &Movie2::getDuration )
			.def( "setWorkAreaFromEvent", &Movie2::setWorkAreaFromEvent )
			.def( "removeWorkArea", &Movie2::removeWorkArea )
			.def( "playSubComposition", &Movie2::playSubComposition )
			.def( "stopSubComposition", &Movie2::stopSubComposition )
            .def_proxy_native_kernel( "setEventListener", this, &MoviePlugin::Movie2_setEventListener )
            .def( "getSlot", &Movie2::getSlot )
            .def( "hasSlot", &Movie2::hasSlot )
            .def( "setEnableMovieLayers", &Movie2::setEnableMovieLayers )
			;

        
        pybind::interface_<Movie2Slot, pybind::bases<Node> >( kernel, "Movie2Slot", false )
            .def( "getMovieName", &Movie2Slot::getMovieName )
            ;


        pybind::interface_<ResourceMovie2, pybind::bases<ResourceReference> >( kernel, "ResourceMovie2", false )
            .def( "hasComposition", &ResourceMovie2::hasComposition )
            .def( "getCompositionDuration", &ResourceMovie2::getCompositionDuration )
            .def( "getCompositionFrameDuration", &ResourceMovie2::getCompositionFrameDuration )
			;
		
		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2" ), new ScriptWrapper<Movie2>() );

        SCRIPT_SERVICE( m_serviceProvider )
            ->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2Slot" ), new ScriptWrapper<Movie2Slot>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceMovie2" ), new ScriptWrapper<ResourceMovie2>() );

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2" ), new NodePrototypeGenerator<Movie2, 128> ) == false )
		{
			return false;
		}

        if( PROTOTYPE_SERVICE( m_serviceProvider )
            ->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2Slot" ), new NodePrototypeGenerator<Movie2Slot, 128> ) == false )
        {
            return false;
        }

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Resource" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceMovie2" ), new ResourceMovie2PrototypeGenerator(m_instance) ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MoviePlugin::_finalize()
	{
		ae_delete_movie_instance( m_instance );
		m_instance = nullptr;

        SCRIPT_SERVICE( m_serviceProvider )
            ->removeWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2" ) );

        SCRIPT_SERVICE( m_serviceProvider )
            ->removeWrapper( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceMovie2" ) );

        PROTOTYPE_SERVICE( m_serviceProvider )
            ->removePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2" ) );

        PROTOTYPE_SERVICE( m_serviceProvider )
            ->removePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2Slot" ) );

        PROTOTYPE_SERVICE( m_serviceProvider )
            ->removePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Resource" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceMovie2" ) );
	}
}
