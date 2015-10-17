#	include "MDLRender.h"

#	include "Image.h"

#	include <iterator>
#	include <algorithm>

#	include <d3dx9tex.h>

#	include <Windows.h>

#	include "stdex/memory_reader.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MDLRender::MDLRender()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool MDLRender::initialize()
	{
		Render::initialize();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MDLRender::finalize()
	{		
		Render::finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	void MDLRender::releaseResource_()
	{
		Render::releaseResource_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool MDLRender::resetDevice_()
	{	
		Render::resetDevice_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Model * MDLRender::createModel( const std::wstring & _file )
	{
		FILE * f;
		errno_t err = _wfopen_s( &f, _file.c_str(), L"rb" );

		if( err != 0 )
		{
			//_logger.message("MDLRender::createModel invalid open file %ls"
			//	, _file.c_str()
			//	);

			return nullptr;
		}

		fseek( f, 0, SEEK_END );
		size_t size = ftell( f );
		rewind( f );

		if( size == 0 )
		{
			//_logger.message("MDLRender::createModel file %ls empty"
			//	, _file.c_str()
			//	);
			
			return nullptr;
		}

		std::vector<unsigned char> buffer(size);
		fread( &buffer[0], 1, size, f );
		fclose( f );

		Model * model = new Model( m_device );

		if( model->load( &buffer[0], size ) == false )
		{
			//_logger.message( "MDLRender::createModel invalid load model %ls"
			//	, _file.c_str()
			//	);

			return nullptr;
		}
		
		m_models.push_back( model );

		return model;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool MDLRender::destroyModel( Model * _model )
	{
		if( _model == nullptr )
		{
			return false;
		}

		TVectorModels::iterator it_erase = std::find( m_models.begin(), m_models.end(), _model );

		if( it_erase == m_models.end() )
		{
			//_logger.message( "MDLRender::destroyModel not found\n"
			//	);

			return false;
		}

		delete _model;

		m_models.erase( it_erase );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MDLRender::renderModel( Model * _model )
	{
		const Model3DFrame & frame = _model->getCurrentFrame();

		uint32_t vertex_count = _model->getVertexCount();
		uint32_t indices_count = _model->getIndicesCount();

		Vertex * verties;
		if( Render::lockVertexBuffer( vertex_count, &verties ) == false )
		{
			//_logger.message( "MDLRender::renderModel Lock vertex == NULL"
			//	);

			return false;
		}

		for( uint32_t i = 0; i != vertex_count; ++i )
		{
			Vertex & v = verties[i];

			v.pos = frame.pos[i];

			v.color = 0xFFFFFFFF;

			v.uv[0] = frame.uv[i];			
		}

		Render::unlockVertexBuffer();

		Indices * indices;
		if( Render::lockIndicesBuffer( indices_count, &indices ) == false )
		{
			//_logger.message( "MDLRender::renderModel Lock indices == NULL"
			//	);

			return false;
		}

		for( uint32_t i = 0; i != indices_count; ++i )
		{
			Indices & dst_index = indices[i];

			uint16_t src_index = frame.indecies[i];

			dst_index = (UINT)src_index;
		}

		Render::unlockIndicesBuffer();

		_model->beginRender();

		Render::drawIndexedPrimitive( 0, vertex_count, indices_count / 3 );

		return true;
	}
}