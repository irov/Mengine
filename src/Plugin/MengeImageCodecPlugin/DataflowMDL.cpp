#	include "DataflowMDL.h"

#	include "Interface/ArchiveInterface.h"
#	include "Interface/MemoryInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/Stream.h"

#	include "stdex/memory_reader.h"

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
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

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
		MemoryCacheBufferInterfacePtr binaryBuffer;
		if( Helper::loadStreamArchiveData( m_serviceProvider, _stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_MDL ), GET_MAGIC_VERSION( MAGIC_MDL ), binaryBuffer ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowMDL::load: invalid get data"
				);

			return false;
		}

		void * binaryBuffer_memory = binaryBuffer->getMemory();
		size_t binaryBuffer_size = binaryBuffer->getSize();

		size_t binary_read = 0;
		stdex::memory_reader ar( binaryBuffer_memory, binaryBuffer_size, binary_read );
				
		uint32_t frameCount;
		ar << frameCount;

		uint32_t vertexCount;
		ar << vertexCount;

		uint32_t indicesCount;
		ar << indicesCount;

		float frameDelay;
		ar << frameDelay;

		float cameraFOV;
		ar << cameraFOV;

		float cameraWidth;
		ar << cameraWidth;

		float cameraHeight;
		ar << cameraHeight;

		float cameraRightSign;
		ar << cameraRightSign;

		Model3DPack * pack = stdex::intrusive_get<Model3DPack>(_data);

		pack->initialize( frameCount, vertexCount, indicesCount, frameDelay );
		pack->setCamera( cameraFOV, cameraWidth / cameraHeight, cameraRightSign );
		
		for( uint32_t i = 0; i != frameCount; ++i )
		{
			Model3DFrame & frame = pack->mutableFrame( i );
						
			ar << frame.cameraPos;
			ar << frame.cameraDir;
			ar << frame.cameraUp;

			frame.pos = _data->allocateMemoryT<mt::vec3f>( vertexCount );
			frame.uv = _data->allocateMemoryT<mt::vec2f>( vertexCount );
			frame.indecies = _data->allocateMemoryT<RenderIndices>( indicesCount );

			ar.readPODs( frame.pos, vertexCount );
			ar.readPODs( frame.uv, vertexCount );
			ar.readPODs( frame.indecies, indicesCount );
		}

		return true;
	}
}