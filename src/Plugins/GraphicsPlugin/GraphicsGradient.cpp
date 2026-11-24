#include "GraphicsGradient.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Kernel/ConstStringHelper.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            static uint8_t lerpGradientComponent( float _left, float _right, float _ratio )
            {
                float component = _left + (_right - _left) * _ratio;
                uint8_t value = (uint8_t)StdMath::round( component * 255.f );

                return value;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderMaterialStage * createGraphicsGradientStage( bool _radial, const DocumentInterfacePtr & _doc )
        {
            const RenderMaterialStage * base = RENDERMATERIAL_SERVICE()
                ->getMaterialStage( STRINGIZE_STRING_LOCAL( "Texture_Blend" ) );

            if( base == nullptr )
            {
                return nullptr;
            }

            RenderMaterialStage stage = *base;
            RenderTextureStage & ramp = stage.textureStages[0];
            ramp.addressU = TAM_CLAMP;
            ramp.addressV = TAM_CLAMP;
            ramp.mipmap = TF_NONE;
            ramp.magnification = TF_LINEAR;
            ramp.minification = TF_LINEAR;

            ERenderPlatform platform = RENDER_SYSTEM()
                ->getRenderPlatformType();

            if( _radial == true && platform != RP_UNKNOWN )
            {
                stage.program = RENDERMATERIAL_SERVICE()
                    ->getProgram( STRINGIZE_STRING_LOCAL( "GraphicsRadialGradient" ) );
            }

            const RenderMaterialStage * cached = RENDERMATERIAL_SERVICE()
                ->cacheMaterialStage( stage, _doc );

            return cached;
        }
        //////////////////////////////////////////////////////////////////////////
        bool uploadGraphicsGradientRamp( const RenderTextureInterfacePtr & _texture, const VectorGraphicsGradientStops & _stops )
        {
            const RenderImageInterfacePtr & image = _texture->getImage();
            uint32_t width = _texture->getWidth();
            Rect rect;
            rect.left = 0;
            rect.top = 0;
            rect.right = width;
            rect.bottom = 1;

            RenderImageLockedInterfacePtr locked = image->lock( 0, 0, rect, false );

            if( locked == nullptr )
            {
                return false;
            }

            size_t pitch;
            uint8_t * buffer = static_cast<uint8_t *>(locked->getLockedBuffer( &pitch ));

            if( buffer == nullptr )
            {
                image->unlock( locked, 0, 0, false );

                return false;
            }

            size_t stopIndex = 0;
            size_t lastIndex = _stops.size() - 1;
            float inverseWidth = 1.f / (float)(width - 1);

            for( uint32_t pixel = 0; pixel != width; ++pixel )
            {
                float offset = (float)pixel * inverseWidth;

                while( stopIndex < lastIndex && _stops[stopIndex + 1].offset <= offset )
                {
                    ++stopIndex;
                }

                const GraphicsGradientStopDesc & left = _stops[stopIndex];
                const GraphicsGradientStopDesc & right = stopIndex == lastIndex ? left : _stops[stopIndex + 1];
                float ratio = offset <= left.offset || stopIndex == lastIndex ? 0.f : (offset - left.offset) / (right.offset - left.offset);

                uint8_t r = Detail::lerpGradientComponent( left.color.getR(), right.color.getR(), ratio );
                uint8_t g = Detail::lerpGradientComponent( left.color.getG(), right.color.getG(), ratio );
                uint8_t b = Detail::lerpGradientComponent( left.color.getB(), right.color.getB(), ratio );
                uint8_t a = Detail::lerpGradientComponent( left.color.getA(), right.color.getA(), ratio );

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
                buffer[0] = r;
                buffer[1] = g;
                buffer[2] = b;
#else
                buffer[0] = b;
                buffer[1] = g;
                buffer[2] = r;
#endif
                buffer[3] = a;
                buffer += 4;
            }

            if( image->unlock( locked, 0, 0, true ) == false )
            {
                return false;
            }

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
