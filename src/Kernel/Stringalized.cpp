#include "Stringalized.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"
#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, bool * const _value )
        {
            uint8_t tmp_value;
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNu8, &tmp_value ) != 1 )
            {
                return false;
            }

            *_value = tmp_value != 0;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, int8_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNd8, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, uint8_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNu8, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, int16_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNd16, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, uint16_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNu16, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, int32_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNd32, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, uint32_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNu32, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, int64_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNd64, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, uint64_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNu64, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_STRINGALIZED_SIZE_T)
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, size_t * const _value )
        {
            if( MENGINE_SSCANF( _string, "%zu", _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, float * const _value )
        {
            if( MENGINE_SSCANF( _string, "%f", _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, double * const _value )
        {
            if( MENGINE_SSCANF( _string, "%lf", _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, mt::vec2f * const _value )
        {
            if( MENGINE_SSCANF( _string, "%f %f", &_value->x, &_value->y ) != 2 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, mt::vec3f * const _value )
        {
            if( MENGINE_SSCANF( _string, "%f %f %f", &_value->x, &_value->y, &_value->z ) != 3 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, mt::vec4f * const _value )
        {
            if( MENGINE_SSCANF( _string, "%f %f %f %f", &_value->x, &_value->y, &_value->z, &_value->w ) != 4 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, const Char ** const _value )
        {
            *_value = _string;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, String * const _value )
        {
            _value->assign( _string );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, ConstString * const _value )
        {
            *_value = Helper::stringizeString( _string );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, FilePath * const _value )
        {
            *_value = Helper::stringizeFilePath( _string );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, Tags * const _value )
        {
            const Char * it_tag = nullptr;

            for( const Char
                * it = _string,
                *it_end = _string + MENGINE_STRLEN( _string );
                it != it_end;
                ++it )
            {
                if( *it == ' ' )
                {
                    if( it_tag != nullptr )
                    {
                        ConstString cstr_value = Helper::stringizeStringSize( it_tag, (ConstString::size_type)(it - it_tag) );

                        _value->addTag( cstr_value );

                        it_tag = nullptr;
                    }
                }
                else
                {
                    if( it_tag == nullptr )
                    {
                        it_tag = it;
                    }
                }
            }

            if( it_tag != nullptr )
            {
                ConstString cstr_value = Helper::stringizeString( it_tag );

                _value->addTag( cstr_value );

                it_tag = nullptr;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, Resolution * const _value )
        {
            uint32_t width;
            uint32_t height;
            if( MENGINE_SSCANF( _string, "%" MENGINE_SCNu32" %" MENGINE_SCNu32, &width, &height ) != 2 )
            {
                return false;
            }

            _value->setWidth( width );
            _value->setHeight( height );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, Color * const _value )
        {
            float r;
            float g;
            float b;
            float a;
            if( MENGINE_SSCANF( _string, "%f %f %f %f", &r, &g, &b, &a ) != 4 )
            {
                return false;
            }

            const float coef = 1.f / 255.f;

            r *= coef;
            g *= coef;
            b *= coef;
            a *= coef;

            _value->setRGBA( r, g, b, a );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, AspectRatioViewport * const _value )
        {
            float width;
            float height;
            float bx;
            float by;
            float ex;
            float ey;

            if( MENGINE_SSCANF( _string, "%f %f %f %f %f %f", &width, &height, &bx, &by, &ex, &ey ) != 6 )
            {
                return false;
            }

            _value->width = width;
            _value->height = height;
            _value->viewport.begin.x = bx;
            _value->viewport.begin.y = by;
            _value->viewport.end.x = ex;
            _value->viewport.end.y = ey;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( bool _value, Char * const _string, size_t _capacity )
        {
            uint32_t tmp_value = _value;

            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRIu32, tmp_value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( int8_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRId8, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( uint8_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRIu8, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( int16_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRId16, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( uint16_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRIu16, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( int32_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRId32, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( uint32_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRIu32, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( int64_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRId64, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( uint64_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRIu64, _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_STRINGALIZED_SIZE_T)
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( size_t _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%zu", _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( float _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%g", _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( double _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%lf", _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const mt::vec2f & _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%g %g", _value.x, _value.y ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const mt::vec3f & _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%g %g %g", _value.x, _value.y, _value.z ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const mt::vec4f & _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%g %g %g %g", _value.x, _value.y, _value.z, _value.w ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const String & _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value.c_str() ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const ConstString & _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value.c_str() ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const FilePath & _value, Char * const _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value.c_str() ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Tags & _value, Char * const _string, size_t _capacity )
        {
            int32_t offset = 0;

            for( const ConstString & tag : _value )
            {
                offset += MENGINE_SNPRINTF( _string + offset, _capacity - offset, "%s ", tag.c_str() );

                if( offset > (int32_t)_capacity )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Resolution & _value, Char * const _string, size_t _capacity )
        {
            uint32_t width = _value.getWidth();
            uint32_t height = _value.getHeight();

            if( MENGINE_SNPRINTF( _string, _capacity, "%" MENGINE_PRIu32" %" MENGINE_PRIu32, width, height ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Color & _value, Char * const _string, size_t _capacity )
        {
            float r = _value.getR();
            float g = _value.getG();
            float b = _value.getB();
            float a = _value.getA();

            if( MENGINE_SNPRINTF( _string, _capacity, "%.16f %.16f %.16f %.16f", r, g, b, a ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const AspectRatioViewport & _value, Char * const _string, size_t _capacity )
        {
            float width = _value.width;
            float height = _value.height;
            float bx = _value.viewport.begin.x;
            float by = _value.viewport.begin.y;
            float ex = _value.viewport.end.x;
            float ey = _value.viewport.end.y;

            if( MENGINE_SNPRINTF( _string, _capacity, "%f %f %f %f %f %f", width, height, bx, by, ex, ey ) > (int32_t)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
