#include "SurfaceTrackMatte.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    bool SurfaceTrackMatte::_initialize()
    {
        m_programVariable = RENDER_SYSTEM()
            ->createProgramVariable( 0, 1, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( m_programVariable, false, "'%s' invalid create program variable"
            , this->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceTrackMatte::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        this->recompile( [this, _resourceImage]() { m_resourceImage = _resourceImage; } );
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

        this->recompile( [this, _resourceTrackMatteImage]() {m_resourceTrackMatteImage = _resourceTrackMatteImage; } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & SurfaceTrackMatte::getResourceTrackMatteImage() const
    {
        return m_resourceTrackMatteImage;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderProgramVariableInterfacePtr & SurfaceTrackMatte::getProgramVariable() const
    {
        return m_programVariable;
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
        switch( _index )
        {
        case 0:
            {
                return m_resourceImage->getUVTextureImage();
            }break;
        case 1:
            {
                return m_resourceTrackMatteImage->getUVTextureImage();
            }break;
        default:
            break;
        }

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceTrackMatte::getColor() const
    {
        const Color & color = m_resourceImage->getColor();

        return color;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceTrackMatte::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out ) const
    {
        MENGINE_UNUSED( _index );

        *_out = _in;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceTrackMatte::update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceTrackMatte::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, false, "'%s' resource is null"
            , this->getName().c_str()
        );

        if( m_resourceImage->compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceImage->getName().c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceTrackMatteImage, false, "'%s' resource is null"
            , this->getName().c_str()
        );

        if( m_resourceTrackMatteImage->compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' is not compile"
                , this->getName().c_str()
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
        m_resourceImage->release();
        m_resourceTrackMatteImage->release();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr SurfaceTrackMatte::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = nullptr;

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTextureAlpha();

        if( textureAlpha == nullptr )
        {
            RenderTextureInterfacePtr textures[2];
            textures[0] = m_resourceImage->getTexture();

            const RenderTextureInterfacePtr & trackMatteTextureAlpha = m_resourceTrackMatteImage->getTextureAlpha();

            if( trackMatteTextureAlpha == nullptr )
            {
                textures[1] = m_resourceTrackMatteImage->getTexture();
            }
            else
            {
                textures[1] = trackMatteTextureAlpha;
            }

            EMaterialBlendMode blendMode = this->getBlendMode();
            bool premultiply = m_resourceImage->isPremultiply();

            switch( m_trackMatteMode )
            {
            case ESTM_MODE_ALPHA:
                {
                    switch( blendMode )
                    {
                    case EMB_NORMAL:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "'%s' track matte mode '%d' invalid support blend mode '%d'"
                                , this->getName().c_str()
                                , m_trackMatteMode
                                , blendMode
                            );

                            return nullptr;
                        }break;
                    }
                }break;
            case ESTM_MODE_ALPHA_INVERTED:
                {
                    switch( blendMode )
                    {
                    case EMB_NORMAL:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "'%s' track matte mode '%d' invalid support blend mode '%d'"
                                , this->getName().c_str()
                                , m_trackMatteMode
                                , blendMode
                            );

                            return nullptr;
                        }break;
                    }
                }break;
            default:
                {
                    LOGGER_ERROR( "'%s' invalid support track matte mode '%d'"
                        , this->getName().c_str()
                        , m_trackMatteMode
                    );

                    return nullptr;
                }break;
            }
        }
        else
        {
            RenderTextureInterfacePtr textures[3];
            textures[0] = m_resourceImage->getTexture();
            textures[1] = textureAlpha;

            const RenderTextureInterfacePtr & trackMatteTextureAlpha = m_resourceTrackMatteImage->getTextureAlpha();

            if( trackMatteTextureAlpha == nullptr )
            {
                textures[2] = m_resourceTrackMatteImage->getTexture();
            }
            else
            {
                textures[2] = trackMatteTextureAlpha;
            }

            EMaterialBlendMode blendMode = this->getBlendMode();
            bool premultiply = m_resourceImage->isPremultiply();

            switch( m_trackMatteMode )
            {
            case ESTM_MODE_ALPHA:
                {
                    switch( blendMode )
                    {
                    case EMB_NORMAL:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "'%s' track matte mode '%d' invalid support blend mode '%d'"
                                , this->getName().c_str()
                                , m_trackMatteMode
                                , blendMode
                            );

                            return nullptr;
                        }break;
                    }
                }break;
            case ESTM_MODE_ALPHA_INVERTED:
                {
                    switch( blendMode )
                    {
                    case EMB_NORMAL:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FUNCTION );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "'%s' track matte mode '%d' invalid support blend mode '%d'"
                                , this->getName().c_str()
                                , m_trackMatteMode
                                , blendMode
                            );

                            return nullptr;
                        }break;
                    }
                }break;
            default:
                {
                    LOGGER_ERROR( "'%s' invalid support track matte mode '%d'"
                        , this->getName().c_str()
                        , m_trackMatteMode
                    );

                    return nullptr;
                }break;
            }
        }

        return material;
    }
}
