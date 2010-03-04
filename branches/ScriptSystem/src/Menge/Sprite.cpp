#	include "Sprite.h" 

#	include "Layer2D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"

#	include "pybind/pybind.hpp"

//#	 include "ResourceTexture.h"
#	include "Material.h"
#	include "Affector.h"
#	include "Texture.h"

namespace	Menge
{
	enum ESpriteVerticesInvalidate
	{
		ESVI_POSITION = 0x01,
		ESVI_COLOR = 0x02,
		ESVI_FULL = 0xFF
	};
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
	: m_resource( 0 )
	, m_currentImageIndex( 0 )
	, m_centerAlign( false )
	, m_percent( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_flipX( false )
	, m_flipY( false )
	, m_blendSrc( BF_SOURCE_ALPHA )
	, m_blendDst( BF_ONE_MINUS_SOURCE_ALPHA )
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
			XML_CASE_ATTRIBUTE_NODE( "PercentVisibility", "Value", m_percent );
			XML_CASE_ATTRIBUTE_NODE_METHODT( "BlendSource", "Value", &Sprite::setBlendSource, int );
			XML_CASE_ATTRIBUTE_NODE_METHODT( "BlendDest", "Value", &Sprite::setBlendDest, int );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

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

		if( compileResource_() == false )
		{
			return false;
		}

		std::size_t max = m_resource->getCount();
		if( m_currentImageIndex >= max )
		{
			MENGE_LOG_WARNING( "Warning: (Sprite::_compile) index (%d) >= image count(%d)"
				, m_currentImageIndex
				, max
				);

			return false;
		}

		m_material = Holder<RenderEngine>::hostage()
							->createMaterial();

		m_material->blendSrc = m_blendSrc;
		m_material->blendDst = m_blendDst;

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

				return false;
			}

			m_texturesNum = 2;
			m_textures[1] = m_alphaImage->getTexture( 0 );

			TextureStage & ts = m_material->textureStage[1];

			ts.colorOp = TOP_SELECTARG1;
			ts.colorArg1 = TARG_CURRENT;
			ts.alphaOp = TOP_MODULATE;
			ts.alphaArg1 = TARG_TEXTURE;
			ts.alphaArg2 = TARG_CURRENT;
			//m_material->textureStage[0].alphaOp = TOP_SELECTARG2;
		}
		
		invalidateVertices();
		invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::compileResource_()
	{
		if( m_resource )
		{
			return true;
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

		invalidateVertices( ESVI_FULL );
		invalidateBoundingBox();
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY )
	{
		//m_percent.v2_0 = _percentX;
		//m_percent.v2_1 = _percentY;
		m_percent = mt::vec4f( _percentX, _percentY );

		invalidateVertices();
		invalidateBoundingBox();
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

		invalidateVertices();
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Sprite::getImageIndex() const
	{
		return m_currentImageIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageResource( const String& _name )
	{
		if( m_resourceName == _name )
		{
			return;
		}
		
		m_resourceName = _name;

		m_currentImageIndex = 0; //?? wtf

		recompile();

		//invalidateVertices(); //?? wtf
		//invalidateBoundingBox(); //?? wtf
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Sprite::getImageResource() const
	{
		return m_resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateVertices( Vertex2D * _vertcies, unsigned char _invalidateVertices )
	{
		if( m_resource == 0 ) 
		{
			return;
		}

		if( _invalidateVertices & ESVI_POSITION )
		{
			bool wrapX = m_resource->getWrapX( m_currentImageIndex );
			bool wrapY = m_resource->getWrapY( m_currentImageIndex );

			ResourceImage * resource = m_alphaImage ? m_alphaImage : m_resource;

			mt::vec2f size = resource->getSize( m_currentImageIndex );
			const mt::vec2f& maxSize = resource->getMaxSize( m_currentImageIndex );
			mt::vec2f offset = resource->getOffset( m_currentImageIndex );

			mt::vec4f percentPx( m_percent.x * maxSize.x, m_percent.y * maxSize.y,
				m_percent.z * maxSize.x, m_percent.w * maxSize.y );

			percentPx.x -= offset.x;
			percentPx.y -= offset.y;
			percentPx.z -= (maxSize.x - offset.x - size.x);
			percentPx.w -= (maxSize.y - offset.y - size.y);

			if( wrapX == false && wrapY == false )
			{
				if( percentPx.x < 0.0f )
				{
					percentPx.x = 0.0f;
				}
				else if( percentPx.x > size.x )
				{
					percentPx.x = size.x;
				}
				if( percentPx.y < 0.0f )
				{
					percentPx.y = 0.0f;
				}
				else if( percentPx.y > size.y )
				{
					percentPx.y = size.y;
				}
				if( percentPx.z < 0.0f )
				{
					percentPx.z = 0.0f;
				}
				else if( percentPx.z > size.x )
				{
					percentPx.z = size.x;
				}
				if( percentPx.w < 0.0f )
				{
					percentPx.w = 0.0f;
				}
				else if( percentPx.w > size.y )
				{
					percentPx.w = size.y;
				}
			}

			mt::vec4f percent( percentPx.x / size.x, percentPx.y / size.y,
				percentPx.z / size.x, percentPx.w / size.y );

			// adjust texture visibility
			percent.x = ::floorf( percent.x * size.x + 0.5f ) / size.x;
			percent.y = ::floorf( percent.y * size.y + 0.5f ) / size.y;
			percent.z = ::floorf( percent.z * size.x + 0.5f ) / size.x;
			percent.w = ::floorf( percent.w * size.y + 0.5f ) / size.y;

			if( _invalidateVertices == ESVI_FULL )
			{
				m_textures[0] = m_resource->getTexture( m_currentImageIndex );

				m_material->textureStage[0].addressU = wrapX ? TAM_WRAP : TAM_CLAMP;
				m_material->textureStage[0].addressV = wrapY ? TAM_WRAP : TAM_CLAMP;

				mt::vec4f uv = m_resource->getUV( m_currentImageIndex );

				float uvX = uv.z - uv.x;
				float uvY = uv.w - uv.y;

				uv.x += percent.x * uvX;
				uv.y += percent.y * uvY;
				uv.z -= percent.z * uvX;
				uv.w -= percent.w * uvY;

				if( m_flipX == true )
				{
					std::swap( uv.x, uv.z );
				}

				if( m_flipY == true )
				{
					std::swap( uv.y, uv.w );
				}

				_vertcies[0].uv[0] = uv.x;
				_vertcies[0].uv[1] = uv.y;
				_vertcies[1].uv[0] = uv.z;
				_vertcies[1].uv[1] = uv.y;
				_vertcies[2].uv[0] = uv.z;
				_vertcies[2].uv[1] = uv.w;
				_vertcies[3].uv[0] = uv.x;
				_vertcies[3].uv[1] = uv.w;
			}

			mt::vec2f visOffset( size.x * percent.x, size.y * percent.y );

			size.x -= size.x * ( percent.x + percent.z );
			size.y -= size.y * ( percent.y + percent.w );

			if( m_centerAlign )
			{
				mt::vec2f alignOffset = maxSize * -0.5f;
				alignOffset.x = ::floorf( alignOffset.x + 0.5f );
				alignOffset.y = ::floorf( alignOffset.y + 0.5f );

				visOffset += alignOffset;
			}

			if( m_flipX )
			{
				offset.x = maxSize.x - ( size.x + offset.x );
			}

			if( m_flipY )
			{
				offset.y = maxSize.y - ( size.y + offset.y );
			}

			visOffset += offset;

			if( m_alphaImage )
			{
				const mt::vec2f& rgbSize = m_resource->getSize( m_currentImageIndex );

				if( rgbSize.x > size.x || rgbSize.y > size.y )
				{
					Texture* rgbTexture = m_resource->getTexture( m_currentImageIndex );
					if( m_material->textureStage[0].matrix == NULL )
					{
						m_material->textureStage[0].matrix = new mt::mat4f();
					}
					mt::mat4f* texMat = m_material->textureStage[0].matrix;
					mt::ident_m4( *texMat );
					texMat->v0.x = size.x / rgbSize.x;
					texMat->v1.y = size.y / rgbSize.y;

					texMat->v2.x = offset.x / rgbTexture->getHWWidth();		// ugly place :( We must not know about HW sizes of
					texMat->v2.y = offset.y / rgbTexture->getHWHeight();	// texture here, either about texture matrix
				}
			}

			const mt::mat3f & wm = getWorldMatrix();

			//mt::mul_v2_m3( m_vertices[0], m_offset, wm );
			//mt::mul_v2_m3( m_vertices[1], m_offset + mt::vec2f( m_size.x, 0.0f ), wm );
			//mt::mul_v2_m3( m_vertices[2], m_offset + m_size, wm );
			//mt::mul_v2_m3( m_vertices[3], m_offset + mt::vec2f( 0.0f, m_size.y ), wm );
			mt::vec2f transformX;
			mt::vec2f transformY;

			mt::vec2f vertices[4];

			mt::mul_v2_m3( vertices[0], visOffset, wm );
			mt::mul_v2_m3_r( transformX, mt::vec2f( size.x, 0.0f ), wm );
			mt::mul_v2_m3_r( transformY, mt::vec2f( 0.0f, size.y ), wm );

			vertices[1] = vertices[0] + transformX;
			vertices[2] = vertices[1] + transformY;
			vertices[3] = vertices[0] + transformY;

			for( int i = 0; i < 4; i++ )
			{
				_vertcies[i].pos[0] = vertices[i].x;
				_vertcies[i].pos[1] = vertices[i].y;
			}
		}

		if( _invalidateVertices & ESVI_COLOR )
		{
			uint32 argb = getWorldColor().getAsARGB();
			ApplyColor2D applyColor( argb );
			std::for_each( _vertcies, _vertcies + 4, applyColor );

			if( ( argb & 0xFF000000 ) == 0xFF000000 )
			{
				m_material->isSolidColor = true;
			}
			else
			{
				m_material->isSolidColor = false;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		//m_textures[0] = m_resource->getTexture( m_currentImageIndex );

		Vertex2D * vertices = getVertices();

		Holder<RenderEngine>::hostage()
			->renderObject2D( m_material, m_textures, m_texturesNum, vertices, 4, LPT_QUAD );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();
		invalidateVertices( ESVI_POSITION );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_setListener()
	{
		Node::_setListener();

		//ToDo
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Vertex2D * vertices = getVertices();

		mt::reset( _boundingBox, vertices[0].pos[0], vertices[0].pos[1] );

		for( int i = 1; i != 4; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i].pos[0], vertices[i].pos[1] );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_invalidateColor()
	{
		invalidateVertices( ESVI_COLOR );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Sprite::getImageSize()
	{
		compileResource_();

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
	bool Sprite::getCenterAlign() const
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Sprite::getImageCount()
	{
		compileResource_();

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

		invalidateVertices( ESVI_POSITION );
		invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibilityToCb( float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb )
	{
		stopAffectors_( MENGE_AFFECTOR_VISIBILITY );

		Affector* affector = 
			NodeAffectorCreator::newNodeAffectorInterpolateLinear(
			_cb, MENGE_AFFECTOR_VISIBILITY, this, &Sprite::setPercentVisibilityVec4f_
			, getPercentVisibility(), mt::vec4f( _percentX, _percentY ), _time, 
			&mt::length_v4 
			);

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
		if( m_alphaImageName == _name )
		{
			return;
		}

		m_alphaImageName = _name;

		recompile();

		//invalidateVertices();
		//invalidateBoundingBox();
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
	void Sprite::setBlendSource( EBlendFactor _src )
	{
		m_blendSrc = _src;

		if( m_material )
		{
			m_material->blendSrc = m_blendSrc;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setBlendDest( EBlendFactor _dst )
	{
		m_blendDst = _dst;

		if( m_material )
		{
			m_material->blendDst = m_blendDst;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& Sprite::getPercentVisibility() const
	{
		return m_percent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibilityVec4f_( const mt::vec4f& _percent )
	{
		setPercentVisibility( mt::vec2f( _percent.x, _percent.y ), mt::vec2f( _percent.z, _percent.w ) );
	}
	//////////////////////////////////////////////////////////////////////////
}
