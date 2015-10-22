#   include "Shape.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Shape::Shape()
		: m_customSize( -1.f, -1.f )
		, m_percentVisibility( 0.f, 0.f, 1.f, 1.f )
		, m_textureUVOffset( 0.f, 0.f )
		, m_textureUVScale( 1.f, 1.f )
		, m_centerAlign( false )
		, m_flipX( false )
		, m_flipY( false )
		, m_invalidateVerticesLocal( true )
		, m_invalidateVerticesWM( true )
		, m_invalidateVerticesColor( true )
		, m_solid( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Shape::~Shape()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void Shape::setResourceImage( ResourceImage * _resourceImage )
	{
		if( m_resourceImage == _resourceImage )
		{
			return;
		}

		m_resourceImage = _resourceImage;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage * Shape::getResourceImage() const
	{
		return m_resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Shape::_compile()
	{
		if( m_resourceImage == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Shape::compileResource_ '%s' image resource null"
				, m_name.c_str()
				);

			return false;
		}

		if( m_resourceImage.compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Shape::compileResource_ '%s' image resource %s not compile"
				, m_name.c_str()
				, m_resourceImage->getName().c_str()
				);

			return false;
		}

		this->invalidateMaterial();
		this->invalidateVertices();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Shape::_release()
	{
		Node::_release();

		m_resourceImage.release();

		this->releaseMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	void Shape::setCustomSize( const mt::vec2f & _customSize )
	{
		m_customSize = _customSize;		

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	void Shape::removeCustomSize()
	{ 
		m_customSize.x = -1.f;
		m_customSize.y = -1.f;

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Shape::hasCustomSize() const
	{
		return m_customSize.x < 0.f || m_customSize.y < 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Shape::getCustomSize() const
	{
		return m_customSize;
	}
    //////////////////////////////////////////////////////////////////////////
    void Shape::setCenterAlign( bool _centerAlign )
    {
        m_centerAlign = _centerAlign;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getCenterAlign() const
    {
        return m_centerAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setFlipX( bool _flipX )
    {
        m_flipX = _flipX;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getFlipY() const
    {
        return m_flipY;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setFlipY( bool _flipY )
    {
        m_flipY = _flipY;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getFlipX() const
    {
        return m_flipX;
    }
    ///////////////////////////////////////////////////////////////////////////
    void Shape::setPercentVisibility( const mt::vec4f& _percent )
    {
        m_percentVisibility = _percent;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & Shape::getPercentVisibility() const
    {
        return m_percentVisibility;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setTextureUVOffset( const mt::vec2f & _offset )
    {
        m_textureUVOffset = _offset;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Shape::getTextureUVOffset() const
    {
        return m_textureUVOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setTextureUVScale( const mt::vec2f & _scale )
    {
        m_textureUVScale = _scale;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Shape::getTextureUVScale() const
    {
        return m_textureUVScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVertices()
    {
        m_invalidateVerticesLocal = true;

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesWM()
    {
        m_invalidateVerticesWM = true;

		this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::updateVertices() const
    {
        m_invalidateVerticesLocal = false;

		mt::vec2f maxSize;
		mt::vec2f size;
		mt::vec2f offset;

		if( m_customSize.x < 0.f || m_customSize.y < 0.f )
		{
			const mt::vec2f & image_maxSize = m_resourceImage->getMaxSize();
			const mt::vec2f & image_size = m_resourceImage->getSize();
			const mt::vec2f & image_offset = m_resourceImage->getOffset();

			maxSize = image_maxSize;
			size = image_size;
			offset = image_offset;
		}
		else
		{
			maxSize = m_customSize;
			size = m_customSize;
			offset = mt::vec2f(0.f, 0.f);
		}

		mt::vec4f percent_size = m_percentVisibility * size;
				
		mt::vec2f visOffset(percent_size.x, percent_size.y);

        if( m_centerAlign == true )
        {
            mt::vec2f alignOffset = - size * 0.5f;

            visOffset += alignOffset;
        }
				
		if( m_flipX == true )
		{
			visOffset.x += maxSize.x - (percent_size.z + offset.x);
		}
		else
		{
			visOffset.x += offset.x;
		}

		if( m_flipY == true )
		{
			visOffset.y += maxSize.y - (percent_size.w + offset.y);
		}
		else
		{
			visOffset.y += offset.y;
		}

        m_verticesLocal[0].x = visOffset.x + 0.f;
        m_verticesLocal[0].y = visOffset.y + 0.f;
        m_verticesLocal[0].z = 0.f;

        m_verticesLocal[1].x = visOffset.x + percent_size.z;
        m_verticesLocal[1].y = visOffset.y + 0.f;
        m_verticesLocal[1].z = 0.f;

        m_verticesLocal[2].x = visOffset.x + percent_size.z;
        m_verticesLocal[2].y = visOffset.y + percent_size.w;
        m_verticesLocal[2].z = 0.f;

        m_verticesLocal[3].x = visOffset.x + 0.f;
        m_verticesLocal[3].y = visOffset.y + percent_size.w;
        m_verticesLocal[3].z = 0.f;

		const mt::uv4f & uv_image = m_resourceImage->getUVImage();
		const mt::uv4f & uv_alpha = m_resourceImage->getUVAlpha();

		// RGB(A)
		{
			mt::uv4f uv;
			mt::multiply_tetragon_uv4_v4( uv, uv_image, m_percentVisibility );

			if( m_flipX == true )
			{
				mt::uv4_swap_u( uv );
			}

			if( m_flipY == true )
			{
				mt::uv4_swap_v( uv );
			}

			for( size_t i = 0; i != 4; ++i )
			{
				m_verticesWM[i].uv[0] = uv[i] * m_textureUVScale + m_textureUVOffset;
			}
		}

		// Alpha
		//if( m_textures[1] != nullptr )
		{
			mt::uv4f uv;
			mt::multiply_tetragon_uv4_v4( uv, uv_alpha, m_percentVisibility );

			if( m_flipX == true )
			{
				mt::uv4_swap_u( uv );
			}

			if( m_flipY == true )
			{
				mt::uv4_swap_v( uv );
			}

			for( size_t i = 0; i != 4; ++i )
			{
				m_verticesWM[i].uv[1] = uv[i] * m_textureUVScale + m_textureUVOffset;
			}
		}

		for( size_t i = 0; i != 4; ++i )
		{
			for( uint32_t j = 2; j < MENGINE_RENDER_VERTEX_UV_COUNT; ++j )
			{
				m_verticesWM[i].uv[j].x = 0.f;
				m_verticesWM[i].uv[j].y = 0.f;
			}
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesColor()
    {
        m_invalidateVerticesColor = true;                
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::updateVerticesColor() const
    {
        m_invalidateVerticesColor = false;

        ColourValue color;
		this->calcTotalColor( color );

		const ColourValue & textureColour = m_resourceImage->getColor();
		color *= textureColour;

        uint32_t argb = color.getAsARGB();
		
		m_verticesWM[0].color = argb;
		m_verticesWM[1].color = argb;
		m_verticesWM[2].color = argb;
		m_verticesWM[3].color = argb;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateColor()
    {
        Node::_invalidateColor();

        this->invalidateVerticesColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::updateVerticesWM() const
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVerticesLocal == true )
        {
            this->updateVertices();
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint32_t i = 0; i != 4; ++i )
        {
            const mt::vec3f & pos = m_verticesLocal[i];

            mt::vec3f & wm_pos = m_verticesWM[i].pos;
            mt::mul_v3_m4( wm_pos, pos, wm);
        }
    }
}