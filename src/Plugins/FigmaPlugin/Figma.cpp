#include "Figma.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/RenderImageInterface.h"
#include "Interface/RenderImageLockedInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/PixelFormatHelper.h"

#include "Config/StdString.h"

#include "freetype/freetype.h"

#include "math/uv4.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <string_view>

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
            void setScissorViewport( const mt::mat4f & _wm, const ::Figma::Rectf & _rect, const RenderScissorInterface * _parent )
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
        //////////////////////////////////////////////////////////////////////////
        static String makeString( ::Figma::FigmaStringView _value )
        {
            return String( _value.data(), _value.size() );
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
            std::transform( _value.begin(), _value.end(), _value.begin(), []( unsigned char _ch )
            {
                return static_cast<Char>(std::tolower( _ch ));
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
        static void addUniqueDirectory( Vector<String> * const _directories, const Char * _directory )
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

            if( std::find( _directories->begin(), _directories->end(), directory ) != _directories->end() )
            {
                return;
            }

            _directories->emplace_back( directory );
        }
        //////////////////////////////////////////////////////////////////////////
        static String normalizeFontName( String _value )
        {
            _value.erase( std::remove_if( _value.begin(), _value.end(), []( Char _ch )
            {
                return _ch == ' ' || _ch == '-';
            } ), _value.end() );

            return Detail::toLowerString( _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static float clamp01( float _value )
        {
            return std::max( 0.f, std::min( 1.f, _value ) );
        }
        //////////////////////////////////////////////////////////////////////////
        static float to26Dot6( FT_Pos _value )
        {
            return static_cast<float>(_value) / 64.f;
        }
        //////////////////////////////////////////////////////////////////////////
        static void appendSignature( String * const _signature, ::Figma::FigmaStringView _value )
        {
            _signature->append( _value.data(), _value.size() );
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
        {
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

            ~FigmaTextRasterizer()
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

                const Char * env = std::getenv( "FIGMA_VIEWER_FONT_DIRS" );
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

            bool makeTextPixels( const ::Figma::RenderListInterface * _renderList, uint32_t _batchIndex, const ::Figma::RenderGeneratedTextureDesc & _desc, float _rasterScale, Vector<uint8_t> * const _pixels, uint32_t * const _width, uint32_t * const _height, String * const _signature )
            {
                if( m_library == nullptr || _pixels == nullptr || _width == nullptr || _height == nullptr || _signature == nullptr )
                {
                    return false;
                }

                if( _desc.text.empty() == true || _desc.color.a <= 0.f || _desc.fontSize <= 0.f || _desc.rect.w <= 0.f || _desc.rect.h <= 0.f )
                {
                    return false;
                }

                FT_Face face = this->faceForDesc_( _desc );

                if( face == nullptr )
                {
                    return false;
                }

                const float rasterScale = std::max( 1.f, _rasterScale );
                const FT_UInt pixelSize = static_cast<FT_UInt>(std::max<long>( 1, std::lround( _desc.fontSize * rasterScale ) ));

                if( FT_Set_Pixel_Sizes( face, 0, pixelSize ) != FT_Err_Ok )
                {
                    return false;
                }

                const uint32_t width = static_cast<uint32_t>(std::ceil( std::max( 1.f, _desc.rect.w ) * rasterScale ));
                const uint32_t height = static_cast<uint32_t>(std::ceil( std::max( 1.f, _desc.rect.h ) * rasterScale ));

                if( width == 0 || height == 0 )
                {
                    return false;
                }

                Vector<::Figma::RenderGeneratedTextLineDesc> lines;
                lines.reserve( _desc.textLineCount );

                for( uint32_t lineIndex = 0; lineIndex != _desc.textLineCount; ++lineIndex )
                {
                    ::Figma::RenderGeneratedTextLineDesc line;

                    if( _renderList->getGeneratedTextureTextLine( _batchIndex, lineIndex, &line ) != ::Figma::EResult::Ok )
                    {
                        return false;
                    }

                    lines.emplace_back( line );
                }

                *_signature = this->makeSignature_( _desc, lines, width, height );

                _pixels->assign( width * height * 4, 0 );

                for( uint32_t lineIndex = 0; lineIndex != _desc.textLineCount; ++lineIndex )
                {
                    const ::Figma::RenderGeneratedTextLineDesc & line = lines[lineIndex];
                    std::string_view lineText = this->textLineView_( _desc, lines, lineIndex );
                    Vector<char32_t> codepoints = this->decodeUtf8_( lineText );

                    if( codepoints.empty() == true )
                    {
                        continue;
                    }

                    const float ascent = line.lineAscent > 0.f
                        ? line.lineAscent * rasterScale
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
                for( const Map<String, FT_Face>::value_type & value : m_faces )
                {
                    if( value.second != nullptr )
                    {
                        FT_Done_Face( value.second );
                    }
                }

                m_faces.clear();
            }

            String makeSignature_( const ::Figma::RenderGeneratedTextureDesc & _desc, const Vector<::Figma::RenderGeneratedTextLineDesc> & _lines, uint32_t _width, uint32_t _height ) const
            {
                String signature;
                Detail::appendSignature( &signature, _desc.key );
                Detail::appendSignature( &signature, _desc.text );
                Detail::appendSignature( &signature, _desc.fontFamily );
                Detail::appendSignature( &signature, _desc.fontStyle );
                Detail::appendSignature( &signature, _desc.fontPostscriptName );
                signature += std::to_string( _desc.fontSize );
                signature.push_back( '|' );
                signature += std::to_string( _desc.color.r );
                signature.push_back( ',' );
                signature += std::to_string( _desc.color.g );
                signature.push_back( ',' );
                signature += std::to_string( _desc.color.b );
                signature.push_back( ',' );
                signature += std::to_string( _desc.color.a );
                signature.push_back( '|' );
                signature += std::to_string( _width );
                signature.push_back( 'x' );
                signature += std::to_string( _height );
                signature.push_back( '|' );

                for( const ::Figma::RenderGeneratedTextLineDesc & line : _lines )
                {
                    Detail::appendSignature( &signature, line.text );
                    signature += std::to_string( line.x );
                    signature.push_back( ',' );
                    signature += std::to_string( line.y );
                    signature.push_back( ',' );
                    signature += std::to_string( line.width );
                    signature.push_back( ',' );
                    signature += std::to_string( line.lineAscent );
                    signature.push_back( '|' );
                }

                return signature;
            }

            FT_Face faceForDesc_( const ::Figma::RenderGeneratedTextureDesc & _desc )
            {
                String key = this->fontKeyForDesc_( _desc );

                if( key.empty() == true )
                {
                    return nullptr;
                }

                Map<String, FT_Face>::iterator it_found = m_faces.find( key );

                if( it_found != m_faces.end() )
                {
                    return it_found->second;
                }

                FT_Face face = this->openFaceForDesc_( _desc );
                m_faces.emplace( key, face );

                return face;
            }

            String fontKeyForDesc_( const ::Figma::RenderGeneratedTextureDesc & _desc ) const
            {
                if( _desc.fontPostscriptName.empty() == false )
                {
                    return Detail::makeString( _desc.fontPostscriptName );
                }

                String key = Detail::makeString( _desc.fontFamily );

                if( key.empty() == false && _desc.fontStyle.empty() == false )
                {
                    key += "-";
                    key += Detail::makeString( _desc.fontStyle );
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

            FT_Face openFaceForDesc_( const ::Figma::RenderGeneratedTextureDesc & _desc ) const
            {
                const String postscriptName = Detail::makeString( _desc.fontPostscriptName );
                const String familyName = Detail::makeString( _desc.fontFamily );
                const String styleName = Detail::makeString( _desc.fontStyle );

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

            Vector<char32_t> decodeUtf8_( std::string_view _text ) const
            {
                Vector<char32_t> result;
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

            std::string_view sourceTextView_( const ::Figma::RenderGeneratedTextureDesc & _desc, const ::Figma::RenderGeneratedTextLineDesc & _line ) const
            {
                const ::Figma::FigmaStringView source = _line.text.empty() == false ? _line.text : _desc.text;
                return std::string_view( source.data(), source.size() );
            }

            std::string_view trimTrailingWhitespace_( std::string_view _value ) const
            {
                while( _value.empty() == false && (_value.back() == ' ' || _value.back() == '\t' || _value.back() == '\r' || _value.back() == '\n') )
                {
                    _value.remove_suffix( 1 );
                }

                return _value;
            }

            std::string_view explicitLineSegment_( std::string_view _value, size_t _lineIndex ) const
            {
                size_t begin = 0;
                size_t index = 0;

                while( begin <= _value.size() )
                {
                    const size_t end = _value.find_first_of( "\r\n", begin );

                    if( index == _lineIndex )
                    {
                        return end == std::string_view::npos ? _value.substr( begin ) : _value.substr( begin, end - begin );
                    }

                    if( end == std::string_view::npos )
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

            std::string_view textLineView_( const ::Figma::RenderGeneratedTextureDesc & _desc, const Vector<::Figma::RenderGeneratedTextLineDesc> & _lines, size_t _lineIndex ) const
            {
                const ::Figma::RenderGeneratedTextLineDesc & line = _lines[_lineIndex];
                std::string_view view = this->sourceTextView_( _desc, line );

                if( view.find_first_of( "\r\n" ) == std::string_view::npos )
                {
                    if( _lineIndex + 1 < _lines.size() )
                    {
                        std::string_view nextView = this->sourceTextView_( _desc, _lines[_lineIndex + 1] );

                        if( nextView.find_first_of( "\r\n" ) != std::string_view::npos )
                        {
                            nextView = this->explicitLineSegment_( nextView, _lineIndex + 1 );
                        }

                        nextView = this->trimTrailingWhitespace_( nextView );

                        const size_t suffix = nextView.empty() == false ? view.rfind( nextView ) : std::string_view::npos;

                        if( suffix != std::string_view::npos && suffix > 0 )
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

            float measureLineAdvance_( FT_Face _face, const Vector<char32_t> & _codepoints ) const
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

            float horizontalScaleForLine_( FT_Face _face, const Vector<char32_t> & _codepoints, float _targetWidth ) const
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

                return std::max( 0.25f, std::min( 4.f, scale ) );
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
                _target[0] = static_cast<uint8_t>(std::lround( Detail::clamp01( _red ) * 255.f ));
                _target[1] = static_cast<uint8_t>(std::lround( Detail::clamp01( _green ) * 255.f ));
                _target[2] = static_cast<uint8_t>(std::lround( Detail::clamp01( _blue ) * 255.f ));
                _target[3] = static_cast<uint8_t>(std::lround( Detail::clamp01( _alpha ) * 255.f ));
#else
                _target[0] = static_cast<uint8_t>(std::lround( Detail::clamp01( _blue ) * 255.f ));
                _target[1] = static_cast<uint8_t>(std::lround( Detail::clamp01( _green ) * 255.f ));
                _target[2] = static_cast<uint8_t>(std::lround( Detail::clamp01( _red ) * 255.f ));
                _target[3] = static_cast<uint8_t>(std::lround( Detail::clamp01( _alpha ) * 255.f ));
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

            void blendGlyphBitmapPixel_( const FT_Bitmap & _bitmap, int32_t _sourceX, int32_t _sourceY, uint8_t * const _target, const ::Figma::RenderGeneratedTextureDesc & _desc ) const
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
                        red = std::min( 1.f, red * invAlpha );
                        green = std::min( 1.f, green * invAlpha );
                        blue = std::min( 1.f, blue * invAlpha );
                    }

                    this->blendStraightPixel_( _target, red, green, blue, sourceAlpha );

                    return;
                }

                const uint8_t coverage = this->coverageAt_( _bitmap, _sourceX, _sourceY );
                const float sourceAlpha = static_cast<float>(coverage) / 255.f * commandAlpha;

                this->blendStraightPixel_( _target, Detail::clamp01( _desc.color.r ), Detail::clamp01( _desc.color.g ), Detail::clamp01( _desc.color.b ), sourceAlpha );
            }

            void rasterizeLine_( FT_Face _face, const ::Figma::RenderGeneratedTextureDesc & _desc, const Vector<char32_t> & _codepoints, float _baselineX, float _baselineY, float _horizontalScale, uint8_t * const _pixels, uint32_t _width, uint32_t _height, uint32_t _stride ) const
            {
                float penX = _baselineX;
                FT_UInt previousGlyph = 0;
                const float horizontalScale = std::max( 0.001f, _horizontalScale );

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
                        const int32_t destinationX = static_cast<int32_t>(std::lround( glyphX ));
                        const int32_t destinationY = static_cast<int32_t>(std::lround( glyphY ));
                        const int32_t destinationWidth = std::max<int32_t>( 1, static_cast<int32_t>(std::lround( static_cast<float>(bitmap.width) * horizontalScale )) );
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
                                const int32_t sourceX = std::min<int32_t>( static_cast<int32_t>(bitmap.width) - 1, std::max<int32_t>( 0, static_cast<int32_t>(std::floor( (static_cast<float>(scaledX) + 0.5f) / horizontalScale )) ) );
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
            Vector<String> m_directories;
            Map<String, FT_Face> m_faces;
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * getFigmaResultMessage( ::Figma::EResult _result )
        {
            switch( _result )
            {
            case ::Figma::EResult::Ok:
                return "Ok";
            case ::Figma::EResult::InvalidArgument:
                return "InvalidArgument";
            case ::Figma::EResult::OutOfMemory:
                return "OutOfMemory";
            case ::Figma::EResult::IoFailed:
                return "IoFailed";
            case ::Figma::EResult::ParseFailed:
                return "ParseFailed";
            case ::Figma::EResult::UnsupportedFormat:
                return "UnsupportedFormat";
            case ::Figma::EResult::MissingEntry:
                return "MissingEntry";
            case ::Figma::EResult::NotFound:
                return "NotFound";
            case ::Figma::EResult::InvalidState:
                return "InvalidState";
            }

            return "Unknown";
        }
        //////////////////////////////////////////////////////////////////////////
        static EMaterialBlendMode getFigmaMaterialBlendMode( ::Figma::ERenderBlendMode _blendMode )
        {
            switch( _blendMode )
            {
            case ::Figma::ERenderBlendMode::Screen:
                return EMB_SCREEN;
            case ::Figma::ERenderBlendMode::Multiply:
                return EMB_MULTIPLY;
            case ::Figma::ERenderBlendMode::ColorDodge:
            case ::Figma::ERenderBlendMode::Lighten:
                return EMB_ADD;
            case ::Figma::ERenderBlendMode::PassThrough:
            case ::Figma::ERenderBlendMode::Normal:
            default:
                return EMB_NORMAL;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static ::Figma::EPointerButton getFigmaPointerButton( uint32_t _button )
        {
            switch( _button )
            {
            case MC_LBUTTON:
                return ::Figma::EPointerButton::Left;
            case MC_MBUTTON:
                return ::Figma::EPointerButton::Middle;
            case MC_RBUTTON:
                return ::Figma::EPointerButton::Right;
            default:
                return ::Figma::EPointerButton::Other;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void transformFigmaVertex( RenderVertex2D * const _vertex, const ::Figma::RenderVertex & _figmaVertex, const mt::mat4f & _wm, const RenderTextureInterfacePtr & _texture, ColorValue_ARGB _color )
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
        , m_textRasterizer( std::make_unique<Detail::FigmaTextRasterizer>() )
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

        this->recompile( [this, &_size]()
        {
            m_viewportSize = _size;

            return true;
        } );
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

        this->recompile( [this, _scale]()
        {
            m_viewportScale = _scale;

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    float Figma::getViewportScale() const
    {
        return m_viewportScale;
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
    bool Figma::inputPointerMove( float _x, float _y )
    {
        return this->inputPointer_( ::Figma::EPointerEventType::Move, _x, _y, ::Figma::EPointerButton::None );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointerDown( float _x, float _y, uint32_t _button )
    {
        return this->inputPointer_( ::Figma::EPointerEventType::Down, _x, _y, Detail::getFigmaPointerButton( _button ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointerUp( float _x, float _y, uint32_t _button )
    {
        return this->inputPointer_( ::Figma::EPointerEventType::Up, _x, _y, Detail::getFigmaPointerButton( _button ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointerCancel( float _x, float _y )
    {
        return this->inputPointer_( ::Figma::EPointerEventType::Cancel, _x, _y, ::Figma::EPointerButton::None );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputKeyDown( uint32_t _keyCode )
    {
        return this->inputKey_( ::Figma::EKeyEventType::Down, _keyCode );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputKeyUp( uint32_t _keyCode )
    {
        return this->inputKey_( ::Figma::EKeyEventType::Up, _keyCode );
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

        ::Figma::RuntimeInterface * runtime = data->getFigmaRuntime();
        ::Figma::DocumentInterface * document = data->getFigmaDocument();

        MENGINE_ASSERTION_MEMORY_PANIC( runtime, "figma '%s' resource '%s' invalid runtime"
            , this->getName().c_str()
            , m_resourceFigma->getName().c_str()
        );

        MENGINE_ASSERTION_MEMORY_PANIC( document, "figma '%s' resource '%s' invalid document"
            , this->getName().c_str()
            , m_resourceFigma->getName().c_str()
        );

        ::Figma::PlayerDesc playerDesc;
        playerDesc.viewport.width = m_viewportSize.x;
        playerDesc.viewport.height = m_viewportSize.y;
        playerDesc.viewport.scale = m_viewportScale;
        playerDesc.startFrameId = m_startFrameId.empty() == true ? nullptr : m_startFrameId.c_str();
        playerDesc.ud = this;

        ::Figma::PlayerInterface * player = nullptr;
        ::Figma::EResult result = runtime->createPlayer( document, playerDesc, &player );

        if( result != ::Figma::EResult::Ok )
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::_release()
    {
        if( m_player != nullptr )
        {
            m_player->destroy();
            m_player = nullptr;
        }

        m_renderVertices.clear();
        m_renderIndices.clear();
        m_renderMaterials.clear();
        m_renderScissors.clear();
        this->clearTextureCache_();

        if( m_resourceFigma != nullptr )
        {
            m_resourceFigma->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::_dispose()
    {
        m_resourceFigma = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Figma::update( const UpdateContext * _context )
    {
        if( m_player == nullptr )
        {
            return;
        }

        const float dt = _context->time * 0.001f;

        ::Figma::EResult result = m_player->update( dt );

        if( result != ::Figma::EResult::Ok )
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
        for( const Map<String, TextureCacheDesc>::value_type & value : m_textureCache )
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
    RenderTextureInterfacePtr Figma::createAssetTexture_( ::Figma::DocumentInterface * _document, const ::Figma::RenderBatchDesc & _batch ) const
    {
        if( _document == nullptr || _batch.textureKey.empty() == true )
        {
            return nullptr;
        }

        const ::Figma::AssetDesc * asset = _document->findAsset( _batch.textureKey );

        if( asset == nullptr || asset->bytes.empty() == true )
        {
            return nullptr;
        }

        ConstString codecType = ConstString::none();
        const String mime = Detail::toLowerString( Detail::makeString( ::Figma::FigmaStringView( asset->mime.data(), asset->mime.size() ) ) );
        const String path = Detail::toLowerString( Detail::makeString( ::Figma::FigmaStringView( asset->path.data(), asset->path.size() ) ) );

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

        void * buffer = memory->newBuffer( asset->bytes.size() );

        if( buffer == nullptr )
        {
            return nullptr;
        }

        Helper::memoryCopy( buffer, 0, asset->bytes.data(), 0, asset->bytes.size() );

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
            Vector<uint8_t> sourcePixels;
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

        image->unlock( locked, 0, successful );

        if( successful == false )
        {
            texture->release();
            return nullptr;
        }

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr Figma::createGeneratedTexture_( const ::Figma::RenderListInterface * _renderList, uint32_t _batchIndex, String * const _signature ) const
    {
        if( _renderList == nullptr || m_textRasterizer == nullptr )
        {
            return nullptr;
        }

        ::Figma::RenderGeneratedTextureDesc desc;

        if( _renderList->getGeneratedTexture( _batchIndex, &desc ) != ::Figma::EResult::Ok )
        {
            return nullptr;
        }

        Vector<uint8_t> pixels;
        uint32_t width = 0;
        uint32_t height = 0;

        if( m_textRasterizer->makeTextPixels( _renderList, _batchIndex, desc, m_viewportScale, &pixels, &width, &height, _signature ) == false )
        {
            return nullptr;
        }

        Detail::premultiplyPixels_( pixels.data(), width * 4, width, height );

        return this->createTextureFromPixels_( width, height, pixels.data(), width * 4 );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr Figma::getBatchTexture_( const ::Figma::RenderListInterface * _renderList, const ::Figma::RenderBatchDesc & _batch, uint32_t _batchIndex ) const
    {
        if( _batch.textureKey.empty() == true )
        {
            return nullptr;
        }

        String cacheKey;

        switch( _batch.textureType )
        {
        case ::Figma::ERenderTextureType::Asset:
            cacheKey = "asset:";
            break;
        case ::Figma::ERenderTextureType::Generated:
            cacheKey = "generated:";
            break;
        case ::Figma::ERenderTextureType::None:
        default:
            return nullptr;
        }

        cacheKey.append( _batch.textureKey.data(), _batch.textureKey.size() );

        Map<String, TextureCacheDesc>::iterator it_found = m_textureCache.find( cacheKey );

        if( _batch.textureType == ::Figma::ERenderTextureType::Asset )
        {
            if( it_found != m_textureCache.end() )
            {
                return it_found->second.texture;
            }

            const FigmaDataInterfacePtr & data = m_resourceFigma->getData();
            ::Figma::DocumentInterface * document = data->getFigmaDocument();

            TextureCacheDesc desc;
            desc.texture = this->createAssetTexture_( document, _batch );

            if( desc.texture == nullptr )
            {
                return nullptr;
            }

            m_textureCache.emplace( cacheKey, desc );

            return desc.texture;
        }

        String signature;
        RenderTextureInterfacePtr generatedTexture = this->createGeneratedTexture_( _renderList, _batchIndex, &signature );

        if( generatedTexture == nullptr )
        {
            return nullptr;
        }

        if( it_found != m_textureCache.end() )
        {
            TextureCacheDesc & cached = it_found->second;

            if( cached.signature == signature )
            {
                generatedTexture->release();
                return cached.texture;
            }

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

        const ::Figma::RenderListInterface * renderList = m_player->getRenderList();

        if( renderList == nullptr )
        {
            return;
        }

        const uint32_t batchCount = renderList->getBatchCount();

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
        Vector<const RenderScissorInterface *> scissorStack;
        scissorStack.reserve( batchCount );
        scissorStack.emplace_back( _context->scissor );

        for( uint32_t batchIndex = 0; batchIndex != batchCount; ++batchIndex )
        {
            ::Figma::RenderBatchDesc batch;
            ::Figma::EResult result = renderList->getBatch( batchIndex, &batch );

            if( result != ::Figma::EResult::Ok )
            {
                continue;
            }

            if( batch.batchType == ::Figma::ERenderBatchType::ClipBegin )
            {
                Detail::FigmaRenderScissorPtr scissor = Helper::makeFactorableUnique<Detail::FigmaRenderScissor>( MENGINE_DOCUMENT_FORWARD );
                scissor->setScissorViewport( wm, batch.clipRect, context.scissor );

                context.scissor = scissor.get();
                scissorStack.emplace_back( context.scissor );
                m_renderScissors.emplace_back( scissor );

                continue;
            }

            if( batch.batchType == ::Figma::ERenderBatchType::ClipEnd )
            {
                if( scissorStack.size() > 1 )
                {
                    scissorStack.pop_back();
                    context.scissor = scissorStack.back();
                }

                continue;
            }

            if( batch.batchType != ::Figma::ERenderBatchType::Geometry )
            {
                continue;
            }

            if( batch.vertexCount == 0 || batch.indexCount == 0 )
            {
                continue;
            }

            RenderTextureInterfacePtr texture;

            if( batch.shaderType == ::Figma::ERenderShaderType::Texture )
            {
                texture = this->getBatchTexture_( renderList, batch, batchIndex );

                if( texture == nullptr )
                {
                    continue;
                }
            }
            else if( batch.shaderType != ::Figma::ERenderShaderType::Color || batch.textureType != ::Figma::ERenderTextureType::None )
            {
                continue;
            }

            const float batchAlpha = batch.opacity * batch.renderLayerOpacity;

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
            vertices.resize( batch.vertexCount );

            for( uint32_t vertexIndex = 0; vertexIndex != batch.vertexCount; ++vertexIndex )
            {
                const ::Figma::RenderVertex & figmaVertex = batch.vertices[vertexIndex];
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
            indices.resize( batch.indexCount );

            for( uint32_t index = 0; index != batch.indexCount; ++index )
            {
                indices[index] = static_cast<RenderIndex>(batch.indices[index]);
            }

            EMaterialBlendMode blendMode = Detail::getFigmaMaterialBlendMode( batch.blendMode );

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

            _renderPipeline->addRenderObject( &context, material, nullptr, vertices.data(), batch.vertexCount, indices.data(), batch.indexCount, nullptr, false, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputPointer_( ::Figma::EPointerEventType _type, float _x, float _y, ::Figma::EPointerButton _button )
    {
        if( m_player == nullptr )
        {
            return false;
        }

        ::Figma::PointerEvent event;
        event.type = _type;
        event.x = _x;
        event.y = _y;
        event.button = _button;
        event.modifiers = ::Figma::EInputModifierFlag::None;

        ::Figma::EResult result = m_player->inputPointer( event );

        return result == ::Figma::EResult::Ok;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Figma::inputKey_( ::Figma::EKeyEventType _type, uint32_t _keyCode )
    {
        if( m_player == nullptr )
        {
            return false;
        }

        ::Figma::KeyEvent event;
        event.type = _type;
        event.keyCode = _keyCode;
        event.modifiers = ::Figma::EInputModifierFlag::None;

        ::Figma::EResult result = m_player->inputKey( event );

        return result == ::Figma::EResult::Ok;
    }
    //////////////////////////////////////////////////////////////////////////
}
