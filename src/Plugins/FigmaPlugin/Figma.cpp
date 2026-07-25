#include "Figma.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/RenderImageInterface.h"
#include "Interface/RenderImageLockedInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/Data.h"
#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/StringView.h"
#include "Kernel/VectorString.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdCType.h"
#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdMath.h"
#include "Config/StdString.h"

#include "freetype/freetype.h"

#include "math/uv4.h"

#if !defined(MENGINE_PLATFORM_WINDOWS)
#   include <dirent.h>
#   include <sys/stat.h>
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class FigmaRenderScissor
            : public Factorable
            , public RenderScissorInterface
        {
            DECLARE_FACTORABLE( FigmaRenderScissor );

        public:
            void setScissorViewport( const mt::mat4f & _wm, const figma_rectf_t & _rect, const RenderScissorInterface * _parent )
            {
                Viewport viewport;
                viewport.setRectangle(
                    mt::vec2f( _rect.x, _rect.y ),
                    mt::vec2f( _rect.x + _rect.w, _rect.y + _rect.h )
                );

                viewport.multiply( &m_viewportWM, _wm );

                if( _parent != nullptr )
                {
                    const Viewport & parentViewport = _parent->getScissorViewportWM();
                    m_viewportWM.clamp( parentViewport );
                }

                if( m_viewportWM.end.x > m_viewportWM.begin.x )
                {
                    m_viewportWM.begin.x = StdMath::floor( m_viewportWM.begin.x );
                    m_viewportWM.end.x = StdMath::ceil( m_viewportWM.end.x );
                }

                if( m_viewportWM.end.y > m_viewportWM.begin.y )
                {
                    m_viewportWM.begin.y = StdMath::floor( m_viewportWM.begin.y );
                    m_viewportWM.end.y = StdMath::ceil( m_viewportWM.end.y );
                }
            }

            const Viewport & getScissorViewportWM() const override
            {
                return m_viewportWM;
            }

        protected:
            Viewport m_viewportWM;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<FigmaRenderScissor, RenderScissorInterface> FigmaRenderScissorPtr;
        typedef Vector<const RenderScissorInterface *> VectorRenderScissorInterface;
        //////////////////////////////////////////////////////////////////////////
        static String makeString( figma_string_view_t _value )
        {
            if( _value.data == nullptr || _value.size == 0 )
            {
                return String();
            }

            return String( _value.data, _value.size );
        }
        //////////////////////////////////////////////////////////////////////////
        static String normalizePath( String _path )
        {
            for( Char & c : _path )
            {
                if( c == '\\' )
                {
                    c = '/';
                }
            }

            return _path;
        }
        //////////////////////////////////////////////////////////////////////////
        static String toLowerString( String _value )
        {
            StdAlgorithm::transform( _value.begin(), _value.end(), _value.begin(), []( unsigned char _ch )
            {
                return static_cast<Char>(StdCType::tolower( _ch ));
            } );

            return _value;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool hasExtension( const String & _path, const Char * _extension )
        {
            const size_t pathSize = _path.size();
            const size_t extensionSize = StdString::strlen( _extension );

            if( pathSize < extensionSize )
            {
                return false;
            }

            return StdString::strncmp( _path.c_str() + pathSize - extensionSize, _extension, extensionSize ) == 0;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool hasFontExtension( const String & _path )
        {
            String::size_type dot = _path.find_last_of( '.' );

            if( dot == String::npos )
            {
                return false;
            }

            String ext = _path.substr( dot + 1 );
            ext = Detail::toLowerString( ext );

            return ext == "ttf" || ext == "otf" || ext == "ttc";
        }
        //////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_PLATFORM_WINDOWS)
        static bool isDirectoryPath( const String & _path )
        {
            struct stat st;

            if( ::lstat( _path.c_str(), &st ) != 0 )
            {
                return false;
            }

            return S_ISDIR( st.st_mode ) != 0;
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        static void addUniqueDirectory( VectorString * const _directories, const Char * _directory )
        {
            if( _directory == nullptr || _directory[0] == '\0' )
            {
                return;
            }

            String directory = Detail::normalizePath( _directory );

            if( directory.empty() == true )
            {
                return;
            }

            if( directory[directory.size() - 1] != '/' )
            {
                directory += '/';
            }

            if( StdAlgorithm::find( _directories->begin(), _directories->end(), directory ) != _directories->end() )
            {
                return;
            }

            _directories->emplace_back( directory );
        }
        //////////////////////////////////////////////////////////////////////////
        static String normalizeFontName( String _value )
        {
            _value.erase( StdAlgorithm::remove_if( _value.begin(), _value.end(), []( Char _ch )
            {
                return _ch == ' ' || _ch == '-';
            } ), _value.end() );

            return Detail::toLowerString( _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static float clamp01( float _value )
        {
            return StdAlgorithm::max( 0.f, StdAlgorithm::min( 1.f, _value ) );
        }
        //////////////////////////////////////////////////////////////////////////
        static float linearToSRGB_( float _value )
        {
            const float value = Detail::clamp01( _value );

            if( value <= 0.0031308f )
            {
                return value * 12.92f;
            }

            return 1.055f * StdMath::pow( value, 1.f / 2.4f ) - 0.055f;
        }
        //////////////////////////////////////////////////////////////////////////
        struct HsvColor
        {
            float h = 0.f;
            float s = 0.f;
            float v = 0.f;
        };
        //////////////////////////////////////////////////////////////////////////
        static HsvColor rgbToHsv_( float _red, float _green, float _blue )
        {
            const float maxValue = StdAlgorithm::max( _red, StdAlgorithm::max( _green, _blue ) );
            const float minValue = StdAlgorithm::min( _red, StdAlgorithm::min( _green, _blue ) );
            const float delta = maxValue - minValue;

            HsvColor result;
            result.v = maxValue;
            result.s = maxValue > 0.f ? delta / maxValue : 0.f;

            if( delta <= 0.000001f )
            {
                return result;
            }

            if( maxValue == _red )
            {
                result.h = (_green - _blue) / delta;

                if( result.h < 0.f )
                {
                    result.h += 6.f;
                }
            }
            else if( maxValue == _green )
            {
                result.h = 2.f + (_blue - _red) / delta;
            }
            else
            {
                result.h = 4.f + (_red - _green) / delta;
            }

            result.h /= 6.f;

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void hsvToRgb_( const HsvColor & _hsv, float * const _red, float * const _green, float * const _blue )
        {
            if( _hsv.s <= 0.000001f )
            {
                *_red = _hsv.v;
                *_green = _hsv.v;
                *_blue = _hsv.v;
                return;
            }

            const float hue = StdMath::fmod( StdAlgorithm::max( 0.f, _hsv.h ), 1.f ) * 6.f;
            const int32_t sector = static_cast<int32_t>(StdMath::floor( hue ));
            const float fraction = hue - static_cast<float>(sector);
            const float p = _hsv.v * (1.f - _hsv.s);
            const float q = _hsv.v * (1.f - _hsv.s * fraction);
            const float t = _hsv.v * (1.f - _hsv.s * (1.f - fraction));

            switch( sector )
            {
            case 0:
                *_red = _hsv.v;
                *_green = t;
                *_blue = p;
                break;
            case 1:
                *_red = q;
                *_green = _hsv.v;
                *_blue = p;
                break;
            case 2:
                *_red = p;
                *_green = _hsv.v;
                *_blue = t;
                break;
            case 3:
                *_red = p;
                *_green = q;
                *_blue = _hsv.v;
                break;
            case 4:
                *_red = t;
                *_green = p;
                *_blue = _hsv.v;
                break;
            default:
                *_red = _hsv.v;
                *_green = p;
                *_blue = q;
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyExposure_( float * const _red, float * const _green, float * const _blue, float _exposure )
        {
            if( StdMath::fabs( _exposure ) <= 0.0001f )
            {
                return;
            }

            HsvColor hsv = Detail::rgbToHsv_( *_red, *_green, *_blue );
            const float value = StdAlgorithm::max( -1.f, StdAlgorithm::min( 1.f, _exposure ) );

            if( value > 0.f )
            {
                const float lift = value * 0.65f;
                hsv.v = hsv.v + (1.f - hsv.v) * lift;
                hsv.s *= 1.f - value * 0.22f;
            }
            else
            {
                hsv.v *= 1.f + value;
            }

            hsv.v = Detail::clamp01( hsv.v );
            hsv.s = Detail::clamp01( hsv.s );
            Detail::hsvToRgb_( hsv, _red, _green, _blue );
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyBrightness_( float * const _red, float * const _green, float * const _blue, float _brightness )
        {
            if( StdMath::fabs( _brightness ) <= 0.0001f )
            {
                return;
            }

            const float value = StdAlgorithm::max( -1.f, StdAlgorithm::min( 1.f, _brightness ) );
            *_red = Detail::clamp01( *_red + value );
            *_green = Detail::clamp01( *_green + value );
            *_blue = Detail::clamp01( *_blue + value );
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyContrast_( float * const _red, float * const _green, float * const _blue, float _contrast )
        {
            if( StdMath::fabs( _contrast ) <= 0.0001f )
            {
                return;
            }

            const float factor = StdAlgorithm::max( 0.f, 1.f + StdAlgorithm::max( -1.f, StdAlgorithm::min( 1.f, _contrast ) ) );
            *_red = Detail::clamp01( (*_red - 0.5f) * factor + 0.5f );
            *_green = Detail::clamp01( (*_green - 0.5f) * factor + 0.5f );
            *_blue = Detail::clamp01( (*_blue - 0.5f) * factor + 0.5f );
        }
        //////////////////////////////////////////////////////////////////////////
        static float applyMaskedLift_( float _component, float _amount, float _mask, float _scale )
        {
            const float value = StdAlgorithm::max( -1.f, StdAlgorithm::min( 1.f, _amount ) ) * _mask * _scale;

            if( value > 0.f )
            {
                return Detail::clamp01( _component + (1.f - _component) * value );
            }

            return Detail::clamp01( _component * (1.f + value) );
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyShadowsHighlights_( float * const _red, float * const _green, float * const _blue, float _shadows, float _highlights )
        {
            if( StdMath::fabs( _shadows ) <= 0.0001f && StdMath::fabs( _highlights ) <= 0.0001f )
            {
                return;
            }

            const float luminance = Detail::clamp01( *_red * 0.2126f + *_green * 0.7152f + *_blue * 0.0722f );
            const float shadowMask = (1.f - luminance) * (1.f - luminance);
            const float highlightMask = luminance * luminance;

            *_red = Detail::applyMaskedLift_( *_red, _shadows, shadowMask, 0.82f );
            *_green = Detail::applyMaskedLift_( *_green, _shadows, shadowMask, 0.82f );
            *_blue = Detail::applyMaskedLift_( *_blue, _shadows, shadowMask, 0.82f );

            *_red = Detail::applyMaskedLift_( *_red, _highlights, highlightMask, 0.62f );
            *_green = Detail::applyMaskedLift_( *_green, _highlights, highlightMask, 0.62f );
            *_blue = Detail::applyMaskedLift_( *_blue, _highlights, highlightMask, 0.62f );
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyTemperature_( float * const _red, float * const _green, float * const _blue, float _temperature )
        {
            if( StdMath::fabs( _temperature ) <= 0.0001f )
            {
                return;
            }

            const float value = StdAlgorithm::max( -1.f, StdAlgorithm::min( 1.f, _temperature ) );
            const float amount = StdMath::fabs( value );

            if( value > 0.f )
            {
                *_red = Detail::clamp01( *_red + (1.f - *_red) * amount * 0.18f );
                *_green = Detail::clamp01( *_green + (1.f - *_green) * amount * 0.035f );
                *_blue = Detail::clamp01( *_blue * (1.f - amount * 0.16f) );
            }
            else
            {
                *_red = Detail::clamp01( *_red * (1.f - amount * 0.16f) );
                *_green = Detail::clamp01( *_green + (1.f - *_green) * amount * 0.025f );
                *_blue = Detail::clamp01( *_blue + (1.f - *_blue) * amount * 0.18f );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyVibrance_( float * const _red, float * const _green, float * const _blue, float _vibrance )
        {
            if( StdMath::fabs( _vibrance ) <= 0.0001f )
            {
                return;
            }

            HsvColor hsv = Detail::rgbToHsv_( *_red, *_green, *_blue );
            const float value = StdAlgorithm::max( -1.f, StdAlgorithm::min( 1.f, _vibrance ) );

            if( value > 0.f )
            {
                const float lowSaturationWeight = 1.f - hsv.s;
                hsv.s += (1.f - hsv.s) * value * (0.55f + lowSaturationWeight * 0.45f);
            }
            else
            {
                hsv.s *= 1.f + value;
            }

            hsv.s = Detail::clamp01( hsv.s );
            Detail::hsvToRgb_( hsv, _red, _green, _blue );
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyTint_( float * const _red, float * const _green, float * const _blue, float _tint )
        {
            if( StdMath::fabs( _tint ) <= 0.0001f )
            {
                return;
            }

            const float amount = StdAlgorithm::min( 1.f, StdMath::fabs( _tint ) ) * 0.18f;
            const float targetRed = _tint >= 0.f ? 1.f : 0.f;
            const float targetGreen = _tint >= 0.f ? 0.f : 1.f;
            const float targetBlue = _tint >= 0.f ? 1.f : 0.f;

            *_red = Detail::clamp01( *_red * (1.f - amount) + targetRed * amount );
            *_green = Detail::clamp01( *_green * (1.f - amount) + targetGreen * amount );
            *_blue = Detail::clamp01( *_blue * (1.f - amount) + targetBlue * amount );
        }
        //////////////////////////////////////////////////////////////////////////
        static float imageFilterValue_( const figma_render_batch_desc_t & _batch, size_t _filterColorAdjustIndex, size_t _paintFilterIndex )
        {
            float value = 0.f;

            if( _batch.has_filter_color_adjust == FIGMA_TRUE && _filterColorAdjustIndex < 8 )
            {
                value += _batch.filter_color_adjust[_filterColorAdjustIndex];
            }

            if( _batch.has_paint_filter == FIGMA_TRUE && _paintFilterIndex < 10 )
            {
                value += _batch.paint_filter[_paintFilterIndex];
            }

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool hasImageFilter_( const figma_render_batch_desc_t & _batch )
        {
            const size_t indices[] = {0, 1, 2, 4, 6, 7};

            for( size_t index : indices )
            {
                if( StdMath::fabs( Detail::imageFilterValue_( _batch, index, index ) ) > 0.0001f )
                {
                    return true;
                }
            }

            if( _batch.has_paint_filter == FIGMA_TRUE )
            {
                if( StdMath::fabs( _batch.paint_filter[8] ) > 0.0001f )
                {
                    return true;
                }

                if( StdMath::fabs( _batch.paint_filter[9] ) > 0.0001f )
                {
                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyImageFilter_( uint8_t * const _pixels, size_t _pitch, uint32_t _width, uint32_t _height, const figma_render_batch_desc_t & _batch )
        {
            if( _pixels == nullptr || Detail::hasImageFilter_( _batch ) == false )
            {
                return;
            }

            const float tint = Detail::imageFilterValue_( _batch, 0, 0 );
            const float shadows = Detail::imageFilterValue_( _batch, 1, 1 );
            const float highlights = Detail::imageFilterValue_( _batch, 2, 2 );
            const float exposure = Detail::imageFilterValue_( _batch, 4, 4 );
            const float temperature = Detail::imageFilterValue_( _batch, 6, 6 );
            const float vibrance = Detail::imageFilterValue_( _batch, 7, 7 );
            const float contrast = _batch.has_paint_filter == FIGMA_TRUE ? _batch.paint_filter[8] : 0.f;
            const float brightness = _batch.has_paint_filter == FIGMA_TRUE ? _batch.paint_filter[9] : 0.f;

            for( uint32_t y = 0; y != _height; ++y )
            {
                uint8_t * row = _pixels + _pitch * y;

                for( uint32_t x = 0; x != _width; ++x )
                {
                    uint8_t * pixel = row + x * 4;
                    const float alpha = static_cast<float>(pixel[3]) / 255.f;

                    if( alpha <= 0.f )
                    {
                        continue;
                    }

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
                    float red = Detail::clamp01( static_cast<float>(pixel[0]) / 255.f / alpha );
                    float green = Detail::clamp01( static_cast<float>(pixel[1]) / 255.f / alpha );
                    float blue = Detail::clamp01( static_cast<float>(pixel[2]) / 255.f / alpha );
#else
                    float red = Detail::clamp01( static_cast<float>(pixel[2]) / 255.f / alpha );
                    float green = Detail::clamp01( static_cast<float>(pixel[1]) / 255.f / alpha );
                    float blue = Detail::clamp01( static_cast<float>(pixel[0]) / 255.f / alpha );
#endif

                    Detail::applyBrightness_( &red, &green, &blue, brightness );
                    Detail::applyExposure_( &red, &green, &blue, exposure );
                    Detail::applyShadowsHighlights_( &red, &green, &blue, shadows, highlights );
                    Detail::applyContrast_( &red, &green, &blue, contrast );
                    Detail::applyTemperature_( &red, &green, &blue, temperature );
                    Detail::applyTint_( &red, &green, &blue, tint );
                    Detail::applyVibrance_( &red, &green, &blue, vibrance );

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
                    pixel[0] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( red * alpha ) * 255.f ));
                    pixel[1] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( green * alpha ) * 255.f ));
                    pixel[2] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( blue * alpha ) * 255.f ));
#else
                    pixel[0] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( blue * alpha ) * 255.f ));
                    pixel[1] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( green * alpha ) * 255.f ));
                    pixel[2] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( red * alpha ) * 255.f ));
#endif
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void appendImageFilterSignature_( String * const _signature, const figma_render_batch_desc_t & _batch )
        {
            Char buffer[512];
            const int32_t size = MENGINE_SNPRINTF( buffer, 512,
                "|filter:%u:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g|paint:%u:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g:%.9g",
                _batch.has_filter_color_adjust == FIGMA_TRUE ? 1u : 0u,
                _batch.filter_color_adjust[0], _batch.filter_color_adjust[1], _batch.filter_color_adjust[2], _batch.filter_color_adjust[3],
                _batch.filter_color_adjust[4], _batch.filter_color_adjust[5], _batch.filter_color_adjust[6], _batch.filter_color_adjust[7],
                _batch.has_paint_filter == FIGMA_TRUE ? 1u : 0u,
                _batch.paint_filter[0], _batch.paint_filter[1], _batch.paint_filter[2], _batch.paint_filter[3], _batch.paint_filter[4],
                _batch.paint_filter[5], _batch.paint_filter[6], _batch.paint_filter[7], _batch.paint_filter[8], _batch.paint_filter[9]
            );

            if( size > 0 )
            {
                _signature->append( buffer, static_cast<size_t>(size) );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static float to26Dot6( FT_Pos _value )
        {
            return static_cast<float>(_value) / 64.f;
        }
        //////////////////////////////////////////////////////////////////////////
        static void appendSignature( String * const _signature, figma_string_view_t _value )
        {
            if( _value.data != nullptr && _value.size != 0 )
            {
                _signature->append( _value.data, _value.size );
            }

            _signature->push_back( '|' );
        }
        //////////////////////////////////////////////////////////////////////////
        static uint8_t premultiplyChannel_( uint8_t _channel, uint8_t _alpha )
        {
            return static_cast<uint8_t>((static_cast<uint32_t>(_channel) * static_cast<uint32_t>(_alpha) + 127u) / 255u);
        }
        //////////////////////////////////////////////////////////////////////////
        static void premultiplyPixels_( uint8_t * const _pixels, size_t _pitch, uint32_t _width, uint32_t _height )
        {
            if( _pixels == nullptr )
            {
                return;
            }

            for( uint32_t y = 0; y != _height; ++y )
            {
                uint8_t * row = _pixels + _pitch * y;

                for( uint32_t x = 0; x != _width; ++x )
                {
                    uint8_t * pixel = row + x * 4;
                    const uint8_t alpha = pixel[3];

                    if( alpha == 255 )
                    {
                        continue;
                    }

                    pixel[0] = Detail::premultiplyChannel_( pixel[0], alpha );
                    pixel[1] = Detail::premultiplyChannel_( pixel[1], alpha );
                    pixel[2] = Detail::premultiplyChannel_( pixel[2], alpha );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        class FigmaTextRasterizer
            : public Factorable
        {
            DECLARE_FACTORABLE( FigmaTextRasterizer );

        private:
            typedef Vector<figma_render_generated_text_line_desc_t> VectorTextLine;
            typedef Vector<char32_t> VectorCodepoint;
            typedef Map<String, FT_Face> MapFace;

        public:
            FigmaTextRasterizer()
                : m_library( nullptr )
            {
                if( FT_Init_FreeType( &m_library ) != FT_Err_Ok )
                {
                    m_library = nullptr;
                }

                this->setFontSearchPath( "" );
            }

            ~FigmaTextRasterizer() override
            {
                this->clearFaces_();

                if( m_library != nullptr )
                {
                    FT_Done_FreeType( m_library );
                    m_library = nullptr;
                }
            }

        public:
            void setFontSearchPath( const String & _path )
            {
                m_directories.clear();

                Detail::addUniqueDirectory( &m_directories, _path.c_str() );

                const Char * env = StdLib::getenv( "FIGMA_VIEWER_FONT_DIRS" );
                if( env != nullptr )
                {
                    String value = env;
                    String::size_type begin = 0;

                    for( ;; )
                    {
                        String::size_type end = value.find( ':', begin );
                        String part = end == String::npos ? value.substr( begin ) : value.substr( begin, end - begin );
                        Detail::addUniqueDirectory( &m_directories, part.c_str() );

                        if( end == String::npos )
                        {
                            break;
                        }

                        begin = end + 1;
                    }
                }

#if defined(MENGINE_PLATFORM_WINDOWS)
                Detail::addUniqueDirectory( &m_directories, "C:/Windows/Fonts/" );
#elif defined(MENGINE_PLATFORM_MACOS) || defined(MENGINE_PLATFORM_IOS)
                Detail::addUniqueDirectory( &m_directories, "/System/Library/Fonts/Supplemental/" );
                Detail::addUniqueDirectory( &m_directories, "/System/Library/Fonts/" );
                Detail::addUniqueDirectory( &m_directories, "/Library/Fonts/" );
#else
                Detail::addUniqueDirectory( &m_directories, "/usr/share/fonts/" );
                Detail::addUniqueDirectory( &m_directories, "/usr/local/share/fonts/" );
#endif

                this->clearFaces_();
            }

            bool makeTextSignature( const figma_render_list_t * _renderList, uint32_t _batchIndex, const figma_render_generated_texture_desc_t & _desc, float _rasterScale, String * const _signature ) const
            {
                if( _renderList == nullptr || _signature == nullptr )
                {
                    return false;
                }

                if( _desc.text.size == 0 || _desc.color.a <= 0.f || _desc.font_size <= 0.f || _desc.rect.w <= 0.f || _desc.rect.h <= 0.f )
                {
                    return false;
                }

                const float rasterScale = StdAlgorithm::max( 1.f, _rasterScale );
                const uint32_t width = static_cast<uint32_t>(StdMath::ceil( StdAlgorithm::max( 1.f, _desc.rect.w ) * rasterScale ));
                const uint32_t height = static_cast<uint32_t>(StdMath::ceil( StdAlgorithm::max( 1.f, _desc.rect.h ) * rasterScale ));

                if( width == 0 || height == 0 )
                {
                    return false;
                }

                VectorTextLine lines;
                lines.reserve( _desc.text_line_count );

                for( uint32_t lineIndex = 0; lineIndex != _desc.text_line_count; ++lineIndex )
                {
                    figma_render_generated_text_line_desc_t line{};

                    if( figma_render_list_get_generated_texture_text_line( _renderList, _batchIndex, lineIndex, &line ) != FIGMA_RESULT_OK )
                    {
                        return false;
                    }

                    lines.emplace_back( line );
                }

                *_signature = this->makeSignature_( _desc, lines, width, height );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool makeTextPixels( const figma_render_list_t * _renderList, uint32_t _batchIndex, const figma_render_generated_texture_desc_t & _desc, float _rasterScale, Data * const _pixels, uint32_t * const _width, uint32_t * const _height )
            {
                if( m_library == nullptr || _pixels == nullptr || _width == nullptr || _height == nullptr )
                {
                    return false;
                }

                if( _desc.text.size == 0 || _desc.color.a <= 0.f || _desc.font_size <= 0.f || _desc.rect.w <= 0.f || _desc.rect.h <= 0.f )
                {
                    return false;
                }

                FT_Face face = this->faceForDesc_( _desc );

                if( face == nullptr )
                {
                    return false;
                }

                const float rasterScale = StdAlgorithm::max( 1.f, _rasterScale );
                const FT_UInt pixelSize = static_cast<FT_UInt>(StdAlgorithm::max<long>( 1, StdMath::lround( _desc.font_size * rasterScale ) ));

                if( FT_Set_Pixel_Sizes( face, 0, pixelSize ) != FT_Err_Ok )
                {
                    return false;
                }

                const uint32_t width = static_cast<uint32_t>(StdMath::ceil( StdAlgorithm::max( 1.f, _desc.rect.w ) * rasterScale ));
                const uint32_t height = static_cast<uint32_t>(StdMath::ceil( StdAlgorithm::max( 1.f, _desc.rect.h ) * rasterScale ));

                if( width == 0 || height == 0 )
                {
                    return false;
                }

                VectorTextLine lines;
                lines.reserve( _desc.text_line_count );

                for( uint32_t lineIndex = 0; lineIndex != _desc.text_line_count; ++lineIndex )
                {
                    figma_render_generated_text_line_desc_t line{};

                    if( figma_render_list_get_generated_texture_text_line( _renderList, _batchIndex, lineIndex, &line ) != FIGMA_RESULT_OK )
                    {
                        return false;
                    }

                    lines.emplace_back( line );
                }

                _pixels->assign( width * height * 4, 0 );

                for( uint32_t lineIndex = 0; lineIndex != _desc.text_line_count; ++lineIndex )
                {
                    const figma_render_generated_text_line_desc_t & line = lines[lineIndex];
                    StringView lineText = this->textLineView_( _desc, lines, lineIndex );
                    VectorCodepoint codepoints = this->decodeUtf8_( lineText );

                    if( codepoints.empty() == true )
                    {
                        continue;
                    }

                    const float ascent = line.line_ascent > 0.f
                        ? line.line_ascent * rasterScale
                        : Detail::to26Dot6( face->size->metrics.ascender );
                    const float baselineX = line.x * rasterScale;
                    const float baselineY = line.y * rasterScale + ascent;
                    const float horizontalScale = this->horizontalScaleForLine_( face, codepoints, line.width * rasterScale );

                    this->rasterizeLine_( face, _desc, codepoints, baselineX, baselineY, horizontalScale, _pixels->data(), width, height, width * 4 );
                }

                *_width = width;
                *_height = height;

                return true;
            }

        protected:
            void clearFaces_()
            {
                for( const MapFace::value_type & value : m_faces )
                {
                    if( value.second != nullptr )
                    {
                        FT_Done_Face( value.second );
                    }
                }

                m_faces.clear();
            }

            String makeSignature_( const figma_render_generated_texture_desc_t & _desc, const VectorTextLine & _lines, uint32_t _width, uint32_t _height ) const
            {
                String signature;
                Detail::appendSignature( &signature, _desc.key );
                Detail::appendSignature( &signature, _desc.text );
                Detail::appendSignature( &signature, _desc.font_family );
                Detail::appendSignature( &signature, _desc.font_style );
                Detail::appendSignature( &signature, _desc.font_postscript_name );
                signature += Helper::stringFloat( _desc.font_size );
                signature.push_back( '|' );
                signature += Helper::stringFloat( _desc.color.r );
                signature.push_back( ',' );
                signature += Helper::stringFloat( _desc.color.g );
                signature.push_back( ',' );
                signature += Helper::stringFloat( _desc.color.b );
                signature.push_back( ',' );
                signature += Helper::stringFloat( _desc.color.a );
                signature.push_back( '|' );
                signature += Helper::stringFormat( "%u", _width );
                signature.push_back( 'x' );
                signature += Helper::stringFormat( "%u", _height );
                signature.push_back( '|' );

                for( const figma_render_generated_text_line_desc_t & line : _lines )
                {
                    Detail::appendSignature( &signature, line.text );
                    signature += Helper::stringFloat( line.x );
                    signature.push_back( ',' );
                    signature += Helper::stringFloat( line.y );
                    signature.push_back( ',' );
                    signature += Helper::stringFloat( line.width );
                    signature.push_back( ',' );
                    signature += Helper::stringFloat( line.line_ascent );
                    signature.push_back( '|' );
                }

                return signature;
            }

            FT_Face faceForDesc_( const figma_render_generated_texture_desc_t & _desc )
            {
                String key = this->fontKeyForDesc_( _desc );

                if( key.empty() == true )
                {
                    return nullptr;
                }

                MapFace::iterator it_found = m_faces.find( key );

                if( it_found != m_faces.end() )
                {
                    return it_found->second;
                }

                FT_Face face = this->openFaceForDesc_( _desc );
                m_faces.emplace( key, face );

                return face;
            }

            String fontKeyForDesc_( const figma_render_generated_texture_desc_t & _desc ) const
            {
                if( _desc.font_postscript_name.size != 0 )
                {
                    return Detail::makeString( _desc.font_postscript_name );
                }

                String key = Detail::makeString( _desc.font_family );

                if( key.empty() == false && _desc.font_style.size != 0 )
                {
                    key += "-";
                    key += Detail::makeString( _desc.font_style );
                }

                return key;
            }

            bool faceMatches_( FT_Face _face, const String & _postscriptName, const String & _familyName, const String & _styleName ) const
            {
                const char * postscriptName = FT_Get_Postscript_Name( _face );

                if( postscriptName != nullptr && _postscriptName.empty() == false && Detail::normalizeFontName( postscriptName ) == Detail::normalizeFontName( _postscriptName ) )
                {
                    return true;
                }

                if( _face->family_name != nullptr && _familyName.empty() == false && Detail::normalizeFontName( _face->family_name ) == Detail::normalizeFontName( _familyName ) )
                {
                    if( _styleName.empty() == true || _face->style_name == nullptr || Detail::normalizeFontName( _face->style_name ) == Detail::normalizeFontName( _styleName ) )
                    {
                        return true;
                    }
                }

                return false;
            }

            FT_Face openFaceAtPath_( const String & _path, const String & _postscriptName, const String & _familyName, const String & _styleName ) const
            {
                for( FT_Long faceIndex = 0; faceIndex != 32; ++faceIndex )
                {
                    FT_Face face = nullptr;

                    if( FT_New_Face( m_library, _path.c_str(), faceIndex, &face ) != FT_Err_Ok )
                    {
                        return nullptr;
                    }

                    const FT_Long faceCount = face->num_faces;

                    if( this->faceMatches_( face, _postscriptName, _familyName, _styleName ) == true )
                    {
                        if( FT_Select_Charmap( face, FT_ENCODING_UNICODE ) != FT_Err_Ok )
                        {
                            FT_Done_Face( face );
                            return nullptr;
                        }

                        return face;
                    }

                    FT_Done_Face( face );

                    if( faceIndex + 1 >= faceCount )
                    {
                        break;
                    }
                }

                return nullptr;
            }

#if !defined(MENGINE_PLATFORM_WINDOWS)
            FT_Face openMatchingFaceInDirectoryNative_( const String & _directory, const String & _postscriptName, const String & _familyName, const String & _styleName ) const
            {
                DIR * dir = ::opendir( _directory.c_str() );

                if( dir == nullptr )
                {
                    return nullptr;
                }

                FT_Face foundFace = nullptr;

                for( ;; )
                {
                    struct dirent * entry = ::readdir( dir );

                    if( entry == nullptr )
                    {
                        break;
                    }

                    const Char * name = entry->d_name;

                    if( StdString::strcmp( name, "." ) == 0 || StdString::strcmp( name, ".." ) == 0 )
                    {
                        continue;
                    }

                    String fullPath = _directory;
                    fullPath += name;

                    if( Detail::isDirectoryPath( fullPath ) == true )
                    {
                        fullPath += '/';

                        foundFace = this->openMatchingFaceInDirectoryNative_( fullPath, _postscriptName, _familyName, _styleName );

                        if( foundFace != nullptr )
                        {
                            break;
                        }

                        continue;
                    }

                    if( Detail::hasFontExtension( name ) == false )
                    {
                        continue;
                    }

                    foundFace = this->openFaceAtPath_( fullPath, _postscriptName, _familyName, _styleName );

                    if( foundFace != nullptr )
                    {
                        break;
                    }
                }

                ::closedir( dir );

                return foundFace;
            }
#endif

            FT_Face openMatchingFaceInDirectory_( const String & _directory, const String & _postscriptName, const String & _familyName, const String & _styleName ) const
            {
#if !defined(MENGINE_PLATFORM_WINDOWS)
                return this->openMatchingFaceInDirectoryNative_( _directory, _postscriptName, _familyName, _styleName );
#else
                if( FILE_SYSTEM()->existDirectory( "", _directory.c_str() ) == false )
                {
                    return nullptr;
                }

                FT_Face foundFace = nullptr;

                FILE_SYSTEM()->findFiles( _directory.c_str(), "", "*", [this, &_directory, &_postscriptName, &_familyName, &_styleName, &foundFace]( const FilePath & _filePath )
                {
                    String relativePath = _filePath.c_str();

                    if( Detail::hasFontExtension( relativePath ) == false )
                    {
                        return true;
                    }

                    String fullPath = _directory;
                    fullPath += relativePath;

                    foundFace = this->openFaceAtPath_( fullPath, _postscriptName, _familyName, _styleName );

                    return foundFace == nullptr;
                } );

                return foundFace;
#endif
            }

            FT_Face openFaceForDesc_( const figma_render_generated_texture_desc_t & _desc ) const
            {
                const String postscriptName = Detail::makeString( _desc.font_postscript_name );
                const String familyName = Detail::makeString( _desc.font_family );
                const String styleName = Detail::makeString( _desc.font_style );

                if( postscriptName.empty() == true && familyName.empty() == true )
                {
                    return nullptr;
                }

                for( const String & directory : m_directories )
                {
                    FT_Face face = this->openMatchingFaceInDirectory_( directory, postscriptName, familyName, styleName );

                    if( face != nullptr )
                    {
                        return face;
                    }
                }

                LOGGER_WARNING( "figma missing font postscript '%s' family '%s' style '%s'"
                    , postscriptName.c_str()
                    , familyName.c_str()
                    , styleName.c_str()
                );

                return nullptr;
            }

            VectorCodepoint decodeUtf8_( StringView _text ) const
            {
                VectorCodepoint result;
                result.reserve( _text.size() );

                const unsigned char * cursor = reinterpret_cast<const unsigned char *>(_text.data());
                const unsigned char * const end = cursor + _text.size();

                while( cursor < end )
                {
                    const unsigned char c = *cursor++;

                    if( c < 0x80 )
                    {
                        result.emplace_back( static_cast<char32_t>(c) );
                        continue;
                    }

                    char32_t codepoint = U'?';
                    int continuation = 0;

                    if( (c & 0xE0) == 0xC0 )
                    {
                        codepoint = c & 0x1F;
                        continuation = 1;
                    }
                    else if( (c & 0xF0) == 0xE0 )
                    {
                        codepoint = c & 0x0F;
                        continuation = 2;
                    }
                    else if( (c & 0xF8) == 0xF0 )
                    {
                        codepoint = c & 0x07;
                        continuation = 3;
                    }

                    bool valid = continuation != 0 && cursor + continuation <= end;

                    for( int index = 0; valid == true && index != continuation; ++index )
                    {
                        const unsigned char part = *cursor++;

                        if( (part & 0xC0) != 0x80 )
                        {
                            valid = false;
                            break;
                        }

                        codepoint = (codepoint << 6) | (part & 0x3F);
                    }

                    result.emplace_back( valid == true ? codepoint : U'?' );
                }

                return result;
            }

            StringView sourceTextView_( const figma_render_generated_texture_desc_t & _desc, const figma_render_generated_text_line_desc_t & _line ) const
            {
                const figma_string_view_t source = _line.text.size != 0 ? _line.text : _desc.text;

                if( source.data == nullptr || source.size == 0 )
                {
                    return {};
                }

                return StringView( source.data, source.size );
            }

            StringView trimTrailingWhitespace_( StringView _value ) const
            {
                while( _value.empty() == false && (_value.back() == ' ' || _value.back() == '\t' || _value.back() == '\r' || _value.back() == '\n') )
                {
                    _value.remove_suffix( 1 );
                }

                return _value;
            }

            StringView explicitLineSegment_( StringView _value, size_t _lineIndex ) const
            {
                size_t begin = 0;
                size_t index = 0;

                while( begin <= _value.size() )
                {
                    const size_t end = _value.find_first_of( "\r\n", begin );

                    if( index == _lineIndex )
                    {
                        return end == StringView::npos ? _value.substr( begin ) : _value.substr( begin, end - begin );
                    }

                    if( end == StringView::npos )
                    {
                        break;
                    }

                    begin = end + 1;

                    if( _value[end] == '\r' && begin < _value.size() && _value[begin] == '\n' )
                    {
                        ++begin;
                    }

                    ++index;
                }

                return {};
            }

            StringView textLineView_( const figma_render_generated_texture_desc_t & _desc, const VectorTextLine & _lines, size_t _lineIndex ) const
            {
                const figma_render_generated_text_line_desc_t & line = _lines[_lineIndex];
                StringView view = this->sourceTextView_( _desc, line );

                if( view.find_first_of( "\r\n" ) == StringView::npos )
                {
                    if( _lineIndex + 1 < _lines.size() )
                    {
                        StringView nextView = this->sourceTextView_( _desc, _lines[_lineIndex + 1] );

                        if( nextView.find_first_of( "\r\n" ) != StringView::npos )
                        {
                            nextView = this->explicitLineSegment_( nextView, _lineIndex + 1 );
                        }

                        nextView = this->trimTrailingWhitespace_( nextView );

                        const size_t suffix = nextView.empty() == false ? view.rfind( nextView ) : StringView::npos;

                        if( suffix != StringView::npos && suffix > 0 )
                        {
                            return this->trimTrailingWhitespace_( view.substr( 0, suffix ) );
                        }
                    }

                    return this->trimTrailingWhitespace_( view );
                }

                return this->trimTrailingWhitespace_( this->explicitLineSegment_( view, _lineIndex ) );
            }

            FT_UInt glyphIndexForCodepoint_( FT_Face _face, char32_t _codepoint ) const
            {
                FT_UInt glyphIndex = FT_Get_Char_Index( _face, static_cast<FT_ULong>(_codepoint) );

                if( glyphIndex == 0 )
                {
                    glyphIndex = FT_Get_Char_Index( _face, static_cast<FT_ULong>(U'?') );
                }

                return glyphIndex;
            }

            float measureLineAdvance_( FT_Face _face, const VectorCodepoint & _codepoints ) const
            {
                float advance = 0.f;
                FT_UInt previousGlyph = 0;

                for( char32_t codepoint : _codepoints )
                {
                    if( codepoint == U'\n' || codepoint == U'\r' )
                    {
                        continue;
                    }

                    const FT_UInt glyphIndex = this->glyphIndexForCodepoint_( _face, codepoint );

                    if( previousGlyph != 0 && glyphIndex != 0 && FT_HAS_KERNING( _face ) != 0 )
                    {
                        FT_Vector kerning;

                        if( FT_Get_Kerning( _face, previousGlyph, glyphIndex, FT_KERNING_DEFAULT, &kerning ) == FT_Err_Ok )
                        {
                            advance += Detail::to26Dot6( kerning.x );
                        }
                    }

                    if( FT_Load_Glyph( _face, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR ) == FT_Err_Ok )
                    {
                        advance += Detail::to26Dot6( _face->glyph->metrics.horiAdvance );
                    }

                    previousGlyph = glyphIndex;
                }

                return advance;
            }

            float horizontalScaleForLine_( FT_Face _face, const VectorCodepoint & _codepoints, float _targetWidth ) const
            {
                const float measuredWidth = this->measureLineAdvance_( _face, _codepoints );

                if( measuredWidth <= 0.001f || _targetWidth <= 0.001f )
                {
                    return 1.f;
                }

                const float scale = _targetWidth / measuredWidth;

                if( scale < 1.f && scale > 0.94f )
                {
                    return 1.f;
                }

                return StdAlgorithm::max( 0.25f, StdAlgorithm::min( 4.f, scale ) );
            }

            uint8_t coverageAt_( const FT_Bitmap & _bitmap, int32_t _x, int32_t _y ) const
            {
                if( _bitmap.buffer == nullptr || _x < 0 || _y < 0 || _x >= static_cast<int32_t>(_bitmap.width) || _y >= static_cast<int32_t>(_bitmap.rows) )
                {
                    return 0;
                }

                const int32_t pitch = static_cast<int32_t>(_bitmap.pitch);
                const unsigned char * sourceRow = pitch >= 0
                    ? _bitmap.buffer + _y * pitch
                    : _bitmap.buffer + (static_cast<int32_t>(_bitmap.rows) - 1 - _y) * -pitch;

                switch( _bitmap.pixel_mode )
                {
                case FT_PIXEL_MODE_MONO:
                    return (sourceRow[_x >> 3] & (0x80 >> (_x & 7))) != 0 ? 255 : 0;
                case FT_PIXEL_MODE_GRAY:
                    return sourceRow[_x];
                case FT_PIXEL_MODE_GRAY2:
                    {
                        const unsigned char packed = sourceRow[_x >> 2];
                        return static_cast<uint8_t>(((packed >> ((3 - (_x & 3)) * 2)) & 0x03) * 85);
                    }break;
                case FT_PIXEL_MODE_GRAY4:
                    {
                        const unsigned char packed = sourceRow[_x >> 1];
                        return static_cast<uint8_t>(((packed >> ((1 - (_x & 1)) * 4)) & 0x0F) * 17);
                    }break;
                default:
                    break;
                }

                return 0;
            }

            void readTargetPixel_( const uint8_t * _target, float * const _red, float * const _green, float * const _blue, float * const _alpha ) const
            {
#if defined(MENGINE_RENDER_TEXTURE_RGBA)
                *_red = static_cast<float>(_target[0]) / 255.f;
                *_green = static_cast<float>(_target[1]) / 255.f;
                *_blue = static_cast<float>(_target[2]) / 255.f;
                *_alpha = static_cast<float>(_target[3]) / 255.f;
#else
                *_blue = static_cast<float>(_target[0]) / 255.f;
                *_green = static_cast<float>(_target[1]) / 255.f;
                *_red = static_cast<float>(_target[2]) / 255.f;
                *_alpha = static_cast<float>(_target[3]) / 255.f;
#endif
            }

            void writeTargetPixel_( uint8_t * const _target, float _red, float _green, float _blue, float _alpha ) const
            {
#if defined(MENGINE_RENDER_TEXTURE_RGBA)
                _target[0] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _red ) * 255.f ));
                _target[1] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _green ) * 255.f ));
                _target[2] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _blue ) * 255.f ));
                _target[3] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _alpha ) * 255.f ));
#else
                _target[0] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _blue ) * 255.f ));
                _target[1] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _green ) * 255.f ));
                _target[2] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _red ) * 255.f ));
                _target[3] = static_cast<uint8_t>(StdMath::lround( Detail::clamp01( _alpha ) * 255.f ));
#endif
            }

            void blendStraightPixel_( uint8_t * const _target, float _red, float _green, float _blue, float _alpha ) const
            {
                const float sourceAlpha = Detail::clamp01( _alpha );

                if( sourceAlpha <= 0.f )
                {
                    return;
                }

                float destinationRed;
                float destinationGreen;
                float destinationBlue;
                float destinationAlpha;
                this->readTargetPixel_( _target, &destinationRed, &destinationGreen, &destinationBlue, &destinationAlpha );

                const float inverseAlpha = 1.f - sourceAlpha;
                const float outputAlpha = sourceAlpha + destinationAlpha * inverseAlpha;

                float outputRed = 0.f;
                float outputGreen = 0.f;
                float outputBlue = 0.f;

                if( outputAlpha > 0.f )
                {
                    outputRed = (_red * sourceAlpha + destinationRed * destinationAlpha * inverseAlpha) / outputAlpha;
                    outputGreen = (_green * sourceAlpha + destinationGreen * destinationAlpha * inverseAlpha) / outputAlpha;
                    outputBlue = (_blue * sourceAlpha + destinationBlue * destinationAlpha * inverseAlpha) / outputAlpha;
                }

                this->writeTargetPixel_( _target, outputRed, outputGreen, outputBlue, outputAlpha );
            }

            void blendGlyphBitmapPixel_( const FT_Bitmap & _bitmap, int32_t _sourceX, int32_t _sourceY, uint8_t * const _target, const figma_render_generated_texture_desc_t & _desc ) const
            {
                const float commandAlpha = Detail::clamp01( _desc.color.a );

                if( commandAlpha <= 0.f )
                {
                    return;
                }

                if( _bitmap.pixel_mode == FT_PIXEL_MODE_BGRA )
                {
                    const int32_t pitch = static_cast<int32_t>(_bitmap.pitch);
                    const unsigned char * sourceRow = pitch >= 0
                        ? _bitmap.buffer + _sourceY * pitch
                        : _bitmap.buffer + (static_cast<int32_t>(_bitmap.rows) - 1 - _sourceY) * -pitch;
                    const unsigned char * pixel = sourceRow + _sourceX * 4;
                    const float sourceAlpha = static_cast<float>(pixel[3]) / 255.f * commandAlpha;

                    if( sourceAlpha <= 0.f )
                    {
                        return;
                    }

                    float red = static_cast<float>(pixel[2]) / 255.f;
                    float green = static_cast<float>(pixel[1]) / 255.f;
                    float blue = static_cast<float>(pixel[0]) / 255.f;

                    if( pixel[3] != 0 && pixel[3] != 255 )
                    {
                        const float invAlpha = 255.f / static_cast<float>(pixel[3]);
                        red = StdAlgorithm::min( 1.f, red * invAlpha );
                        green = StdAlgorithm::min( 1.f, green * invAlpha );
                        blue = StdAlgorithm::min( 1.f, blue * invAlpha );
                    }

                    this->blendStraightPixel_( _target, red, green, blue, sourceAlpha );

                    return;
                }

                const uint8_t coverage = this->coverageAt_( _bitmap, _sourceX, _sourceY );
                const float linearCoverage = static_cast<float>(coverage) / 255.f;
                const float sourceAlpha = Detail::linearToSRGB_( linearCoverage ) * commandAlpha;

                this->blendStraightPixel_( _target, Detail::clamp01( _desc.color.r ), Detail::clamp01( _desc.color.g ), Detail::clamp01( _desc.color.b ), sourceAlpha );
            }

            void rasterizeLine_( FT_Face _face, const figma_render_generated_texture_desc_t & _desc, const VectorCodepoint & _codepoints, float _baselineX, float _baselineY, float _horizontalScale, uint8_t * const _pixels, uint32_t _width, uint32_t _height, uint32_t _stride ) const
            {
                float penX = _baselineX;
                FT_UInt previousGlyph = 0;
                const float horizontalScale = StdAlgorithm::max( 0.001f, _horizontalScale );

                for( char32_t codepoint : _codepoints )
                {
                    if( codepoint == U'\n' || codepoint == U'\r' )
                    {
                        continue;
                    }

                    const FT_UInt glyphIndex = this->glyphIndexForCodepoint_( _face, codepoint );

                    if( previousGlyph != 0 && glyphIndex != 0 && FT_HAS_KERNING( _face ) != 0 )
                    {
                        FT_Vector kerning;

                        if( FT_Get_Kerning( _face, previousGlyph, glyphIndex, FT_KERNING_DEFAULT, &kerning ) == FT_Err_Ok )
                        {
                            penX += Detail::to26Dot6( kerning.x );
                        }
                    }

                    if( FT_Load_Glyph( _face, glyphIndex, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR ) != FT_Err_Ok )
                    {
                        previousGlyph = glyphIndex;
                        continue;
                    }

                    FT_GlyphSlot glyph = _face->glyph;
                    const FT_Bitmap & bitmap = glyph->bitmap;

                    if( bitmap.width != 0 && bitmap.rows != 0 && bitmap.buffer != nullptr )
                    {
                        const float glyphX = _baselineX + (penX + Detail::to26Dot6( glyph->metrics.horiBearingX ) - _baselineX) * horizontalScale;
                        const float glyphY = _baselineY - Detail::to26Dot6( glyph->metrics.horiBearingY );
                        const int32_t destinationX = static_cast<int32_t>(StdMath::lround( glyphX ));
                        const int32_t destinationY = static_cast<int32_t>(StdMath::lround( glyphY ));
                        const int32_t destinationWidth = StdAlgorithm::max<int32_t>( 1, static_cast<int32_t>(StdMath::lround( static_cast<float>(bitmap.width) * horizontalScale )) );
                        const int32_t destinationHeight = static_cast<int32_t>(bitmap.rows);

                        for( int32_t scaledY = 0; scaledY != destinationHeight; ++scaledY )
                        {
                            const int32_t sourceY = scaledY;
                            const int32_t targetY = destinationY + scaledY;

                            if( targetY < 0 || targetY >= static_cast<int32_t>(_height) )
                            {
                                continue;
                            }

                            uint8_t * targetRow = _pixels + static_cast<uint32_t>(targetY) * _stride;

                            for( int32_t scaledX = 0; scaledX != destinationWidth; ++scaledX )
                            {
                                const int32_t sourceX = StdAlgorithm::min<int32_t>( static_cast<int32_t>(bitmap.width) - 1, StdAlgorithm::max<int32_t>( 0, static_cast<int32_t>(StdMath::floor( (static_cast<float>(scaledX) + 0.5f) / horizontalScale )) ) );
                                const int32_t targetX = destinationX + scaledX;

                                if( targetX < 0 || targetX >= static_cast<int32_t>(_width) )
                                {
                                    continue;
                                }

                                this->blendGlyphBitmapPixel_( bitmap, sourceX, sourceY, targetRow + static_cast<uint32_t>(targetX) * 4, _desc );
                            }
                        }
                    }

                    penX += Detail::to26Dot6( glyph->metrics.horiAdvance );
                    previousGlyph = glyphIndex;
                }
            }

        protected:
            FT_Library m_library;
            VectorString m_directories;
            MapFace m_faces;
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * getFigmaResultMessage( figma_result_t _result )
        {
            switch( _result )
            {
            case FIGMA_RESULT_OK:
                return "Ok";
            case FIGMA_RESULT_INVALID_ARGUMENT:
                return "InvalidArgument";
            case FIGMA_RESULT_OUT_OF_MEMORY:
                return "OutOfMemory";
            case FIGMA_RESULT_IO_FAILED:
                return "IoFailed";
            case FIGMA_RESULT_PARSE_FAILED:
                return "ParseFailed";
            case FIGMA_RESULT_UNSUPPORTED_FORMAT:
                return "UnsupportedFormat";
            case FIGMA_RESULT_MISSING_ENTRY:
                return "MissingEntry";
            case FIGMA_RESULT_NOT_FOUND:
                return "NotFound";
            case FIGMA_RESULT_INVALID_STATE:
                return "InvalidState";
            case FIGMA_RESULT_VERSION_MISMATCH:
                return "VersionMismatch";
            }

            return "Unknown";
        }
        //////////////////////////////////////////////////////////////////////////
        static EMaterialBlendMode getFigmaMaterialBlendMode( figma_render_blend_mode_t _blendMode )
        {
            switch( _blendMode )
            {
            case FIGMA_RENDER_BLEND_SCREEN:
                return EMB_SCREEN;
            case FIGMA_RENDER_BLEND_MULTIPLY:
                return EMB_MULTIPLY;
            case FIGMA_RENDER_BLEND_COLOR_DODGE:
            case FIGMA_RENDER_BLEND_LIGHTEN:
                return EMB_ADD;
            case FIGMA_RENDER_BLEND_PASS_THROUGH:
            case FIGMA_RENDER_BLEND_NORMAL:
            default:
                return EMB_NORMAL;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static figma_pointer_button_t getFigmaPointerButton( uint32_t _button )
        {
            switch( _button )
            {
            case MC_LBUTTON:
                return FIGMA_POINTER_BUTTON_LEFT;
            case MC_MBUTTON:
                return FIGMA_POINTER_BUTTON_MIDDLE;
            case MC_RBUTTON:
                return FIGMA_POINTER_BUTTON_RIGHT;
            default:
                return FIGMA_POINTER_BUTTON_OTHER;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static figma_input_modifier_flags_t getFigmaInputModifiers( const InputSpecialData & _special )
        {
            figma_input_modifier_flags_t modifiers = FIGMA_INPUT_MODIFIER_NONE;

            if( _special.isShift == true )
            {
                modifiers |= FIGMA_INPUT_MODIFIER_SHIFT;
            }

            if( _special.isControl == true )
            {
                modifiers |= FIGMA_INPUT_MODIFIER_CONTROL;
            }

            if( _special.isAlt == true )
            {
                modifiers |= FIGMA_INPUT_MODIFIER_ALT;
            }

            if( _special.isSpecial == true )
            {
                modifiers |= FIGMA_INPUT_MODIFIER_COMMAND;
            }

            return modifiers;
        }
        //////////////////////////////////////////////////////////////////////////
        static void transformFigmaVertex( RenderVertex2D * const _vertex, const figma_render_vertex_t & _figmaVertex, const mt::mat4f & _wm, const RenderTextureInterfacePtr & _texture, ColorValue_ARGB _color )
        {
            mt::vec3f position( _figmaVertex.x, _figmaVertex.y, 0.f );
            mt::mul_v3_v3_m4( &_vertex->position, position, _wm );

            _vertex->color = _color;

            if( _texture != nullptr )
            {
                mt::uv4_quad_point( _vertex->uv + 0, _texture->getUV(), mt::vec2f( _figmaVertex.u, _figmaVertex.v ) );
            }
            else
            {
                _vertex->uv[0].x = _figmaVertex.u;
                _vertex->uv[0].y = _figmaVertex.v;
            }

            _vertex->uv[1].x = 0.f;
            _vertex->uv[1].y = 0.f;
        }
        //////////////////////////////////////////////////////////////////////////
        static void copyRGBPixelsToTexture_( const uint8_t * _source, size_t _sourcePitch, uint8_t * _target, size_t _targetPitch, uint32_t _width, uint32_t _height )
        {
            for( uint32_t y = 0; y != _height; ++y )
            {
                const uint8_t * sourceRow = _source + _sourcePitch * y;
                uint8_t * targetRow = _target + _targetPitch * y;

                for( uint32_t x = 0; x != _width; ++x )
                {
                    const uint8_t * sourcePixel = sourceRow + x * 3;
                    uint8_t * targetPixel = targetRow + x * 4;

#if defined(MENGINE_RENDER_TEXTURE_RGBA)
                    targetPixel[0] = sourcePixel[0];
                    targetPixel[1] = sourcePixel[1];
                    targetPixel[2] = sourcePixel[2];
#else
                    targetPixel[0] = sourcePixel[2];
                    targetPixel[1] = sourcePixel[1];
                    targetPixel[2] = sourcePixel[0];
#endif
                    targetPixel[3] = 255;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Figma::Figma()
        : m_player( nullptr )
        , m_viewportSize( 1024.f, 768.f )
        , m_viewportScale( 1.f )
        , m_playbackRate( 1.f )
        , m_textRasterizer( Helper::makeFactorableUnique<Detail::FigmaTextRasterizer>( MENGINE_DOCUMENT_FACTORABLE ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Figma::~Figma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::setResourceFigma( const ResourcePtr & _resource )
    {
        ResourceFigmaPtr resourceFigma = ResourceFigmaPtr::from( _resource );

        if( m_resourceFigma == resourceFigma )
        {
            return;
        }

        this->recompile( [this, &resourceFigma]()
        {
            m_resourceFigma = resourceFigma;

            if( m_resourceFigma == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & Figma::getResourceFigma() const
    {
        return m_resourceFigma;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::setViewportSize( const mt::vec2f & _size )
    {
        if( m_viewportSize == _size )
        {
            return;
        }

        m_viewportSize = _size;

        this->updatePlayerViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Figma::getViewportSize() const
    {
        return m_viewportSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::setViewportScale( float _scale )
    {
        if( m_viewportScale == _scale )
        {
            return;
        }

        m_viewportScale = _scale;

        this->clearTextureCache_();
        this->updatePlayerViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    float Figma::getViewportScale() const
    {
        return m_viewportScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::updatePlayerViewport_()
    {
        if( m_player == nullptr )
        {
            return true;
        }

        figma_viewport_desc_t viewport{};
        viewport.width = m_viewportSize.x;
        viewport.height = m_viewportSize.y;
        viewport.scale = m_viewportScale;

        figma_result_t result = figma_player_set_viewport( m_player, &viewport );

        if( result != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "figma '%s' resource '%s' invalid viewport result '%s'"
                , this->getName().c_str()
                , m_resourceFigma->getName().c_str()
                , Detail::getFigmaResultMessage( result )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::setStartFrameId( const String & _startFrameId )
    {
        if( m_startFrameId == _startFrameId )
        {
            return;
        }

        this->recompile( [this, &_startFrameId]()
        {
            m_startFrameId = _startFrameId;

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const String & Figma::getStartFrameId() const
    {
        return m_startFrameId;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::setFontSearchPath( const String & _fontSearchPath )
    {
        if( m_fontSearchPath == _fontSearchPath )
        {
            return;
        }

        m_fontSearchPath = _fontSearchPath;

        if( m_textRasterizer != nullptr )
        {
            m_textRasterizer->setFontSearchPath( m_fontSearchPath );
        }

        this->clearTextureCache_();
    }
    //////////////////////////////////////////////////////////////////////////
    const String & Figma::getFontSearchPath() const
    {
        return m_fontSearchPath;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::setPlaybackRate( float _playbackRate )
    {
        if( _playbackRate <= 0.f )
        {
            m_playbackRate = 1.f;
            return;
        }

        m_playbackRate = _playbackRate;
    }
    //////////////////////////////////////////////////////////////////////////
    float Figma::getPlaybackRate() const
    {
        return m_playbackRate;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::replay()
    {
        if( m_player == nullptr )
        {
            return false;
        }

        figma_result_t result = figma_player_restart( m_player );

        if( result != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "figma '%s' resource '%s' invalid replay result '%s'"
                , this->getName().c_str()
                , m_resourceFigma->getName().c_str()
                , Detail::getFigmaResultMessage( result )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointerMove( float _x, float _y )
    {
        return this->inputPointer_( FIGMA_POINTER_EVENT_MOVE, 0, _x, _y, FIGMA_POINTER_BUTTON_NONE, FIGMA_INPUT_MODIFIER_NONE, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointerDown( float _x, float _y, uint32_t _button )
    {
        return this->inputPointer_( FIGMA_POINTER_EVENT_DOWN, 0, _x, _y, Detail::getFigmaPointerButton( _button ), FIGMA_INPUT_MODIFIER_NONE, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointerUp( float _x, float _y, uint32_t _button )
    {
        return this->inputPointer_( FIGMA_POINTER_EVENT_UP, 0, _x, _y, Detail::getFigmaPointerButton( _button ), FIGMA_INPUT_MODIFIER_NONE, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointerCancel( float _x, float _y )
    {
        return this->inputPointer_( FIGMA_POINTER_EVENT_CANCEL, 0, _x, _y, FIGMA_POINTER_BUTTON_NONE, FIGMA_INPUT_MODIFIER_NONE, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputKeyDown( uint32_t _keyCode )
    {
        return this->inputKey_( FIGMA_KEY_EVENT_DOWN, _keyCode, FIGMA_INPUT_MODIFIER_NONE, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputKeyUp( uint32_t _keyCode )
    {
        return this->inputKey_( FIGMA_KEY_EVENT_UP, _keyCode, FIGMA_INPUT_MODIFIER_NONE, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::setBindingText( const String & _key, const String & _value )
    {
        FigmaBindingValue value;
        value.type = EFigmaBindingValueType::Text;
        value.stringValue = _value;

        return this->setBindingValue( _key, value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::setBindingNumber( const String & _key, double _value )
    {
        FigmaBindingValue value;
        value.type = EFigmaBindingValueType::Number;
        value.numberValue = _value;

        return this->setBindingValue( _key, value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::setBindingVisible( const String & _key, bool _value )
    {
        FigmaBindingValue value;
        value.type = EFigmaBindingValueType::Boolean;
        value.boolValue = _value;

        m_bindingValues[_key] = value;

        if( m_player == nullptr )
        {
            return true;
        }

        const figma_string_view_t key{_key.data(), _key.size()};
        const figma_result_t result = figma_player_set_visible( m_player, key, _value == true ? FIGMA_TRUE : FIGMA_FALSE );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::setBindingEnabled( const String & _key, bool _value )
    {
        FigmaBindingValue value;
        value.type = EFigmaBindingValueType::Boolean;
        value.boolValue = _value;

        m_bindingValues[_key] = value;

        if( m_player == nullptr )
        {
            return true;
        }

        const figma_string_view_t key{_key.data(), _key.size()};
        const figma_result_t result = figma_player_set_enabled( m_player, key, _value == true ? FIGMA_TRUE : FIGMA_FALSE );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::setBindingImage( const String & _key, const String & _assetId )
    {
        FigmaBindingValue value;
        value.type = EFigmaBindingValueType::Image;
        value.stringValue = _assetId;

        return this->setBindingValue( _key, value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::setBindingState( const String & _key, bool _value )
    {
        FigmaBindingValue value;
        value.type = EFigmaBindingValueType::Boolean;
        value.boolValue = _value;

        m_bindingValues[_key] = value;

        if( m_player == nullptr )
        {
            return true;
        }

        const figma_string_view_t key{_key.data(), _key.size()};
        const figma_result_t result = figma_player_set_state( m_player, key, _value == true ? FIGMA_TRUE : FIGMA_FALSE );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::setBindingValue( const String & _key, const FigmaBindingValue & _value )
    {
        if( _key.empty() == true )
        {
            return false;
        }

        m_bindingValues[_key] = _value;

        return this->applyBindingValue_( _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::clearBindingValue( const String & _key )
    {
        m_bindingValues.erase( _key );

        if( m_player == nullptr )
        {
            return true;
        }

        const figma_string_view_t key{_key.data(), _key.size()};
        const figma_result_t result = figma_player_clear_binding_value( m_player, key );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::navigateToFrame( const String & _targetFrameId )
    {
        if( m_player == nullptr )
        {
            return false;
        }

        const figma_string_view_t targetFrameId{_targetFrameId.data(), _targetFrameId.size()};
        const figma_result_t result = figma_player_navigate_to_frame( m_player, targetFrameId );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::openOverlay( const String & _targetFrameId )
    {
        if( m_player == nullptr )
        {
            return false;
        }

        const figma_string_view_t targetFrameId{_targetFrameId.data(), _targetFrameId.size()};
        const figma_result_t result = figma_player_open_overlay( m_player, targetFrameId );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::closeOverlay()
    {
        if( m_player == nullptr )
        {
            return false;
        }

        const figma_result_t result = figma_player_close_overlay( m_player );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::goBack()
    {
        if( m_player == nullptr )
        {
            return false;
        }

        const figma_result_t result = figma_player_go_back( m_player );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceFigma, "name '%s' can't setup resource"
            , this->getName().c_str()
        );

        if( m_resourceFigma->compile() == false )
        {
            LOGGER_ERROR( "figma '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceFigma->getName().c_str()
            );

            return false;
        }

        const FigmaDataInterfacePtr & data = m_resourceFigma->getData();

        MENGINE_ASSERTION_MEMORY_PANIC( data, "figma '%s' resource '%s' invalid data"
            , this->getName().c_str()
            , m_resourceFigma->getName().c_str()
        );

        figma_runtime_t * runtime = data->getFigmaRuntime();
        figma_document_t * document = data->getFigmaDocument();

        MENGINE_ASSERTION_MEMORY_PANIC( runtime, "figma '%s' resource '%s' invalid runtime"
            , this->getName().c_str()
            , m_resourceFigma->getName().c_str()
        );

        MENGINE_ASSERTION_MEMORY_PANIC( document, "figma '%s' resource '%s' invalid document"
            , this->getName().c_str()
            , m_resourceFigma->getName().c_str()
        );

        figma_player_desc_t playerDesc{};
        playerDesc.viewport.width = m_viewportSize.x;
        playerDesc.viewport.height = m_viewportSize.y;
        playerDesc.viewport.scale = m_viewportScale;
        playerDesc.start_frame_id = {m_startFrameId.data(), m_startFrameId.size()};
        playerDesc.user_data = this;

        figma_player_t * player = nullptr;
        figma_result_t result = figma_runtime_create_player( runtime, document, &playerDesc, &player );

        if( result != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "figma '%s' resource '%s' invalid create player result '%s'"
                , this->getName().c_str()
                , m_resourceFigma->getName().c_str()
                , Detail::getFigmaResultMessage( result )
            );

            m_resourceFigma->release();

            return false;
        }

        m_player = player;

        figma_action_router_t actionRouter{};
        actionRouter.user_data = this;
        actionRouter.route_trigger = &Figma::routeTriggerCallback_;
        actionRouter.route_action = &Figma::routeActionCallback_;
        actionRouter.on_frame_changed = &Figma::onFrameChangedCallback_;
        actionRouter.on_overlay_opened = &Figma::onOverlayOpenedCallback_;
        actionRouter.on_overlay_closed = &Figma::onOverlayClosedCallback_;
        actionRouter.on_state_changed = &Figma::onStateChangedCallback_;

        result = figma_player_set_action_router( m_player, &actionRouter );

        if( result != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "figma '%s' resource '%s' invalid action router result '%s'"
                , this->getName().c_str()
                , m_resourceFigma->getName().c_str()
                , Detail::getFigmaResultMessage( result )
            );

            figma_player_destroy( m_player );
            m_player = nullptr;
            m_resourceFigma->release();

            return false;
        }

        for( const MapFigmaBindingValue::value_type & binding : m_bindingValues )
        {
            if( this->applyBindingValue_( binding.first, binding.second ) == false )
            {
                figma_player_destroy( m_player );
                m_player = nullptr;
                m_resourceFigma->release();

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::_release()
    {
        if( m_player != nullptr )
        {
            figma_player_destroy( m_player );
            m_player = nullptr;
        }

        m_renderVertices.clear();
        m_renderIndices.clear();
        m_renderMaterials.clear();
        m_renderScissors.clear();
        this->clearTextureCache_();
        this->clearRenderLayerTargets_();

        if( m_resourceFigma != nullptr )
        {
            m_resourceFigma->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::_dispose()
    {
        m_resourceFigma = nullptr;
        m_bindingValues.clear();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::_activate()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::_afterActivate()
    {
        if( Node::_afterActivate() == false )
        {
            return false;
        }

        this->setPickerPicked( false );
        this->setPickerPressed( false );
        this->setPickerHandle( false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::_deactivate()
    {
        this->setPickerPicked( false );
        this->setPickerPressed( false );
        this->setPickerHandle( false );

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::update( const UpdateContext * _context )
    {
        if( m_player == nullptr )
        {
            return;
        }

        const float dt = _context->time * 0.001f * m_playbackRate;

        figma_result_t result = figma_player_update( m_player, dt );

        if( result != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "figma '%s' resource '%s' invalid update result '%s'"
                , this->getName().c_str()
                , m_resourceFigma->getName().c_str()
                , Detail::getFigmaResultMessage( result )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::clearTextureCache_() const
    {
        for( const MapTextureCache::value_type & value : m_textureCache )
        {
            const TextureCacheDesc & desc = value.second;

            if( desc.texture != nullptr )
            {
                desc.texture->release();
            }
        }

        m_textureCache.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    Figma::RenderLayerTargetDesc * Figma::ensureRenderLayerTarget_( uint32_t _layerId, uint32_t _width, uint32_t _height, float _contentWidth, float _contentHeight ) const
    {
        if( _layerId == 0 || _width == 0 || _height == 0 )
        {
            return nullptr;
        }

        RenderLayerTargetDesc & desc = m_renderLayerTargets[_layerId];

        if( desc.target != nullptr && desc.width == _width && desc.height == _height )
        {
            desc.vertices[1].position = mt::vec3f( _contentWidth, 0.f, 0.f );
            desc.vertices[2].position = mt::vec3f( _contentWidth, _contentHeight, 0.f );
            desc.vertices[3].position = mt::vec3f( 0.f, _contentHeight, 0.f );

            return &desc;
        }

        desc.material = nullptr;
        desc.texture = nullptr;
        desc.target = nullptr;
        desc.width = 0;
        desc.height = 0;

        RenderTargetInterfacePtr target = RENDER_SYSTEM()
            ->createRenderTargetTexture( _width, _height, PF_B8G8R8A8, MENGINE_DOCUMENT_FACTORABLE );

        if( target == nullptr )
        {
            return nullptr;
        }

        RenderImageInterfacePtr image = RENDER_SYSTEM()
            ->createRenderImageTarget( target, MENGINE_DOCUMENT_FACTORABLE );

        if( image == nullptr )
        {
            return nullptr;
        }

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createRenderTexture( image, _width, _height, MENGINE_DOCUMENT_FACTORABLE );

        if( texture == nullptr )
        {
            return nullptr;
        }

        RenderTextureInterfacePtr textures[] = {texture};
        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_TEXTURE_BLEND_PREMULTIPLY, PT_TRIANGLELIST, textures, 1, MENGINE_DOCUMENT_FACTORABLE );

        if( material == nullptr )
        {
            return nullptr;
        }

        const mt::uv4f & uv = target->getUV();
        desc.vertices[0].position = mt::vec3f( 0.f, 0.f, 0.f );
        desc.vertices[1].position = mt::vec3f( _contentWidth, 0.f, 0.f );
        desc.vertices[2].position = mt::vec3f( _contentWidth, _contentHeight, 0.f );
        desc.vertices[3].position = mt::vec3f( 0.f, _contentHeight, 0.f );
        desc.vertices[0].uv[0] = uv.p0;
        desc.vertices[1].uv[0] = uv.p1;
        desc.vertices[2].uv[0] = uv.p2;
        desc.vertices[3].uv[0] = uv.p3;

        for( RenderVertex2D & vertex : desc.vertices )
        {
            vertex.uv[1] = mt::vec2f( 0.f, 0.f );
        }

        desc.target = target;
        desc.texture = texture;
        desc.material = material;
        desc.width = _width;
        desc.height = _height;

        return &desc;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::clearRenderLayerTargets_() const
    {
        m_renderLayerTargets.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::renderLayerTarget_( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, RenderLayerTargetDesc * _target, float _opacity ) const
    {
        if( _target == nullptr || _target->material == nullptr || _opacity <= 0.f )
        {
            return;
        }

        // Fragment_Blend_Premultiply multiplies the sampled RGBA by both the
        // vertex color and its alpha. Feed sqrt(opacity) into every channel so
        // the offscreen layer's premultiplied RGB and alpha are each scaled by
        // opacity exactly once. Using white RGB with alpha=opacity leaves the
        // destination weighted by 1-opacity^2 and visibly doubles shared UI.
        const float premultiplyOpacity = StdMath::sqrtf( Detail::clamp01( _opacity ) );
        const ColorValue_ARGB color = Helper::makeRGBAF( premultiplyOpacity, premultiplyOpacity, premultiplyOpacity, premultiplyOpacity );

        for( RenderVertex2D & vertex : _target->vertices )
        {
            vertex.color = color;
        }

        static const RenderIndex indices[] = {0, 1, 2, 0, 2, 3};
        _renderPipeline->addRenderObject( _context, _target->material, nullptr, _target->vertices, 4, indices, 6, nullptr, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr Figma::createTextureFromPixels_( uint32_t _width, uint32_t _height, const void * _pixels, size_t _pitch ) const
    {
        if( _width == 0 || _height == 0 || _pixels == nullptr )
        {
            return nullptr;
        }

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( 1, _width, _height, PF_A8R8G8B8, MENGINE_DOCUMENT_FACTORABLE );

        if( texture == nullptr )
        {
            return nullptr;
        }

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = _width;
        rect.bottom = _height;

        RenderImageLockedInterfacePtr locked = image->lock( 0, rect, false );

        if( locked == nullptr )
        {
            texture->release();
            return nullptr;
        }

        size_t texturePitch = 0;
        void * textureMemory = locked->getLockedBuffer( &texturePitch );

        if( textureMemory == nullptr )
        {
            image->unlock( locked, 0, false );
            texture->release();
            return nullptr;
        }

        const uint8_t * source = static_cast<const uint8_t *>(_pixels);
        uint8_t * target = static_cast<uint8_t *>(textureMemory);
        const size_t rowSize = _width * 4;

        for( uint32_t y = 0; y != _height; ++y )
        {
            Helper::memoryCopy( target, 0, source, 0, rowSize );

            source += _pitch;
            target += texturePitch;
        }

        image->unlock( locked, 0, true );

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr Figma::createAssetTexture_( figma_document_t * _document, const figma_render_batch_desc_t & _batch ) const
    {
        if( _document == nullptr || _batch.texture_key.size == 0 )
        {
            return nullptr;
        }

        figma_asset_desc_t asset{};

        if( figma_document_find_asset( _document, _batch.texture_key, &asset ) == FIGMA_FALSE || asset.bytes.size == 0 )
        {
            return nullptr;
        }

        ConstString codecType = ConstString::none();
        const String mime = Detail::toLowerString( Detail::makeString( asset.mime ) );
        const String path = Detail::toLowerString( Detail::makeString( asset.path ) );

        if( mime == "image/png" || Detail::hasExtension( path, ".png" ) == true )
        {
            codecType = STRINGIZE_STRING_LOCAL( "pngImage" );
        }
        else if( mime == "image/jpeg" || mime == "image/jpg" || Detail::hasExtension( path, ".jpg" ) == true || Detail::hasExtension( path, ".jpeg" ) == true )
        {
            codecType = STRINGIZE_STRING_LOCAL( "jpegImage" );
        }

        if( codecType.empty() == true )
        {
            LOGGER_WARNING( "figma '%s' unsupported asset mime '%s' path '%s'"
                , this->getName().c_str()
                , mime.c_str()
                , path.c_str()
            );

            return nullptr;
        }

        MemoryInputInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FACTORABLE );

        void * buffer = memory->newBuffer( asset.bytes.size );

        if( buffer == nullptr )
        {
            return nullptr;
        }

        Helper::memoryCopy( buffer, 0, asset.bytes.data, 0, asset.bytes.size );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        if( decoder == nullptr )
        {
            return nullptr;
        }

        if( decoder->prepareData( nullptr, memory ) == false )
        {
            return nullptr;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();
        const uint32_t width = dataInfo->width;
        const uint32_t height = dataInfo->height;
        const EPixelFormat format = dataInfo->format;

        if( width == 0 || height == 0 || format == PF_UNKNOWN )
        {
            return nullptr;
        }

        if( format != PF_R8G8B8 && format != PF_A8R8G8B8 )
        {
            LOGGER_WARNING( "figma '%s' unsupported asset pixel format '%u' mime '%s' path '%s'"
                , this->getName().c_str()
                , (uint32_t)format
                , mime.c_str()
                , path.c_str()
            );

            return nullptr;
        }

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createTexture( 1, width, height, PF_A8R8G8B8, MENGINE_DOCUMENT_FACTORABLE );

        if( texture == nullptr )
        {
            return nullptr;
        }

        const RenderImageInterfacePtr & image = texture->getImage();

        Rect rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = width;
        rect.bottom = height;

        RenderImageLockedInterfacePtr locked = image->lock( 0, rect, false );

        if( locked == nullptr )
        {
            texture->release();
            return nullptr;
        }

        size_t pitch = 0;
        void * textureMemory = locked->getLockedBuffer( &pitch );

        if( textureMemory == nullptr )
        {
            image->unlock( locked, 0, false );
            texture->release();
            return nullptr;
        }

        bool successful = false;

        if( format == PF_R8G8B8 && codecType != STRINGIZE_STRING_LOCAL( "pngImage" ) )
        {
            const size_t sourcePitch = (size_t)width * 3;
            Data sourcePixels;
            sourcePixels.resize( sourcePitch * height );

            ImageDecoderData data;
            data.buffer = sourcePixels.data();
            data.size = sourcePixels.size();
            data.pitch = sourcePitch;
            data.format = PF_R8G8B8;
            data.flags = DF_IMAGE_PREMULTIPLY_ALPHA;
            data.mipmap = 0;

            successful = decoder->decode( &data ) != 0;

            if( successful == true )
            {
                Detail::copyRGBPixelsToTexture_( sourcePixels.data(), sourcePitch, static_cast<uint8_t *>(textureMemory), pitch, width, height );
            }
        }
        else
        {
            ImageDecoderData data;
            data.buffer = textureMemory;
            data.size = pitch * height;
            data.pitch = pitch;
            data.format = PF_A8R8G8B8;
            data.flags = DF_IMAGE_PREMULTIPLY_ALPHA;
            data.mipmap = 0;

            successful = decoder->decode( &data ) != 0;
        }

        if( successful == true )
        {
            Detail::applyImageFilter_( static_cast<uint8_t *>(textureMemory), pitch, width, height, _batch );
        }

        image->unlock( locked, 0, successful );

        if( successful == false )
        {
            texture->release();
            return nullptr;
        }

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr Figma::createGeneratedTexture_( const figma_render_list_t * _renderList, uint32_t _batchIndex, const figma_render_generated_texture_desc_t & _desc ) const
    {
        if( _renderList == nullptr || m_textRasterizer == nullptr )
        {
            return nullptr;
        }

        Data pixels;
        uint32_t width = 0;
        uint32_t height = 0;

        if( m_textRasterizer->makeTextPixels( _renderList, _batchIndex, _desc, m_viewportScale, &pixels, &width, &height ) == false )
        {
            return nullptr;
        }

        Detail::premultiplyPixels_( pixels.data(), width * 4, width, height );

        return this->createTextureFromPixels_( width, height, pixels.data(), width * 4 );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr Figma::getBatchTexture_( const figma_render_list_t * _renderList, const figma_render_batch_desc_t & _batch, uint32_t _batchIndex ) const
    {
        if( _batch.texture_key.size == 0 )
        {
            return nullptr;
        }

        String cacheKey;

        switch( _batch.texture_type )
        {
        case FIGMA_RENDER_TEXTURE_ASSET:
            cacheKey = "asset:";
            break;
        case FIGMA_RENDER_TEXTURE_GENERATED:
            cacheKey = "generated:";
            break;
        case FIGMA_RENDER_TEXTURE_NONE:
        default:
            return nullptr;
        }

        cacheKey.append( _batch.texture_key.data, _batch.texture_key.size );

        if( _batch.texture_type == FIGMA_RENDER_TEXTURE_ASSET )
        {
            Detail::appendImageFilterSignature_( &cacheKey, _batch );
        }

        MapTextureCache::iterator it_found = m_textureCache.find( cacheKey );

        if( _batch.texture_type == FIGMA_RENDER_TEXTURE_ASSET )
        {
            if( it_found != m_textureCache.end() )
            {
                return it_found->second.texture;
            }

            const FigmaDataInterfacePtr & data = m_resourceFigma->getData();
            figma_document_t * document = data->getFigmaDocument();

            TextureCacheDesc desc;
            desc.texture = this->createAssetTexture_( document, _batch );

            if( desc.texture == nullptr )
            {
                return nullptr;
            }

            m_textureCache.emplace( cacheKey, desc );

            return desc.texture;
        }

        figma_render_generated_texture_desc_t generatedDesc{};

        if( figma_render_list_get_generated_texture( _renderList, _batchIndex, &generatedDesc ) != FIGMA_RESULT_OK )
        {
            return nullptr;
        }

        String signature;

        if( m_textRasterizer == nullptr || m_textRasterizer->makeTextSignature( _renderList, _batchIndex, generatedDesc, m_viewportScale, &signature ) == false )
        {
            return nullptr;
        }

        if( it_found != m_textureCache.end() && it_found->second.signature == signature )
        {
            return it_found->second.texture;
        }

        RenderTextureInterfacePtr generatedTexture = this->createGeneratedTexture_( _renderList, _batchIndex, generatedDesc );

        if( generatedTexture == nullptr )
        {
            return nullptr;
        }

        if( it_found != m_textureCache.end() )
        {
            TextureCacheDesc & cached = it_found->second;

            if( cached.texture != nullptr )
            {
                cached.texture->release();
            }

            cached.texture = generatedTexture;
            cached.signature = signature;

            return cached.texture;
        }

        TextureCacheDesc desc;
        desc.texture = generatedTexture;
        desc.signature = signature;

        m_textureCache.emplace( cacheKey, desc );

        return desc.texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        if( m_player == nullptr )
        {
            return;
        }

        const figma_render_list_t * renderList = figma_player_get_render_list( m_player );

        if( renderList == nullptr )
        {
            return;
        }

        const uint32_t batchCount = figma_render_list_get_batch_count( renderList );

        m_renderVertices.resize( batchCount );
        m_renderIndices.resize( batchCount );
        m_renderMaterials.clear();
        m_renderMaterials.reserve( batchCount );
        m_renderScissors.clear();
        m_renderScissors.reserve( batchCount );

        const mt::mat4f & wm = this->getWorldMatrix();

        Color totalColor;
        this->calcTotalColor( &totalColor );

        const float totalColorR = totalColor.getR();
        const float totalColorG = totalColor.getG();
        const float totalColorB = totalColor.getB();
        const float totalColorA = totalColor.getA();

        RenderContext context = *_context;
        Detail::VectorRenderScissorInterface scissorStack;
        scissorStack.reserve( batchCount );
        scissorStack.emplace_back( _context->scissor );

        uint32_t activeRenderLayerId = 0;
        uint32_t renderLayerSegmentIndex = 0;
        float activeRenderLayerOpacity = 1.f;
        RenderLayerTargetDesc * activeRenderLayerTarget = nullptr;

        auto finishRenderLayer = [&]()
        {
            if( activeRenderLayerId == 0 || activeRenderLayerTarget == nullptr )
            {
                return;
            }

            RenderContext layerContext = context;
            layerContext.target = _context->target;
            this->renderLayerTarget_( _renderPipeline, &layerContext, activeRenderLayerTarget, activeRenderLayerOpacity );

            activeRenderLayerId = 0;
            activeRenderLayerOpacity = 1.f;
            activeRenderLayerTarget = nullptr;
        };

        for( uint32_t batchIndex = 0; batchIndex != batchCount; ++batchIndex )
        {
            figma_render_batch_desc_t batch{};
            figma_result_t result = figma_render_list_get_batch( renderList, batchIndex, &batch );

            if( result != FIGMA_RESULT_OK )
            {
                continue;
            }

            if( batch.render_layer_id != activeRenderLayerId )
            {
                finishRenderLayer();

                if( batch.render_layer_id != 0 )
                {
                    if( _context->resolution != nullptr )
                    {
                        // Figma render layers are contiguous command ranges. The same
                        // layer id may occur again after regular geometry and each
                        // range must start with a fresh transparent target.
                        ++renderLayerSegmentIndex;

                        const Resolution & contentResolution = _context->resolution->getContentResolution();
                        const Resolution & windowResolution = APPLICATION_SERVICE()->getCurrentWindowResolution();
                        activeRenderLayerTarget = this->ensureRenderLayerTarget_(
                            renderLayerSegmentIndex,
                            windowResolution.getWidth(),
                            windowResolution.getHeight(),
                            contentResolution.getWidthF(),
                            contentResolution.getHeightF()
                        );
                    }

                    if( activeRenderLayerTarget != nullptr )
                    {
                        activeRenderLayerId = batch.render_layer_id;
                        activeRenderLayerOpacity = batch.render_layer_opacity;
                    }
                }
            }

            if( activeRenderLayerId != 0 && batch.render_layer_id == activeRenderLayerId && activeRenderLayerTarget != nullptr )
            {
                context.target = activeRenderLayerTarget->target.get();
            }
            else
            {
                context.target = _context->target;
            }

            if( batch.batch_type == FIGMA_RENDER_BATCH_CLIP_BEGIN )
            {
                Detail::FigmaRenderScissorPtr scissor = Helper::makeFactorableUnique<Detail::FigmaRenderScissor>( MENGINE_DOCUMENT_FORWARD );
                scissor->setScissorViewport( wm, batch.clip_rect, context.scissor );

                context.scissor = scissor.get();
                scissorStack.emplace_back( context.scissor );
                m_renderScissors.emplace_back( scissor );

                continue;
            }

            if( batch.batch_type == FIGMA_RENDER_BATCH_CLIP_END )
            {
                if( scissorStack.size() > 1 )
                {
                    scissorStack.pop_back();
                    context.scissor = scissorStack.back();
                }

                continue;
            }

            if( batch.batch_type != FIGMA_RENDER_BATCH_GEOMETRY )
            {
                continue;
            }

            if( batch.vertex_count == 0 || batch.index_count == 0 )
            {
                continue;
            }

            RenderTextureInterfacePtr texture;

            if( batch.shader_type == FIGMA_RENDER_SHADER_TEXTURE )
            {
                texture = this->getBatchTexture_( renderList, batch, batchIndex );

                if( texture == nullptr )
                {
                    continue;
                }
            }
            else if( batch.shader_type != FIGMA_RENDER_SHADER_COLOR || batch.texture_type != FIGMA_RENDER_TEXTURE_NONE )
            {
                continue;
            }

            const bool renderToLayer = activeRenderLayerId != 0 && batch.render_layer_id == activeRenderLayerId && activeRenderLayerTarget != nullptr;
            const float batchAlpha = renderToLayer == true ? batch.opacity : batch.opacity * batch.render_layer_opacity;

            ColorValue_ARGB color = Helper::makeRGBAF(
                totalColorR,
                totalColorG,
                totalColorB,
                totalColorA * batchAlpha
            );

            if( (color & 0xFF000000) == 0 )
            {
                continue;
            }

            VectorRenderVertex2D & vertices = m_renderVertices[batchIndex];
            vertices.resize( batch.vertex_count );

            for( uint32_t vertexIndex = 0; vertexIndex != batch.vertex_count; ++vertexIndex )
            {
                const figma_render_vertex_t & figmaVertex = batch.vertices[vertexIndex];
                RenderVertex2D & vertex = vertices[vertexIndex];

                ColorValue_ARGB vertexColor = Helper::makeRGBAF(
                    totalColorR * figmaVertex.color.r,
                    totalColorG * figmaVertex.color.g,
                    totalColorB * figmaVertex.color.b,
                    totalColorA * batchAlpha * figmaVertex.color.a
                );

                Detail::transformFigmaVertex( &vertex, figmaVertex, wm, texture, vertexColor );
            }

            VectorRenderIndex & indices = m_renderIndices[batchIndex];
            indices.resize( batch.index_count );

            for( uint32_t index = 0; index != batch.index_count; ++index )
            {
                indices[index] = static_cast<RenderIndex>(batch.indices[index]);
            }

            EMaterialBlendMode blendMode = Detail::getFigmaMaterialBlendMode( batch.blend_mode );

            RenderMaterialInterfacePtr material;

            if( texture != nullptr )
            {
                EMaterial materialId;

                switch( blendMode )
                {
                case EMB_ADD:
                    materialId = EM_TEXTURE_INTENSIVE_PREMULTIPLY;
                    break;
                case EMB_SCREEN:
                    materialId = EM_TEXTURE_SCREEN_PREMULTIPLY;
                    break;
                case EMB_MULTIPLY:
                    materialId = EM_TEXTURE_MULTIPLY_PREMULTIPLY;
                    break;
                case EMB_NORMAL:
                default:
                    materialId = EM_TEXTURE_BLEND_PREMULTIPLY;
                    break;
                }

                RenderTextureInterfacePtr textures[] = {texture};
                material = RENDERMATERIAL_SERVICE()
                    ->getMaterial3( materialId, PT_TRIANGLELIST, textures, 1, MENGINE_DOCUMENT_FACTORABLE );
            }
            else
            {
                material = RENDERMATERIAL_SERVICE()
                    ->getSolidMaterial( blendMode, false );
            }

            m_renderMaterials.emplace_back( material );

            _renderPipeline->addRenderObject( &context, material, nullptr, vertices.data(), batch.vertex_count, indices.data(), batch.index_count, nullptr, false, MENGINE_DOCUMENT_FORWARD );
        }

        finishRenderLayer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointer_( figma_pointer_event_type_t _type, uint32_t _pointerId, float _x, float _y, figma_pointer_button_t _button, figma_input_modifier_flags_t _modifiers, figma_input_dispatch_result_t * const _dispatch )
    {
        if( m_player == nullptr )
        {
            return false;
        }

        figma_pointer_event_t event{};
        event.type = _type;
        event.pointer_id = _pointerId;
        event.x = _x;
        event.y = _y;
        event.button = _button;
        event.modifiers = _modifiers;

        figma_result_t result = figma_player_input_pointer( m_player, &event, _dispatch );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputKey_( figma_key_event_type_t _type, uint32_t _keyCode, figma_input_modifier_flags_t _modifiers, figma_input_dispatch_result_t * const _dispatch )
    {
        if( m_player == nullptr )
        {
            return false;
        }

        figma_key_event_t event{};
        event.type = _type;
        event.key_code = _keyCode;
        event.modifiers = _modifiers;

        figma_result_t result = figma_player_input_key( m_player, &event, _dispatch );

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::screenToLocal_( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _localPoint ) const
    {
        if( _context == nullptr || _context->resolution == nullptr || _context->camera == nullptr || _context->viewport == nullptr || _localPoint == nullptr )
        {
            return false;
        }

        mt::vec2f pointViewport;
        _context->resolution->fromScreenToContentPosition( _screenPoint, &pointViewport );

        const Viewport & viewport = _context->viewport->getViewportWM();
        pointViewport -= viewport.begin;

        const mt::vec2f viewportSize = viewport.size();
        if( viewportSize.x <= mt::constant::eps || viewportSize.y <= mt::constant::eps )
        {
            return false;
        }

        pointViewport /= viewportSize;

        mt::vec2f pointNormalized;
        pointNormalized.x = pointViewport.x * 2.f - 1.f;
        pointNormalized.y = 1.f - pointViewport.y * 2.f;

        mt::vec2f pointWorld;
        mt::mul_v2_v2_m4( &pointWorld, pointNormalized, _context->camera->getCameraViewProjectionMatrixInv() );

        mt::mat4f worldMatrixInv;
        mt::inv_m4_m4( &worldMatrixInv, this->getWorldMatrix() );
        mt::mul_v2_v2_m4( _localPoint, pointWorld, worldMatrixInv );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::pick( const mt::vec2f & _point, const RenderContext * _context ) const
    {
        if( m_player == nullptr )
        {
            return false;
        }

        mt::vec2f localPoint;
        if( this->screenToLocal_( _context, _point, &localPoint ) == false )
        {
            return false;
        }

        figma_bool_t hit = FIGMA_FALSE;
        const figma_result_t result = figma_player_hit_test( m_player, localPoint.x, localPoint.y, &hit );
        if( result != FIGMA_RESULT_OK )
        {
            return false;
        }

        return hit == FIGMA_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Scriptable * Figma::getPickerScriptable()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    Eventable * Figma::getPickerEventable()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    PickerInputHandlerInterface * Figma::getPickerInputHandler()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event )
    {
        MENGINE_UNUSED( _context );

        figma_input_dispatch_result_t dispatch{};
        const figma_key_event_type_t type = _event.isDown == true ? FIGMA_KEY_EVENT_DOWN : FIGMA_KEY_EVENT_UP;
        if( this->inputKey_( type, static_cast<uint32_t>(_event.code), Detail::getFigmaInputModifiers( _event.special ), &dispatch ) == false )
        {
            return false;
        }

        return dispatch.handled;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleTextEvent( const RenderContext * _context, const InputTextEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        mt::vec2f localPoint;
        if( this->screenToLocal_( _context, _event.position.screen, &localPoint ) == false )
        {
            return false;
        }

        figma_input_dispatch_result_t dispatch{};
        const figma_pointer_event_type_t type = _event.isDown == true ? FIGMA_POINTER_EVENT_DOWN : FIGMA_POINTER_EVENT_UP;
        if( this->inputPointer_( type, _event.touchId, localPoint.x, localPoint.y, Detail::getFigmaPointerButton( static_cast<uint32_t>(_event.button) ), Detail::getFigmaInputModifiers( _event.special ), &dispatch ) == false )
        {
            return false;
        }

        return dispatch.hit == true || dispatch.handled == true || dispatch.captured == true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event )
    {
        mt::vec2f localPoint;
        if( this->screenToLocal_( _context, _event.position.screen, &localPoint ) == false )
        {
            return false;
        }

        figma_input_dispatch_result_t dispatch{};
        if( this->inputPointer_( FIGMA_POINTER_EVENT_MOVE, _event.touchId, localPoint.x, localPoint.y, FIGMA_POINTER_BUTTON_NONE, Detail::getFigmaInputModifiers( _event.special ), &dispatch ) == false )
        {
            return false;
        }

        return dispatch.hit == true || dispatch.handled == true || dispatch.captured == true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _event );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event )
    {
        mt::vec2f localPoint;
        if( this->screenToLocal_( _context, _event.position.screen, &localPoint ) == false )
        {
            return false;
        }

        figma_input_dispatch_result_t dispatch{};
        if( this->inputPointer_( FIGMA_POINTER_EVENT_MOVE, _event.touchId, localPoint.x, localPoint.y, FIGMA_POINTER_BUTTON_NONE, Detail::getFigmaInputModifiers( _event.special ), &dispatch ) == false )
        {
            return false;
        }

        return dispatch.hit == true || dispatch.handled == true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event )
    {
        mt::vec2f localPoint;
        if( this->screenToLocal_( _context, _event.position.screen, &localPoint ) == false )
        {
            return;
        }

        this->inputPointer_( FIGMA_POINTER_EVENT_MOVE, _event.touchId, localPoint.x, localPoint.y, FIGMA_POINTER_BUTTON_NONE, Detail::getFigmaInputModifiers( _event.special ), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    figma_result_t FIGMA_CALL Figma::routeTriggerCallback_( void * _userData, const figma_trigger_event_t * _event )
    {
        if( _userData == nullptr || _event == nullptr )
        {
            return FIGMA_RESULT_INVALID_ARGUMENT;
        }

        Figma * self = static_cast<Figma *>(_userData);

        return self->routeTrigger_( *_event );
    }
    //////////////////////////////////////////////////////////////////////////
    figma_result_t FIGMA_CALL Figma::routeActionCallback_( void * _userData, const figma_action_event_t * _event, figma_action_response_t * _response )
    {
        if( _userData == nullptr || _event == nullptr || _response == nullptr )
        {
            return FIGMA_RESULT_INVALID_ARGUMENT;
        }

        Figma * self = static_cast<Figma *>(_userData);

        return self->routeAction_( *_event, _response );
    }
    //////////////////////////////////////////////////////////////////////////
    void FIGMA_CALL Figma::onFrameChangedCallback_( void * _userData, figma_string_view_t _previousFrameId, figma_string_view_t _currentFrameId )
    {
        Figma * self = static_cast<Figma *>(_userData);

        self->onFrameChanged_( _previousFrameId, _currentFrameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void FIGMA_CALL Figma::onOverlayOpenedCallback_( void * _userData, figma_string_view_t _frameId )
    {
        Figma * self = static_cast<Figma *>(_userData);

        self->onOverlayOpened_( _frameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void FIGMA_CALL Figma::onOverlayClosedCallback_( void * _userData, figma_string_view_t _frameId )
    {
        Figma * self = static_cast<Figma *>(_userData);

        self->onOverlayClosed_( _frameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void FIGMA_CALL Figma::onStateChangedCallback_( void * _userData, figma_string_view_t _sourceNodeId, figma_string_view_t _previousStateId, figma_string_view_t _currentStateId )
    {
        Figma * self = static_cast<Figma *>(_userData);

        self->onStateChanged_( _sourceNodeId, _previousStateId, _currentStateId );
    }
    //////////////////////////////////////////////////////////////////////////
    figma_result_t Figma::routeTrigger_( const figma_trigger_event_t & _event )
    {
        FigmaTriggerEvent event;
        event.inputKind = static_cast<EFigmaActionInputKind>(_event.input_kind);
        event.triggerType = static_cast<EFigmaTriggerType>(_event.trigger_type);
        event.interactionId = Detail::makeString( _event.interaction_id );
        event.sourceNodeId = Detail::makeString( _event.source_node_id );
        event.currentFrameId = Detail::makeString( _event.current_frame_id );
        event.pointerId = _event.pointer.pointer_id;
        event.x = _event.pointer.x;
        event.y = _event.pointer.y;
        event.button = static_cast<uint32_t>(_event.pointer.button);
        event.keyCode = _event.key.key_code;
        event.modifiers = static_cast<uint32_t>(_event.input_kind == FIGMA_ACTION_INPUT_KEY ? _event.key.modifiers : _event.pointer.modifiers);

        EVENTABLE_METHOD( EVENT_FIGMA_TRIGGER )
            ->onFigmaTrigger( event );

        return FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    figma_result_t Figma::routeAction_( const figma_action_event_t & _event, figma_action_response_t * const _response )
    {
        FigmaActionEvent event;
        event.inputKind = static_cast<EFigmaActionInputKind>(_event.input_kind);
        event.triggerType = static_cast<EFigmaTriggerType>(_event.trigger_type);
        event.connectionType = static_cast<EFigmaConnectionType>(_event.connection_type);
        event.navigationType = static_cast<EFigmaNavigationType>(_event.navigation_type);
        event.actionId = Detail::makeString( _event.action_id );
        event.interactionId = Detail::makeString( _event.interaction_id );
        event.sourceNodeId = Detail::makeString( _event.source_node_id );
        event.currentFrameId = Detail::makeString( _event.current_frame_id );
        event.targetFrameId = Detail::makeString( _event.target_frame_id );
        event.pointerId = _event.pointer.pointer_id;
        event.x = _event.pointer.x;
        event.y = _event.pointer.y;
        event.button = static_cast<uint32_t>(_event.pointer.button);
        event.keyCode = _event.key.key_code;
        event.modifiers = static_cast<uint32_t>(_event.input_kind == FIGMA_ACTION_INPUT_KEY ? _event.key.modifiers : _event.pointer.modifiers);

        FigmaActionResponse response;
        response.result = static_cast<EFigmaActionResult>(_response->result);
        response.targetFrameId = Detail::makeString( _response->target_frame_id );

        EVENTABLE_METHOD( EVENT_FIGMA_ACTION )
            ->onFigmaAction( event, &response );

        _response->result = static_cast<figma_action_result_t>(response.result);
        m_actionTargetFrameId = response.targetFrameId;
        _response->target_frame_id = {m_actionTargetFrameId.data(), m_actionTargetFrameId.size()};

        return FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::onFrameChanged_( figma_string_view_t _previousFrameId, figma_string_view_t _currentFrameId )
    {
        const String previousFrameId = Detail::makeString( _previousFrameId );
        const String currentFrameId = Detail::makeString( _currentFrameId );

        EVENTABLE_METHOD( EVENT_FIGMA_FRAME_CHANGED )
            ->onFigmaFrameChanged( previousFrameId, currentFrameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::onOverlayOpened_( figma_string_view_t _frameId )
    {
        const String frameId = Detail::makeString( _frameId );

        EVENTABLE_METHOD( EVENT_FIGMA_OVERLAY_OPENED )
            ->onFigmaOverlayOpened( frameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::onOverlayClosed_( figma_string_view_t _frameId )
    {
        const String frameId = Detail::makeString( _frameId );

        EVENTABLE_METHOD( EVENT_FIGMA_OVERLAY_CLOSED )
            ->onFigmaOverlayClosed( frameId );
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::onStateChanged_( figma_string_view_t _sourceNodeId, figma_string_view_t _previousStateId, figma_string_view_t _currentStateId )
    {
        const String sourceNodeId = Detail::makeString( _sourceNodeId );
        const String previousStateId = Detail::makeString( _previousStateId );
        const String currentStateId = Detail::makeString( _currentStateId );

        EVENTABLE_METHOD( EVENT_FIGMA_STATE_CHANGED )
            ->onFigmaStateChanged( sourceNodeId, previousStateId, currentStateId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::applyBindingValue_( const String & _key, const FigmaBindingValue & _value )
    {
        if( m_player == nullptr )
        {
            return true;
        }

        const figma_string_view_t key{_key.data(), _key.size()};
        figma_result_t result = FIGMA_RESULT_INVALID_ARGUMENT;

        switch( _value.type )
        {
        case EFigmaBindingValueType::Text:
            {
                const figma_string_view_t value{_value.stringValue.data(), _value.stringValue.size()};
                result = figma_player_set_text( m_player, key, value );
            }
            break;
        case EFigmaBindingValueType::Number:
            result = figma_player_set_number( m_player, key, _value.numberValue );
            break;
        case EFigmaBindingValueType::Boolean:
            {
                figma_binding_value_t value{};
                value.type = FIGMA_BINDING_VALUE_BOOLEAN;
                value.bool_value = _value.boolValue == true ? FIGMA_TRUE : FIGMA_FALSE;
                result = figma_player_set_binding_value( m_player, key, &value );
            }
            break;
        case EFigmaBindingValueType::Image:
            {
                const figma_string_view_t value{_value.stringValue.data(), _value.stringValue.size()};
                result = figma_player_set_image( m_player, key, value );
            }
            break;
        }

        return result == FIGMA_RESULT_OK;
    }
    //////////////////////////////////////////////////////////////////////////
}
