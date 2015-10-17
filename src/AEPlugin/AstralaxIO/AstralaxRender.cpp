#	include "AstralaxRender.h"

#	include "Image.h"

#	include <iterator>
#	include <algorithm>

#	include <d3dx9tex.h>

#	include <Windows.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define DXCALL
	//////////////////////////////////////////////////////////////////////////
	AstralaxRender::AstralaxRender()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxRender::initialize()
	{
		Render::initialize();
		
		bool states[17];
		states[MAGIC_RENDER_STATE_BLENDING] = false;
		states[MAGIC_RENDER_STATE_TEXTURE_COUNT] = false;
		states[MAGIC_RENDER_STATE_TEXTURE] = true;
		states[MAGIC_RENDER_STATE_ADDRESS_U] = false;
		states[MAGIC_RENDER_STATE_ADDRESS_V] = false;
		states[MAGIC_RENDER_STATE_OPERATION_RGB] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT1_RGB] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT2_RGB] = false;
		states[MAGIC_RENDER_STATE_OPERATION_ALPHA] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT1_ALPHA] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT2_ALPHA] = false;
		states[MAGIC_RENDER_STATE_ZENABLE] = false;
		states[MAGIC_RENDER_STATE_ZWRITE] = false;
		states[MAGIC_RENDER_STATE_ALPHATEST_INIT] = false;
		states[MAGIC_RENDER_STATE_ALPHATEST] = false;
		states[MAGIC_RENDER_STATE_TECHNIQUE_ON] = false;
		states[MAGIC_RENDER_STATE_TECHNIQUE_OFF] = false;

		Magic_SetRenderStateFilter( states, false );

		Magic_SetAxis( MAGIC_pXpYpZ );

		Magic_SetStartingScaleForAtlas( 1.f );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxRender::finalize()
	{		
		Render::finalize();

		Magic_DestroyAll();

		for( TVectorAtlas::iterator
			it = m_atlas.begin(),
			it_end = m_atlas.end();
		it != it_end;
		++it )
		{
			Atlas * atlas = *it;

			delete atlas;
		}

		m_atlas.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxRender::releaseResource_()
	{
		Render::releaseResource_();

		for( TVectorAtlas::iterator
			it = m_atlas.begin(),
			it_end = m_atlas.end();
		it != it_end;
		++it )
		{
			Atlas * atlas = *it;

			atlas->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxRender::resetDevice_()
	{	
		Render::resetDevice_();

		for( TVectorAtlas::iterator
			it = m_atlas.begin(),
			it_end = m_atlas.end();
		it != it_end;
		++it )
		{
			Atlas * atlas = *it;

			if( atlas->initialize() == false )
			{
				return false;
			}

			if( atlas->restore() == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxRender::setAtlas_( int _stage, int _texture )
	{ 
		Atlas * atlas = this->getAtlas_( _texture );

		if( atlas == nullptr )
		{
			return false;
		}

		atlas->beginRender( _stage );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Atlas * AstralaxRender::getAtlas_( int _id )
	{
		if( _id >= m_atlas.size() )
		{
			//_logger.message( "Render::getAtlas invalid find atlas %d"
			//	, _id
			//	);

			return nullptr;
		}

		Atlas * atlas = m_atlas[_id];

		return atlas;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxRender::updateEmittersAtlas( )
	{
		MAGIC_CHANGE_ATLAS c;
		while( Magic_GetNextAtlasChange( &c ) == MAGIC_SUCCESS )
		{
			switch( c.type )
			{
			case MAGIC_CHANGE_ATLAS_LOAD:
				{
					Atlas * atlas = this->getAtlas_( c.index );

					if( atlas == nullptr )
					{
						//_logger.message("updateEmittersAtlas_ MAGIC_CHANGE_ATLAS_LOAD not found atlas %d"
						//	, c.index
						//	);

						return false;
					}

					if( atlas->loadTexture( c ) == false )
					{
						//_logger.message("updateEmittersAtlas_ MAGIC_CHANGE_ATLAS_LOAD invalid load texture %d"
						//	, c.index
						//	);

						return false;
					}
				}break;
			case MAGIC_CHANGE_ATLAS_CLEAN:
				{
					Atlas * atlas = this->getAtlas_( c.index );

					if( atlas == nullptr )
					{
						//_logger.message("updateEmittersAtlas_ MAGIC_CHANGE_ATLAS_CLEAN not found atlas %d"
						//	, c.index
						//	);

						return false;
					}

					if( atlas->clearTexture( c ) == false )
					{
						//_logger.message("updateEmittersAtlas_ MAGIC_CHANGE_ATLAS_CLEAN invalid clear texture %d"
						//	, c.index
						//	);

						return false;
					}
				}break;
			case MAGIC_CHANGE_ATLAS_CREATE:
				{
					if( c.index != m_atlas.size() )
					{
						//_logger.message("updateEmittersAtlas_ MAGIC_CHANGE_ATLAS_CREATE alredy exist atlas %d"
						//	, c.index
						//	);

						return false;
					}

					Atlas * atlas = new Atlas(m_device, c.width, c.height);

					if( atlas->initialize() == false )
					{
						//_logger.message("updateEmittersAtlas_ MAGIC_CHANGE_ATLAS_CREATE invalid initialize atlas %d"
						//	, c.index
						//	);

						return false;
					}

					m_atlas.push_back( atlas );
				}break;
			case MAGIC_CHANGE_ATLAS_DELETE:
				{
					Atlas * atlas = this->getAtlas_( c.index );

					if( atlas == nullptr )
					{
						//_logger.message("updateEmittersAtlas_ MAGIC_CHANGE_ATLAS_DELETE not found atlas %d"
						//	, c.index
						//	);

						return false;
					}

					atlas->finalize();

					delete atlas;

					TVectorAtlas::iterator it_remove = m_atlas.begin();
					std::advance( it_remove, c.index );
					m_atlas.erase( it_remove );
				}break;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Emitter * AstralaxRender::createEmitter( const std::wstring & _ptc )
	{
		FILE * f;
		errno_t err = _wfopen_s( &f, _ptc.c_str(), L"rb" );

		if( err != 0 )
		{
			//_logger.message("AstralaxRender::createEmitter invalid open file %ls"
			//	, _ptc.c_str()
			//	);

			return nullptr;
		}

		fseek( f, 0, SEEK_END );
		size_t size = ftell( f );
		rewind( f );

		if( size == 0 )
		{
			//_logger.message("AstralaxRender::createEmitter file %ls empty"
			//	, _ptc.c_str()
			//	);
			
			return nullptr;
		}

		char * buffer_memory = new char[size];
		std::auto_ptr<char> buffer(buffer_memory);

		fread( buffer_memory, 1, size, f );
		fclose( f );
				
		HM_FILE mf = Magic_OpenFileInMemory( buffer_memory );

		if( mf == MAGIC_ERROR )
		{
			//_logger.message("AstralaxRender::createEmitter file %ls MAGIC_ERROR"
			//	, _ptc.c_str()
			//	);

			return nullptr;
		}

		if( mf == MAGIC_UNKNOWN )
		{
			//_logger.message("AstralaxRender::createEmitter file %ls MAGIC_UNKNOWN"
			//	, _ptc.c_str()
			//	);

			return nullptr;
		}

		Emitter * emitter = this->createEmitter_( mf, buffer );

		if( emitter == nullptr )
		{
			//_logger.message( "AstralaxRender::createEmitter %ls invalid create emitter"
			//	, _ptc.c_str()
			//	);

			Magic_CloseFile( mf );

			return nullptr;
		}		

		m_emitters.push_back( emitter );

		printf("AstralaxRender::createEmitter %ls:%s\n"
			, _ptc.c_str()
			, emitter->getName()
			);

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	Emitter * AstralaxRender::createEmitter_( HM_FILE _mf, std::auto_ptr<char> _buffer )
	{
		HM_EMITTER emitterId = this->createEmitterId_( _mf );

		if( emitterId == 0 )
		{
			return nullptr;
		}

		HM_EMITTER emitters[] = {emitterId};

		Magic_CreateAtlasesForEmitters( 2048, 2048, 1, emitters, 1, 0.01f );

		if( this->updateEmittersAtlas() == false )
		{
			return nullptr;
		}

		Emitter * emitter = new Emitter();

		if( emitter->initialize( _mf, _buffer, emitterId ) == false )
		{
			return nullptr;
		}

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxRender::createEmitterId_( HM_FILE _mf )
	{
		MAGIC_FIND_DATA find;
		const char* magicName = Magic_FindFirst( _mf, &find, MAGIC_FOLDER | MAGIC_EMITTER );

		while( magicName != nullptr )
		{
			if( find.animate == 1 )
			{
				HM_EMITTER id = Magic_LoadEmitter( _mf, magicName );

				if( id == 0 )
				{
					return 0;
				}

				return id;
			}
			else
			{		
				const char * currentFolder = Magic_GetCurrentFolder( _mf );
				Magic_SetCurrentFolder( _mf, magicName );

				HM_EMITTER id = this->createEmitterId_( _mf );

				Magic_SetCurrentFolder( _mf, currentFolder );

				if( id != 0 )
				{
					return id;
				}
			}

			magicName = Magic_FindNext( _mf, &find );
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxRender::destroyEmitter( Emitter * _emitter )
	{
		if( _emitter == nullptr )
		{
			return false;
		}

		TVectorEmitters::iterator it_erase = std::find( m_emitters.begin(), m_emitters.end(), _emitter );

		if( it_erase == m_emitters.end() )
		{
			//_logger.message("AstralaxRender::destroyEmitter %s %d not found\n"
			//	, _emitter->getName()
			//	, _emitter->getId()
			//	);

			return false;
		}

		printf("AstralaxRender::destroyEmitter %s %d\n"
			, _emitter->getName()
			, _emitter->getId()
			);

		_emitter->finalize();

		this->updateEmittersAtlas();

		delete _emitter;

		m_emitters.erase( it_erase );		
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxRender::renderEmitter( Emitter * _emitter, const ParticleEmitterRenderFlush & _flush )
	{	
		if( _flush.vertices.empty() == true )
		{
			return true;
		}

		UINT particle_vertex_count = (UINT)_flush.vertices.size();

		Vertex * verties;
		if( Render::lockVertexBuffer( particle_vertex_count, &verties ) == false )
		{
			//_logger.message( "AstralaxRender::renderEmitter Lock vertex == NULL"
			//	);

			return false;
		}

		stdext::unchecked_array_iterator<Vertex *> chkd_vertex_array( verties );
		std::copy( _flush.vertices.begin(), _flush.vertices.end(), chkd_vertex_array );

		Render::unlockVertexBuffer();

		UINT particle_indices_count = (UINT)_flush.indices.size();

		Indices * indices;
		if( Render::lockIndicesBuffer( particle_indices_count, &indices ) == false )
		{
			//_logger.message( "MDLRender::renderModel Lock indices == NULL"
			//	);

			return false;
		}

		stdext::unchecked_array_iterator<Indices *> chkd_indices_array( indices );
		std::copy( _flush.indices.begin(), _flush.indices.end(), chkd_indices_array );

		Render::unlockIndicesBuffer();

		mt::mat4f projection;
		mt::mat4f view;

		if( _emitter->getTransform( projection, view ) == false )
		{
			//_logger.message( "AstralaxRender::renderEmitter headerP %s emitter invalid beginRender"
			//	, _emitter->getName()
			//	);

			return false;
		}

		DXCALL m_device->SetTransform( D3DTS_PROJECTION, (D3DXMATRIX *)projection.buff());
		DXCALL m_device->SetTransform( D3DTS_VIEW, (D3DXMATRIX *)view.buff() );

		for( TVectorParticleMesh::const_iterator 
			it = _flush.meshes.begin(),
			it_end = _flush.meshes.end();
		it != it_end;
		++it )
		{
			const ParticleMesh & mesh = *it;

			MAGIC_MATERIAL m;
			if( Magic_GetMaterial( mesh.material, &m ) != MAGIC_SUCCESS )
			{
				//_logger.message( "AstralaxRender::renderEmitter headerP %s emitter invalid get material %d"
				//	, _emitter->getName()
				//	, mesh.material
				//	);

				return false;
			}

			switch( m.blending )
			{
			case MAGIC_BLENDING_NORMAL:
				{
					DXCALL m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					DXCALL m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					DXCALL m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				}break;
			case MAGIC_BLENDING_ADD:
				{
					DXCALL m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					DXCALL m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
					DXCALL m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				}break;
			case MAGIC_BLENDING_OPACITY:
				{
					DXCALL m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				}break;
			}

			for( int stage = 0; stage != 8; ++stage )
			{
				DXCALL m_device->SetTexture( stage, nullptr );
				DXCALL m_device->SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
				DXCALL m_device->SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			}
			
			for( int stage = 0; stage != m.textures; ++stage )
			{
				int texture = mesh.texture[stage];
								
				this->setAtlas_( stage, texture );

				MAGIC_TEXTURE_STATES & state = m.states[stage];

				DXCALL m_device->SetSamplerState( stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				DXCALL m_device->SetSamplerState( stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				DXCALL m_device->SetSamplerState( stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

				const int dx_address[] = {D3DTADDRESS_WRAP, D3DTADDRESS_MIRROR, D3DTADDRESS_CLAMP, D3DTADDRESS_BORDER};

				DXCALL m_device->SetSamplerState( stage, D3DSAMP_ADDRESSU, dx_address[state.address_u] );
				DXCALL m_device->SetSamplerState( stage, D3DSAMP_ADDRESSV, dx_address[state.address_v] );

				const int dx_operation[] = {D3DTOP_SELECTARG1, D3DTOP_ADD, D3DTOP_SUBTRACT, D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_MODULATE4X};
				const int dx_arg[] = {D3DTA_CURRENT, D3DTA_DIFFUSE, D3DTA_TEXTURE};
				
				DXCALL m_device->SetTextureStageState( stage, D3DTSS_COLOROP, dx_operation[state.operation_rgb] );
				DXCALL m_device->SetTextureStageState( stage, D3DTSS_COLORARG1, dx_arg[state.argument_rgb1] );
				DXCALL m_device->SetTextureStageState( stage, D3DTSS_COLORARG2, dx_arg[state.argument_rgb2] );

				DXCALL m_device->SetTextureStageState( stage, D3DTSS_ALPHAOP, dx_operation[state.operation_alpha] );
				DXCALL m_device->SetTextureStageState( stage, D3DTSS_ALPHAARG1, dx_arg[state.argument_alpha1] );
				DXCALL m_device->SetTextureStageState( stage, D3DTSS_ALPHAARG2, dx_arg[state.argument_alpha2] );		
			}
			
			Render::drawIndexedPrimitive( mesh.indexBegin, mesh.vertexCount, mesh.primCount );
		}

		return true;
	}
}