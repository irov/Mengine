#	include "Movie2.h"

#   include "Interface/TimelineInterface.h"

#	include "Kernel/Materialable.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Movie2::Movie2()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie2 )
	{
		if( m_resourceMovie2 == _resourceMovie2 )
		{
			return;
		}

		m_resourceMovie2 = _resourceMovie2;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceMovie2Ptr & Movie2::getResourceMovie2() const
	{
		return m_resourceMovie2;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::setCompositionName( const ConstString & _name )
	{
		m_compositionName = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Movie2::getCompositionName() const
	{
		return m_compositionName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_play( float _time )
	{
		(void)_time;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_restart( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_resume( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_stop( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_end( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_compile()
	{
		if( m_resourceMovie2 == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie2::_compile: '%s' can't setup resource"
				, this->getName().c_str()
				);

			return false;
		}

		if( m_resourceMovie2.compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie2::_compile '%s' resource %s not compile"
				, m_name.c_str() 
				, m_resourceMovie2->getName().c_str()
				);

			return false;
		}

		m_composition = m_resourceMovie2->createComposition( m_compositionName );

		if( m_composition == nullptr )
		{
			return false;
		}
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_release()
	{	
		m_resourceMovie2->destroyComposition( m_composition );

		m_resourceMovie2.release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		if( m_composition->composition_data->flags & AE_MOVIE_COMPOSITION_ANCHOR_POINT )
		{	
			mt::vec3f origin;

			const float * anchorPoint = m_composition->composition_data->anchorPoint;

			origin.from_f3( anchorPoint );

			this->setOrigin( origin );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_deactivate()
	{
		

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_afterActivate()
	{
		Node::_afterActivate();

		bool autoPlay = this->getAutoPlay();

		if( autoPlay == true )
		{
			float time = TIMELINE_SERVICE( m_serviceProvider )
				->getTime();

			if( this->play( time ) == 0 )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie2::_afterActivate '%s' resource '%s' auto play return 0"
					, this->getName().c_str()
					, this->m_resourceMovie2->getName().c_str()
					);

				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_update( float _current, float _timing )
	{
		(void)_current;
		(void)_timing;

		if( this->isPlay() == false )
		{
			return;
		}

		update_movie_composition( m_composition, _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_render( const RenderObjectState * _state )
	{
		aeMovieRenderContext context;
		begin_movie_render_context( m_composition, &context );

		m_meshes.reserve( context.mesh_count );

		for( uint32_t i = 0; i != context.mesh_count; ++i )
		{
			aeMovieRenderMesh mesh;
			compute_movie_mesh( &context, i, &mesh );

			m_meshes.push_back( Mesh() );
			Mesh & m = m_meshes.back();

			m.vertices.resize( mesh.vertexCount );
		
			for( uint32_t index = 0; index != mesh.vertexCount; ++index )
			{
				RenderVertex2D & v = m.vertices[index];

				v.position.from_f3( mesh.position + index * 3 );

				for( uint32_t index_uv = 0; index_uv != MENGINE_RENDER_VERTEX_UV_COUNT; ++index_uv )
				{
					v.uv[index_uv].from_f2( mesh.uv + index * 2 );
				}

				v.color = Helper::makeARGB( mesh.r, mesh.g, mesh.b, mesh.a );
			}

			m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

			ResourceReference * resource_reference = (ResourceReference *)mesh.resource_data;

			switch( mesh.resource_type )
			{
			case AE_MOVIE_RESOURCE_IMAGE:
				{
					ResourceImage * resource_image = static_cast<ResourceImage *>(resource_reference);

					m.material = Helper::makeImageMaterial( m_serviceProvider, resource_image, ConstString::none(), EMB_NORMAL, false, false );

					RENDER_SERVICE( m_serviceProvider )
						->addRenderObject( _state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

		return true;
	}
}