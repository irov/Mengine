#	include "DataflowMDL.h"

#	include "Interface/ArchiveInterface.h"

#	include "Archive/ArchiveRead.hpp"

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
	DataInterfacePtr DataflowMDL::load( const InputStreamInterfacePtr & _stream )
	{
		size_t magic_header;
		_stream->read( &magic_header, sizeof(magic_header) );

		if( magic_header != 0x3D3DBABE )
		{
			LOGGER_ERROR(m_serviceProvider)( "DataflowMDL::load: mdl invalid magic header"
				);

			return nullptr;
		}

		size_t version_valid = DATAFLOW_VERSION_MDL;

		size_t version;
		_stream->read( &version, sizeof(version) );

		if( version != version_valid )
		{
			LOGGER_ERROR(m_serviceProvider)( "DataflowMDL::load: mdl invalid version %d:%d"
				, version
				, version_valid
				);

			return nullptr;
		}

		size_t binary_size;
		_stream->read( &binary_size, sizeof(binary_size) );

		size_t compress_size;
		_stream->read( &compress_size, sizeof(compress_size) );

		static TBlobject compress;
		compress.resize(compress_size);

		_stream->read( &compress[0], compress_size );

		m_cacheBinary.clear();
		m_cacheBinary.resize(binary_size);

		TBlobject::value_type * cacheBinaryBuffer = &m_cacheBinary[0];
		TBlobject::value_type * compressBuffer = &compress[0];

		size_t uncompressSize = 0;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->uncompress( cacheBinaryBuffer, binary_size, uncompressSize, compressBuffer, compress_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "DataflowAEK::load: aek invalid uncompress"
				);

			return nullptr;
		}

		ArchiveRead ar(m_cacheBinary.begin(), m_cacheBinary.end());

		ar.begin();

		size_t frameCount;
		ar >> frameCount;

		size_t vertexCount;
		ar >> vertexCount;

		size_t indeciesCount;
		ar >> indeciesCount;

		float frameDelay;
		ar >> frameDelay;

		Model3DPack * pack = m_poolModel3DPack.createObjectT();

		pack->initialize( frameCount, vertexCount, indeciesCount, frameDelay );
		
		for( size_t i = 0; i != frameCount; ++i )
		{
			Model3DFrame & frame = pack->mutableFrame( i );

			ar >> frame.cameraPos;
			ar >> frame.cameraDir;

			ar.readPODs( frame.pos, vertexCount );
			ar.readPODs( frame.uv, vertexCount );

			ar.readPODs( frame.indecies, indeciesCount );
		}

		return pack;
	}
}