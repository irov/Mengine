#include "SurfaceTrackMatte.h"

#   include "Interface/RenderSystemInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/ResourceImage.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	SurfaceTrackMatte::SurfaceTrackMatte()
        : m_trackMatteMode( ESTM_MODE_NONE )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SurfaceTrackMatte::~SurfaceTrackMatte()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::setResourceImage( const ResourceImagePtr & _resourceImage )
	{
		if( m_resourceImage == _resourceImage )
		{
			return;
		}

		m_resourceImage = _resourceImage;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & SurfaceTrackMatte::getResourceImage() const
	{
		return m_resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::setResourceTrackMatteImage( const ResourceImagePtr & _resourceTrackMatteImage )
	{
		if( m_resourceTrackMatteImage == _resourceTrackMatteImage )
		{
			return;
		}

		m_resourceTrackMatteImage = _resourceTrackMatteImage;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & SurfaceTrackMatte::getResourceTrackMatteImage() const
	{
		return m_resourceTrackMatteImage;
	}
    //////////////////////////////////////////////////////////////////////////
    void SurfaceTrackMatte::setTrackMatteMode( ESurfaceTrackMatteMode _trackMatteMode )
    {
        m_trackMatteMode = _trackMatteMode;
    }
    //////////////////////////////////////////////////////////////////////////
    ESurfaceTrackMatteMode SurfaceTrackMatte::getTrackMatteMode() const
    {
        return m_trackMatteMode;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceTrackMatte::getMaxSize() const
    {
        const mt::vec2f & maxSize = m_resourceImage->getMaxSize();

        return maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceTrackMatte::getSize() const
    {
        const mt::vec2f & size = m_resourceImage->getSize();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceTrackMatte::getOffset() const
    {
        const mt::vec2f & offset = m_resourceImage->getOffset();

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceTrackMatte::getUVCount() const
    {
        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceTrackMatte::getUV( uint32_t _index ) const
    {
        if( _index == 0 )
        {
            return m_resourceImage->getUVImage();
        }
        if( _index == 1 )
        {
            return m_resourceTrackMatteImage->getUVImage();
        }

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & SurfaceTrackMatte::getColour() const
    {
        const ColourValue & color = m_resourceImage->getColor();

        return color;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceTrackMatte::correctUV( uint32_t _index, mt::vec2f & _out, const mt::vec2f & _in )
    {
        (void)_index;

        _out = _in;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceTrackMatte::_update( float _current, float _timing )
    {
        (void)_current;
        (void)_timing;

        return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceTrackMatte::_compile()
	{
		if( m_resourceImage == nullptr )
		{
			LOGGER_ERROR("SurfaceTrackMatte::_compile: '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

		if( m_resourceImage.compile() == false )
		{
			LOGGER_ERROR("SurfaceTrackMatte::_compile: '%s' resource '%s' is not compile"
				, m_name.c_str()
				, m_resourceImage->getName().c_str()
				);

			return false;
		}

		if( m_resourceTrackMatteImage == nullptr )
		{
			LOGGER_ERROR("SurfaceTrackMatte::_compile: '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

		if( m_resourceTrackMatteImage.compile() == false )
		{
			LOGGER_ERROR("SurfaceTrackMatte::_compile: '%s' resource '%s' is not compile"
				, m_name.c_str()
				, m_resourceTrackMatteImage->getName().c_str()
				);

			return false;
		}

		this->invalidateMaterial();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::_release()
	{
		m_resourceImage.release();
		m_resourceTrackMatteImage.release();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr SurfaceTrackMatte::_updateMaterial() const
	{
		RenderTextureInterfacePtr textures[2];

		textures[0] = m_resourceImage->getTexture();
		textures[1] = m_resourceTrackMatteImage->getTexture();

        RenderMaterialInterfacePtr material = nullptr;

        switch( m_trackMatteMode )
        {
        case ESTM_MODE_ALPHA:
            {
                material = RENDERMATERIAL_SERVICE()
                    ->getMaterial( STRINGIZE_STRING_LOCAL( "TrackMatte_Blend" ), PT_TRIANGLELIST, 2, textures );
            }break;
        case ESTM_MODE_ALPHA_INVERTED:
            {
                material = RENDERMATERIAL_SERVICE()
                    ->getMaterial( STRINGIZE_STRING_LOCAL( "TrackMatteInverted_Blend" ), PT_TRIANGLELIST, 2, textures );
            }break;
        default:
            {
                LOGGER_ERROR("SurfaceTrackMatte::_updateMaterial '%s' invalid support track matte mode '%d'"
                    , this->getName().c_str()
                    , m_trackMatteMode
                    );
            }break;
        }

        return material;
	}
	//////////////////////////////////////////////////////////////////////////
}
