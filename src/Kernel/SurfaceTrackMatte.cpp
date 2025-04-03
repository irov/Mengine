#include "SurfaceTrackMatte.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
        RenderProgramVariableInterfacePtr programVariable = RENDER_SYSTEM()
            ->createProgramVariable( 0, 1, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( programVariable, "'%s' invalid create program variable"
            , this->getName().c_str()
        );

        m_programVariable = programVariable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceTrackMatte::_finalize()
    {
        m_programVariable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceTrackMatte::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        this->recompile( [this, &_resourceImage]()
        {
            m_resourceImage = _resourceImage;

            if( m_resourceImage == nullptr )
            {
                return false;
            }

            return true;
        } );
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

        this->recompile( [this, &_resourceTrackMatteImage]()
        {
            m_resourceTrackMatteImage = _resourceTrackMatteImage;

            if( m_resourceTrackMatteImage == nullptr )
            {
                return false;
            }

            return true;
        } );
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
        const mt::uv4f & uv = m_resourceImage->getUVTexture( _index );

        return uv;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceTrackMatte::getColor() const
    {
        const Color & color = m_resourceImage->getColor();

        return color;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceTrackMatte::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        MENGINE_UNUSED( _index );

        *_out = _in;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceTrackMatte::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceImage->compile() == false )
        {
            LOGGER_ERROR( "surface track matte '%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceImage->getName().c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceTrackMatteImage, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceTrackMatteImage->compile() == false )
        {
            LOGGER_ERROR( "surface track matte '%s' resource '%s' is not compile"
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

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTexture( 1 );

        if( textureAlpha == nullptr )
        {
            RenderTextureInterfacePtr textures[2];
            textures[0] = m_resourceImage->getTexture( 0 );

            const RenderTextureInterfacePtr & trackMatteTextureAlpha = m_resourceTrackMatteImage->getTexture( 1 );

            if( trackMatteTextureAlpha == nullptr )
            {
                textures[1] = m_resourceTrackMatteImage->getTexture( 0 );
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
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "surface track matte '%s' mode [%u] invalid support blend mode [%u]"
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
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY, PT_TRIANGLELIST, 2, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "surface track matte '%s' mode [%u] invalid support blend mode [%u]"
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
                    LOGGER_ERROR( "surface track matte '%s' invalid support mode [%u]"
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
            textures[0] = m_resourceImage->getTexture( 0 );
            textures[1] = textureAlpha;

            const RenderTextureInterfacePtr & trackMatteTextureAlpha = m_resourceTrackMatteImage->getTexture( 1 );

            if( trackMatteTextureAlpha == nullptr )
            {
                textures[2] = m_resourceTrackMatteImage->getTexture( 0 );
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
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "surface track matte '%s' mode [%u] invalid support blend mode [%u]"
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
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    case EMB_ADD:
                        {
                            if( premultiply == false )
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                            else
                            {
                                material = this->getMaterial3( EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA, PT_TRIANGLELIST, 3, textures, MENGINE_DOCUMENT_FACTORABLE );
                            }
                        }break;
                    default:
                        {
                            LOGGER_ERROR( "surface track matte '%s' mode [%u] invalid support blend mode [%u]"
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
                    LOGGER_ERROR( "surface track matte '%s' invalid support mode [%u]"
                        , this->getName().c_str()
                        , m_trackMatteMode
                    );

                    return nullptr;
                }break;
            }
        }

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
}
