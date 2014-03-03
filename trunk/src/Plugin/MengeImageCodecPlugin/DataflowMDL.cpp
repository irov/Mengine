#	include "DataflowMDL.h"

#	include "Interface/ArchiveInterface.h"
#	include "Interface/CacheInterface.h"

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
	DataInterfacePtr DataflowMDL::create()
	{
		Model3DPack * pack = m_poolModel3DPack.createObjectT();

		return pack;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DataflowMDL::load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream )
	{
		size_t magic_header;
		_stream->read( &magic_header, sizeof(magic_header) );

		if( magic_header != DATAFLOW_MAGIC_MDL )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowMDL::load: mdl invalid magic header"
				);

			return false;
		}

		size_t version_valid = DATAFLOW_VERSION_MDL;

		size_t version;
		_stream->read( &version, sizeof(version) );

		if( version != version_valid )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowMDL::load: mdl invalid version %d:%d"
				, version
				, version_valid
				);

			return false;
		}

		size_t binary_size;
		_stream->read( &binary_size, sizeof(binary_size) );

		size_t compress_size;
		_stream->read( &compress_size, sizeof(compress_size) );

		CacheMemoryBuffer compress_buffer(m_serviceProvider, compress_size);
		TBlobject::value_type * compress_memory = compress_buffer.getMemoryT<TBlobject::value_type>();

		_stream->read( compress_memory, compress_size );

		CacheMemoryBuffer binary_buffer(m_serviceProvider, binary_size);
		TBlobject::value_type * binary_memory = binary_buffer.getMemoryT<TBlobject::value_type>();
		
		size_t uncompressSize = 0;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->uncompress( binary_memory, binary_size, uncompressSize, compress_memory, compress_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowAEK::load: aek invalid uncompress"
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

		return true;
	}
}