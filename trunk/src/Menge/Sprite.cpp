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
#	include "Material.h"
#	include "NodeAffector.h"
#	include "Texture.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Sprite);
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
	, m_material( NULL )
	, m_alphaImage( NULL )
	, m_disableTextureColor( false )
	, m_texturesNum( 0 )
	{ 
		m_textures[0] = NULL;
		m_textures[1] = NULL;
	}
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
			MENGE_LOG_ERROR( "Sprite: Image resource not found '%s'"
				, m_resourceName.c_str() 
				);

			return false;
		}

		m_material = Holder<RenderEngine>::hostage()
							->createMaterial();

		m_texturesNum = 1;

		m_material->textureStage[0].alphaOp = TOP_MODULATE;

		if( m_disableTextureColor == true )
		{
			m_material->textureStage[0].colorOp = TOP_SELECTARG2;
		}
		else
		{
			m_material->textureStage[0].colorOp = TOP_MODULATE;
		}

		if( m_alphaImageName.empty() == false )
		{
			m_alphaImage = Holder<ResourceManager>::hostage()
							->getResourceT<ResourceImage>( m_alphaImageName );

			if( m_alphaImage == NULL )
			{
				MENGE_LOG_ERROR( "Warning: (Sprite::_compile) can't get AlphaImage '%s'"
					, m_alphaImageName.c_str() 
					);
			}
			else
			{
				m_texturesNum = 2;
				m_textures[1] = m_alphaImage->getTexture( 0 );
				m_material->textureStage[1].colorOp = TOP_SELECTARG1;
				m_material->textureStage[1].colorArg1 = TARG_CURRENT;
				m_material->textureStage[1].alphaOp = TOP_MODULATE;
				m_material->textureStage[1].alphaArg1 = TARG_TEXTURE;
				m_material->textureStage[1].alphaArg2 = TARG_CURRENT;
				//m_material->textureStage[0].alphaOp = TOP_SELECTARG2;
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

		if( m_material != NULL &&
			m_material->textureStage[0].matrix != NULL )
		{
			delete m_material->textureStage[0].matrix;
			m_material->textureStage[0].matrix = NULL;
		}
		Holder<RenderEngine>::hostage()
			->releaseMaterial( m_material );

		m_material = NULL;

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
				MENGE_LOG_WARNING( "Warning: (Sprite::setImageIndex) index (%d) >= image count(%d)"
					, _index
					, max
					);

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

		m_textures[0] = m_resource->getTexture( m_currentImageIndex );
		bool wrapX = m_resource->getWrapX( m_currentImageIndex );
		bool wrapY = m_resource->getWrapY( m_currentImageIndex );
		m_material->textureStage[0].addressU = wrapX ? TAM_WRAP : TAM_CLAMP;
		m_material->textureStage[0].addressV = wrapY ? TAM_WRAP : TAM_CLAMP;

		m_material->blendSrc = m_blendSrc;
		m_material->blendDst = m_blendDest;

		if( m_alphaImage )
		{
			updateDimensions_( m_alphaImage, wrapX || wrapY );
		}
		else
		{
			updateDimensions_( m_resource, wrapX || wrapY );
		}

		invalidateBoundingBox();
		invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateDimensions_( ResourceImage * _resource, bool _wrap )
	{
		m_size = _resource->getSize( m_currentImageIndex );
		const mt::vec2f& maxSize = _resource->getMaxSize( m_currentImageIndex );
		mt::vec2f offset = _resource->getOffset( m_currentImageIndex );

		mt::vec4f percentPx( m_percent.x * maxSize.x, m_percent.y * maxSize.y,
							m_percent.z * maxSize.x, m_percent.w * maxSize.y );
		percentPx.x -= offset.x;
		percentPx.y -= offset.y;
		percentPx.z -= (maxSize.x - offset.x - m_size.x);
		percentPx.w -= (maxSize.y - offset.y - m_size.y);

		if( _wrap == false )
		{
			if( percentPx.x < 0.0f )
			{
				percentPx.x = 0.0f;
			}
			else if( percentPx.x > m_size.x )
			{
				percentPx.x = m_size.x;
			}
			if( percentPx.y < 0.0f )
			{
				percentPx.y = 0.0f;
			}
			else if( percentPx.y > m_size.y )
			{
				percentPx.y = m_size.y;
			}
			if( percentPx.z < 0.0f )
			{
				percentPx.z = 0.0f;
			}
			else if( percentPx.z > m_size.x )
			{
				percentPx.z = m_size.x;
			}
			if( percentPx.w < 0.0f )
			{
				percentPx.w = 0.0f;
			}
			else if( percentPx.w > m_size.y )
			{
				percentPx.w = m_size.y;
			}
		}

		mt::vec4f percent( percentPx.x / m_size.x, percentPx.y / m_size.y,
							percentPx.z / m_size.x, percentPx.w / m_size.y );

		// adjust texture visibility
		percent.x = ::floorf( percent.x * m_size.x + 0.5f ) / m_size.x;
		percent.y = ::floorf( percent.y * m_size.y + 0.5f ) / m_size.y;
		percent.z = ::floorf( percent.z * m_size.x + 0.5f ) / m_size.x;
		percent.w = ::floorf( percent.w * m_size.y + 0.5f ) / m_size.y;

		mt::vec2f  visOffset( m_size.x * percent.x, m_size.y * percent.y );

		m_size.x = m_size.x - m_size.x * ( percent.x + percent.z );
		m_size.y = m_size.y - m_size.y * ( percent.y + percent.w );

		if( m_centerAlign )
		{
			m_alignOffset = maxSize * -0.5f;
			m_alignOffset.x = ::floorf( m_alignOffset.x + 0.5f );
			m_alignOffset.y = ::floorf( m_alignOffset.y + 0.5f );
		}

		if( m_flipX )
		{
			offset.x = maxSize.x - ( m_size.x + offset.x );
		}

		if( m_flipY )
		{
			offset.y = maxSize.y - ( m_size.y + offset.y );
		}

		m_offset = offset + m_alignOffset + visOffset;

		m_uv = _resource->getUV( m_currentImageIndex );

		float uvX = m_uv.z - m_uv.x;
		float uvY = m_uv.w - m_uv.y;

		m_uv.x = m_uv.x + percent.x * uvX;
		m_uv.y = m_uv.y + percent.y * uvY;
		m_uv.z = m_uv.z - percent.z * uvX;
		m_uv.w = m_uv.w - percent.w * uvY;

		if( m_alphaImage )
		{
			const mt::vec2f& rgbSize = m_resource->getSize( m_currentImageIndex );
			if( rgbSize.x > m_size.x 
				|| rgbSize.y > m_size.y )
			{
				Texture* rgbTexture = m_resource->getTexture( m_currentImageIndex );
				if( m_material->textureStage[0].matrix == NULL )
				{
					m_material->textureStage[0].matrix = new mt::mat4f();
				}
				mt::mat4f* texMat = m_material->textureStage[0].matrix;
				mt::ident_m4( *texMat );
				texMat->v0.x = m_size.x / rgbSize.x;
				texMat->v1.y = m_size.y / rgbSize.y;
				
				texMat->v2.x = offset.x / rgbTexture->getHWWidth();		// ugly place :( We must not know about HW sizes of
				texMat->v2.y = offset.y / rgbTexture->getHWHeight();	// texture here, either about texture matrix
				
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

		m_vertices[0].uv[0] = m_uv.x;
		m_vertices[0].uv[1] = m_uv.y;
		m_vertices[1].uv[0] = m_uv.z;
		m_vertices[1].uv[1] = m_uv.y;
		m_vertices[2].uv[0] = m_uv.z;
		m_vertices[2].uv[1] = m_uv.w;
		m_vertices[3].uv[0] = m_uv.x;
		m_vertices[3].uv[1] = m_uv.w;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateVertices()
	{
		if( m_invalidateVertices == false )
		{
			return;
		}

		m_invalidateVertices = false;

		const mt::mat3f & wm = getWorldMatrix();

		//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
		//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
		//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
		//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
		mt::vec2f transformX;
		mt::vec2f transformY;

		mt::vec2f vertices[4];

		mt::mul_v2_m3( vertices[0], m_offset, wm );
		mt::mul_v2_m3_r( transformX, mt::vec2f( m_size.x, 0.0f ), wm );
		mt::mul_v2_m3_r( transformY, mt::vec2f( 0.0f, m_size.y ), wm );

		vertices[1] = vertices[0] + transformX;
		vertices[2] = vertices[1] + transformY;
		vertices[3] = vertices[0] + transformY;

		for( int i = 0; i < 4; i++ )
		{
			m_vertices[i].pos[0] = vertices[i].x;
			m_vertices[i].pos[1] = vertices[i].y;
		}

		uint32 argb = getWorldColor().getAsARGB();
		ApplyColor2D applyColor( argb );
		std::for_each( m_vertices, m_vertices + 4, applyColor );

		if( ( argb & 0xFF000000 ) == 0xFF000000 )
		{
			m_material->isSolidColor = true;
		}
		else
		{
			m_material->isSolidColor = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::invalidateVertices()
	{
		m_invalidateVertices = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		m_textures[0] = m_resource->getTexture( m_currentImageIndex );

		updateVertices();

		Holder<RenderEngine>::hostage()
			->renderObject2D( m_material, m_textures, m_texturesNum, m_vertices, 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
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
		updateVertices();

		mt::reset( _boundingBox, m_vertices[0].pos[0], m_vertices[0].pos[1] );

		for( int i = 1; i != 4; ++i )
		{
			mt::add_internal_point( _boundingBox, m_vertices[i].pos[0], m_vertices[i].pos[1] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateColor()
	{
		invalidateVertices();
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
		stopAffectors_( MENGE_AFFECTOR_VISIBILITY );

		NodeAffector* affector = 
			NodeAffectorCreator::newNodeAffectorInterpolateLinear<mt::vec4f, Sprite>(
			_cb, MENGE_AFFECTOR_VISIBILITY, getPercentVisibility(), mt::vec4f( _percentX, _percentY ), _time, 
			&mt::length_v4, &Sprite::setPercentVisibilityVec4f );

		m_affectorsToAdd.push_back( affector );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibilityToStop()
	{
		stopAffectors_( MENGE_AFFECTOR_VISIBILITY );
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
	void Sprite::disableTextureColor( bool _disable )
	{
		m_disableTextureColor = _disable;
		if( m_material == NULL )
		{
			return;
		}
		// else
		if( m_disableTextureColor == true )
		{
			m_material->textureStage[0].colorOp = TOP_SELECTARG2;
		}
		else
		{
			m_material->textureStage[0].colorOp = TOP_MODULATE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& Sprite::getPercentVisibility() const
	{
		return m_percent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibilityVec4f( const mt::vec4f& _percent )
	{
		setPercentVisibility( mt::vec2f( _percent.x, _percent.y ), mt::vec2f( _percent.z, _percent.w ) );
	}
	//////////////////////////////////////////////////////////////////////////
}
