#	include "Sprite.h" 

#	include "Layer2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "LogEngine.h"

#	include "math/box2.h"

#	include "pybind/pybind.hpp"

//#	 include "ResourceTexture.h"
#	include "RenderObject.h"
namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Sprite);
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
	: m_resource( 0 )
	, m_currentImageIndex( 0 )
	, m_centerAlign( false )
	, m_alignOffset( 0.f, 0.f )
	, m_percent( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
	, m_offset( 0.0f, 0.0f )
	, m_size( 0.0f, 0.0f )
	, m_flipX( false )
	, m_flipY( false )
	, m_blendSrc( BF_SOURCE_ALPHA )
	, m_blendDest( BF_ONE_MINUS_SOURCE_ALPHA )
	, m_invalidateVertices( true )
	, m_percentVisibilityToCb( NULL )
	, m_renderObject( NULL )
	, m_alphaImage( NULL )
	{ }
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourceName );
			XML_CASE_ATTRIBUTE_NODE( "ImageAlpha", "Name", m_alphaImageName );
			XML_CASE_ATTRIBUTE_NODE( "ImageIndex", "Value", m_currentImageIndex );
			XML_CASE_ATTRIBUTE_NODE( "CenterAlign", "Value", m_centerAlign );
			XML_CASE_ATTRIBUTE_NODE( "BlendSource", "Value", ((int&)m_blendSrc) );
			XML_CASE_ATTRIBUTE_NODE( "BlendDest", "Value", ((int&)m_blendDest) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		updateSprite_();

		//this->registerEventMethod("COLOR_END", "onColorEnd" );
		//this->registerEventMethod("COLOR_STOP", "onColorStop" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		if( m_resourceName.empty() )
		{
			return false;
		}

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourceName );

		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( "Image resource not found \"%s\""
				, m_resourceName.c_str() );
			return false;
		}

		m_renderObject = Holder<RenderEngine>::hostage()
							->createRenderObject();

		m_renderObject->material.primitiveType = PT_TRIANGLELIST;

		m_renderObject->vertices.resize( 4 );

		uint16 indicies[] = { 0, 3, 1, 1, 3, 2 };
		m_renderObject->material.indicies.assign( indicies, indicies + 6 );

		m_renderObject->material.textureStages = 1;

		if( m_alphaImageName.empty() == false )
		{
			m_alphaImage = Holder<ResourceManager>::hostage()
							->getResourceT<ResourceImage>( m_alphaImageName );
			if( m_alphaImage == NULL )
			{
				MENGE_LOG_ERROR( "Warning: (Sprite::_compile) can't get AlphaImage \"%s\"",
					m_alphaImageName.c_str() );
			}
			else
			{
				m_renderObject->material.textureStages = 2;
				m_renderObject->material.textureStage[1].texture = m_alphaImage->getImage( 0 );
				m_renderObject->material.textureStage[1].colorOp = TOP_SELECTARG1;
				m_renderObject->material.textureStage[1].colorArg1 = TARG_CURRENT;
				m_renderObject->material.textureStage[1].alphaOp = TOP_SELECTARG1;
				m_renderObject->material.textureStage[1].alphaArg1 = TARG_TEXTURE;
			}
		}
		
		updateSprite_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_release()
	{
		Node::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_alphaImage );

		m_alphaImage = 0;

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;

		Holder<RenderEngine>::hostage()
			->releaseRenderObject( m_renderObject );

		m_renderObject = NULL;

		m_resource = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::flip( bool _x )
	{
		if( _x )
		{
			m_flipX = !m_flipX;
		}
		else
		{
			m_flipY = !m_flipY;
		}

		updateSprite_();
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY )
	{
		//m_percent.v2_0 = _percentX;
		//m_percent.v2_1 = _percentY;
		m_percent = mt::vec4f( _percentX, _percentY );
		updateSprite_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageIndex( std::size_t _index )
	{
		m_currentImageIndex = _index;
		if( m_resource != NULL )
		{
			std::size_t max = m_resource->getCount();
			if( _index >= max )
			{
				MENGE_LOG_ERROR( "Warning: (Sprite::setImageIndex) index (%d) > image count", 
					_index );
				m_currentImageIndex = max - 1;
			}
		}
		updateSprite_();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Sprite::getImageIndex() const
	{
		return m_currentImageIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageResource( const String& _name )
	{
		m_currentImageIndex = 0;

		if( m_resourceName != _name )
		{
			m_resourceName = _name;

			recompile();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Sprite::getImageResource() const
	{
		return m_resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateSprite_()
	{
		if( m_resource == 0 ) 
		{
			return;
		}

		m_renderObject->material.textureStage[0].texture = m_resource->getImage( m_currentImageIndex );

		m_renderObject->material.blendSrc = m_blendSrc;
		m_renderObject->material.blendDst = m_blendDest;

		if( m_alphaImage )
		{
			updateDimensions_( m_alphaImage );
		}
		else
		{
			updateDimensions_( m_resource );
		}

		invalidateBoundingBox();
		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateDimensions_( ResourceImage * _resource )
	{
		m_size = _resource->getSize( m_currentImageIndex );

		// adjust texture visibility
		m_percent.x = ::floorf( m_percent.x * m_size.x + 0.5f ) / m_size.x;
		m_percent.y = ::floorf( m_percent.y * m_size.y + 0.5f ) / m_size.y;
		m_percent.z = ::floorf( m_percent.z * m_size.x + 0.5f ) / m_size.x;
		m_percent.w = ::floorf( m_percent.w * m_size.y + 0.5f ) / m_size.y;

		mt::vec2f  visOffset( m_size.x * m_percent.x, m_size.y * m_percent.y );

		m_size.x = m_size.x - m_size.x * ( m_percent.x + m_percent.z );
		m_size.y = m_size.y - m_size.y * ( m_percent.y + m_percent.w );

		if( m_centerAlign )
		{
			mt::vec2f size = _resource->getMaxSize( m_currentImageIndex );

			m_alignOffset = size * -0.5f;
			m_alignOffset.x = ::floorf( m_alignOffset.x + 0.5f );
			m_alignOffset.y = ::floorf( m_alignOffset.y + 0.5f );
		}

		mt::vec2f offset = _resource->getOffset( m_currentImageIndex );
		const mt::vec2f & maxSize = _resource->getMaxSize( m_currentImageIndex );
		const mt::vec2f & size = _resource->getSize( m_currentImageIndex );

		if( m_flipX )
		{
			offset.x = maxSize.x - ( size.x + offset.x );
		}

		if( m_flipY )
		{
			offset.y = maxSize.y - ( size.y + offset.y );
		}

		m_offset = offset + m_alignOffset + visOffset;

		m_uv = _resource->getUV( m_currentImageIndex );

		float uvX = m_uv.z - m_uv.x;
		float uvY = m_uv.w - m_uv.y;

		m_uv.x = m_uv.x + m_percent.x * uvX;
		m_uv.y = m_uv.y + m_percent.y * uvY;
		m_uv.z = m_uv.z - m_percent.z * uvX;
		m_uv.w = m_uv.w - m_percent.w * uvY;

		if( m_alphaImage )
		{
			const mt::vec2f& rgbSize = m_resource->getSize( m_currentImageIndex );
			if( rgbSize.x > size.x 
				|| rgbSize.y > size.y )
			{
				if( m_renderObject->material.textureStage[0].matrix == NULL )
				{
					m_renderObject->material.textureStage[0].matrix = new mt::mat4f();
				}
				mt::mat4f* texMat = m_renderObject->material.textureStage[0].matrix;
				mt::ident_m4( *texMat );
				texMat->v0.x = size.x / rgbSize.x;
				texMat->v1.y = size.y / rgbSize.y;
				
				texMat->v2.x = offset.x / maxSize.x;
				texMat->v2.y = offset.y / maxSize.y;
				
			}
		}

		if( m_flipX == true )
		{
			std::swap( m_uv.x, m_uv.z );
		}

		if( m_flipY == true )
		{
			std::swap( m_uv.y, m_uv.w );
		}

		m_renderObject->vertices[0].uv[0] = m_uv.x;
		m_renderObject->vertices[0].uv[1] = m_uv.y;
		m_renderObject->vertices[1].uv[0] = m_uv.z;
		m_renderObject->vertices[1].uv[1] = m_uv.y;
		m_renderObject->vertices[2].uv[0] = m_uv.z;
		m_renderObject->vertices[2].uv[1] = m_uv.w;
		m_renderObject->vertices[3].uv[0] = m_uv.x;
		m_renderObject->vertices[3].uv[1] = m_uv.w;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f * Sprite::getVertices()
	{
		if( m_invalidateVertices == true )
		{
			const mt::mat3f & wm = getWorldMatrix();

			//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
			//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
			//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
			//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
			mt::vec2f transformX;
			mt::vec2f transformY;

			mt::mul_v2_m3( m_vertices[0], m_offset, wm );
			mt::mul_v2_m3_r( transformX, mt::vec2f( m_size.x, 0.0f ), wm );
			mt::mul_v2_m3_r( transformY, mt::vec2f( 0.0f, m_size.y ), wm );

			m_vertices[1] = m_vertices[0] + transformX;
			m_vertices[2] = m_vertices[1] + transformY;
			m_vertices[3] = m_vertices[0] + transformY;

			for( int i = 0; i < 4; i++ )
			{
				m_renderObject->vertices[i].pos[0] = m_vertices[i].x;
				m_renderObject->vertices[i].pos[1] = m_vertices[i].y;
				m_renderObject->vertices[i].n[0] = m_renderObject->vertices[i].n[1] = 0.0f;
				m_renderObject->vertices[i].n[2] = 1.0f;
			}

			m_invalidateVertices = false;
		}

		return m_vertices;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::invalidateVertices()
	{
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::isInvalidateVertices() const
	{
		return m_invalidateVertices;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );

		//if( m_resource == NULL )
		//{
		//	MENGE_LOG_ERROR( "Sprite \"%s\": Image resource not found \"%s\""
		//		, getName().c_str()
		//		, m_resourceName.c_str() );
		//	return;
		//}

		//const RenderImageInterface * renderImage = m_resource->getImage( m_currentImageIndex );
		m_renderObject->material.textureStage[0].texture = m_resource->getImage( m_currentImageIndex );

		const mt::vec2f* vertices = getVertices();

		if( m_invalidateColor == true )
		{
			uint32 argb = getWorldColor().getAsARGB();
			RenderObject::ApplyColor applyColor( argb );
			std::for_each( m_renderObject->vertices.begin(), m_renderObject->vertices.end(), applyColor );

			if( ( argb & 0xFF000000 ) == 0xFF000000 )
			{
				m_renderObject->material.isSolidColor = true;
			}
			else
			{
				m_renderObject->material.isSolidColor = false;
			}

			//m_renderObject->material.color = getWorldColor();
		}

		Holder<RenderEngine>::hostage()
			->renderObject( m_renderObject );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_update( float _timing )
	{
		Node::_update( _timing );

		if( m_percentVisibilityToCb != NULL )
		{
			mt::vec4f percent = m_percent;
			bool end = m_percentVisibilityTo.update( _timing, &percent );
			mt::vec2f px( percent.x, percent.y );
			mt::vec2f py( percent.z, percent.w );
			setPercentVisibility( px, py );
			if( end == true )
			{
				PyObject* callback = m_percentVisibilityToCb;
				m_percentVisibilityToCb = NULL;
				pybind::call( callback, "(Ob)", this->getEmbedding(), true );
				pybind::decref( callback );
			}
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		invalidateBoundingBox();
		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_setListener()
	{
		Node::_setListener();
		//this->registerEvent( EVENT_COLOR_END, ("onColorEnd"), m_listener );
		//this->registerEvent( EVENT_COLOR_STOP, ("onColorStop"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		const mt::vec2f * vertices = getVertices();

		mt::reset( _boundingBox, vertices[0] );

		for( int i = 1; i != 4; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Sprite::getImageSize()
	{
		if( m_resource == NULL )
		{
			MENGE_LOG_ERROR( "Sprite %s: Can't get image size, because resource is NULL '%s'"
				, getName().c_str()
				, m_resourceName.c_str() 
				);
			
			return mt::vec2f(0.f,0.f);
		}

		const mt::vec2f & size = 
			m_resource->getMaxSize( m_currentImageIndex );
			//m_resource->getSize( m_currentImageIndex );

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::getCenterAlign()
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Sprite::getImageCount() const
	{
		if( m_resource == 0 )
		{
			MENGE_LOG_ERROR( "Sprite %s: Can't get image count, because resource is NULL '%s'"
				, getName().c_str()
				, m_resourceName.c_str() 
				);

			return 0;
		}

		return m_resource->getCount();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setCenterAlign( bool _centerAlign )
	{
		m_centerAlign = _centerAlign;
		updateSprite_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibilityToCb( float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb )
	{
		m_percentVisibilityToCb = _cb;
		pybind::incref( m_percentVisibilityToCb );
		mt::vec4f percentTo( _percentX, _percentY );
		m_percentVisibilityTo.start( m_percent, percentTo, _time, mt::length_v4 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibilityToStop()
	{
		if( m_percentVisibilityToCb != NULL )
		{
			PyObject* callback = m_percentVisibilityToCb;
			m_percentVisibilityToCb = NULL;
			pybind::call( callback, "(Ob)", this->getEmbedding(), false );
			pybind::decref( callback );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageAlpha( const String& _name )
	{
		if( m_alphaImageName != _name )
		{
			m_alphaImageName = _name;
			if( isCompile() == true )
			{
				recompile();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
