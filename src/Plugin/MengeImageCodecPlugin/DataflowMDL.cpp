#	include "DataflowMDL.h"

#	include "Interface/ArchiveInterface.h"
#	include "Interface/CacheInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/BlobjectRead.h"
#	include "Core/CacheMemoryBuffer.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DataflowMDL::DataflowMDL()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataflowMDL::~DataflowMDL()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void DataflowMDL::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * DataflowMDL::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DataflowMDL::initialize()
	{
		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	DataInterfacePtr DataflowMDL::create()
	{
		Model3DPack * pack = m_poolModel3DPack.createObjectT();

		return pack;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DataflowMDL::load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream )
	{
		unsigned char * binary_memory;
		size_t binary_size;

		uint32_t bufferId = CACHE_SERVICE(m_serviceProvider)
			->getArchiveData( _stream, m_archivator, GET_MAGIC_NUMBER(MAGIC_MDL), GET_MAGIC_VERSION(MAGIC_MDL), &binary_memory, binary_size );

		if( bufferId == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowMDL::load: invalid get data"
				);

			return false;
		}

		BlobjectRead ar(binary_memory, binary_size);
		
		uint32_t frameCount;
		ar >> frameCount;

		uint32_t vertexCount;
		ar >> vertexCount;

		uint32_t indicesCount;
		ar >> indicesCount;

		float frameDelay;
		ar >> frameDelay;

		float cameraFOV;
		ar >> cameraFOV;

		float cameraWidth;
		ar >> cameraWidth;

		float cameraHeight;
		ar >> cameraHeight;

		float cameraRightSign;
		ar >> cameraRightSign;

		if( vertexCount > MENGINE_MODEL_MAX_VERTEX ||
			indicesCount > MENGINE_MODEL_MAX_INDICES )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowMDL::load: mdl overflow vertex %d:%d indices %d:%d"
				, vertexCount
				, MENGINE_MODEL_MAX_VERTEX
				, indicesCount
				, MENGINE_MODEL_MAX_INDICES
				);

			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( bufferId );

			return false;
		}

		Model3DPack * pack = stdex::intrusive_get<Model3DPack>(_data);

		pack->initialize( frameCount, vertexCount, indicesCount, frameDelay );
		pack->setCamera( cameraFOV, cameraWidth / cameraHeight, cameraRightSign );
		
		for( size_t i = 0; i != frameCount; ++i )
		{
			Model3DFrame & frame = pack->mutableFrame( i );
						
			ar >> frame.cameraPos;
			ar >> frame.cameraDir;	
			ar >> frame.cameraUp;

			ar.readPODs( frame.pos, vertexCount );
			ar.readPODs( frame.uv, vertexCount );

			ar.readPODs( frame.indecies, indicesCount );
		}

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( bufferId );

		return true;
	}
}