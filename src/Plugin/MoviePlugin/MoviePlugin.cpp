#	include "MoviePlugin.h"

#	include "Interface/PrototypeManagerInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"

#	include "Movie2.h"
#	include "ResourceMovie2.h"

#	include "Core/ModuleFactory.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Menge::MoviePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void * stdex_movie_alloc( void * _data, uint32_t _size )
	{
		(void)_data;

		//return stdex_malloc( _size );
		return new uint8_t[_size];
	}
	//////////////////////////////////////////////////////////////////////////
	static void * stdex_movie_alloc_n( void * _data, uint32_t _size, uint32_t _count )
	{
		(void)_data;

		uint32_t total = _size * _count;

		//return stdex_malloc( total );
		return new uint8_t[total];
	}
	//////////////////////////////////////////////////////////////////////////
	static void stdex_movie_free( void * _data, const void * _ptr )
	{
		(void)_data;

		//stdex_free( (void *)_ptr );
		delete[] _ptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static void stdex_movie_free_n( void * _data, const void * _ptr )
	{
		(void)_data;

		//stdex_free( (void *)_ptr );
		delete[] _ptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static void stdex_movie_logerror( void * _data, aeMovieErrorCode _code, const char * _compositionName, const char * _layerName, const char * _message )
	{
		MoviePlugin * plugin = static_cast<MoviePlugin *>(_data);
		ServiceProviderInterface * serviceProvider = plugin->getServiceProvider();

		LOGGER_ERROR( serviceProvider )("MoviePlugin movie error code '%d' composition '%s' layer '%s' message '%s'"
			, _code
			, _compositionName
			, _layerName
			, _message
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

		protected:
			bool setup( ResourceMovie2 * _resource ) override
			{
				_resource->setMovieInstance( m_instance );

				return true;
			}

		protected:
			aeMovieInstance * m_instance;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool MoviePlugin::_initialize()
	{
		m_instance = ae_create_movie_instance( &stdex_movie_alloc, &stdex_movie_alloc_n, &stdex_movie_free, &stdex_movie_free_n, 0, &stdex_movie_logerror, this );

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie2" ), new NodePrototypeGenerator<Movie2, 128> ) == false )
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
	}
}
