/*
 *	Mesh_40_30.cpp
 *
 *	Created by _Berserk_ on 21.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Mesh_40_30.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"
#	include "ResourceManager.h"
#	include "RenderEngine.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Mesh_40_30);
	//////////////////////////////////////////////////////////////////////////
	Mesh_40_30::Mesh_40_30()
		: m_width( 40 )
		, m_height( 30 )
		, m_material( NULL )
		, m_resourceImage( NULL )
		, m_amplitude( 2.0f )
		, m_freq( 0.001f )
		, m_timing(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Mesh_40_30::~Mesh_40_30()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh_40_30::loader( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourceName );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Amplitude", "Value", &Mesh_40_30::setAmplitude );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Frequency", "Value", &Mesh_40_30::setFrequency );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Mesh_40_30::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		if( m_resourceName.empty() == true )
		{
			MENGE_LOG_ERROR( "Warning: (Mesh_40_30::_compile) empty resource" );
			return false;
		}

		m_material = Holder<RenderEngine>::hostage()
						->createMaterial();

		//m_material->textureStages = 1;
		m_material->textureStage[0].colorOp = TOP_MODULATE;

		m_resourceImage = Holder<ResourceManager>::hostage()
							->getResourceT<ResourceImage>( m_resourceName );

		if( m_resourceImage == NULL )
		{
			MENGE_LOG_ERROR( "Warning: resource image not found %s"
				, m_resourceName.c_str()
				);

			return false;
		}

		//m_material->textureStage[0].texture = m_resourceImage->getTexture( 0 );

		m_vertices.resize( m_width * m_height );
		m_mesh.resize( m_width * m_height );
		mt::vec3f tVertex;
		for( size_t j = 0; j < m_height; ++j )
		{
			for( size_t i = 0; i < m_width; ++i )
			{
				Vertex2D& vtx = m_vertices[j*m_width+i];
				mt::vec3f& mvtx = m_mesh[j*m_width+i];
				tVertex.x = 1024.0f / (m_width-1) * static_cast<float>(i);
				tVertex.y = 768.0f / (m_height-1) * static_cast<float>(j);
				tVertex.z = 0.0f;
				//mt::mul_v3_m4( mvtx, tVertex,  proj3D);
				mvtx = tVertex;
				//mt::mul_v3_m4
				vtx.pos[0] = tVertex.x;
				vtx.pos[1] = tVertex.y;
				vtx.pos[2] = 0.0f;
				vtx.color = 0xFFFFFFFF;
				vtx.uv[0] = static_cast<float>( i ) / ( m_width - 1 );
				vtx.uv[1] = static_cast<float>( j ) / ( m_height - 1 );
			} 
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh_40_30::_release()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceImage );
		m_resourceImage = NULL;

		Holder<RenderEngine>::hostage()
			->releaseMaterial( m_material );
		m_material = NULL;

		m_vertices.clear();

		Node::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh_40_30::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );
		Texture* texture = m_resourceImage->getTexture( 0 );
		Holder<RenderEngine>::hostage()
			->renderObject2D( m_material, &texture, 1, &(m_vertices[0]), m_width * m_height, LPT_MESH_40_30 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh_40_30::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

		const mt::mat3f& wm = getWorldMatrix();
		for( size_t j = 0; j < m_height; ++j )
		{
			for( size_t i = 0; i < m_width; ++i )
			{
				Vertex2D& vtx = m_vertices[j*m_width+i];
				mt::vec2f pt( vtx.pos[0], vtx.pos[1] );
				mt::vec2f out;
				mt::mul_v2_m3( out, pt, wm );
				mt::add_internal_point( _boundingBox, out );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh_40_30::_update( float _timing )
	{
		if( m_hide == true )
		{
			return;
		}

		m_timing += _timing;
		mt::vec3f resVertex;
		for( size_t i = 1; i < m_width-1; i++ )
		{
			float kx = 1.0f - ::fabsf( static_cast<float>( i ) / (m_width-1) - 0.5f ) * 2.0f;
			for( size_t j = 1; j < m_height-1; j++ )
			{
				//float noise = perlinNoise_( i * 0.5, j * 0.5, _timing ) * 20.0f;
				//m_vertices[i*m_width + j].pos[2] = -noise;
				//float ky = 1.0f - ::fabsf( static_cast<float>( j ) / (m_height-1) - 0.5f ) * 2.0f;
				float z = ::sinf( i + m_timing * m_freq ) * kx + ::cosf( j + m_timing * m_freq );// * ky;
				mt::vec3f& mvtx = m_mesh[j*m_width + i];
				Vertex2D& vtx = m_vertices[j*m_width + i];
				//mvtx.z = z * 1000.3f;
				//mt::mul_v3_m4( resVertex, mvtx, m_transformMatrix );
				vtx.pos[0] = mvtx.x + z * m_amplitude;//resVertex.x  - 512.0f;
				//vtx.pos[1] = //resVertex.y  + 384.0f;
				vtx.pos[2] = 0.0f;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh_40_30::setAmplitude( float _amplitude )
	{
		m_amplitude = _amplitude;
	}
	//////////////////////////////////////////////////////////////////////////
	void Mesh_40_30::setFrequency( float _frequency )
	{
		m_freq = _frequency;
	}
	//////////////////////////////////////////////////////////////////////////
	float Mesh_40_30::getAmplitude() const
	{
		return m_amplitude;
	}
	//////////////////////////////////////////////////////////////////////////
	float Mesh_40_30::getFrequency() const
	{
		return m_freq;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
