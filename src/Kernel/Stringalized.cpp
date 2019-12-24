#include "Stringalized.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

#include "Config/StdIO.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, bool * _value )
        {
            uint32_t tmp_value;
            if( MENGINE_SSCANF( _string, "%" SCNu32, &tmp_value ) != 1 )
            {
                return false;
            }

            *_value = tmp_value != 0;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, int8_t * _value )
        {
            if( MENGINE_SSCANF( _string, "%" SCNd8, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, uint8_t * _value )
        {
            if( MENGINE_SSCANF( _string, "%" SCNu8, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, int32_t * _value )
        {
            if( MENGINE_SSCANF( _string, "%" SCNd32, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, uint32_t * _value )
        {
            if( MENGINE_SSCANF( _string, "%" SCNu32, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, int64_t * _value )
        {
            if( MENGINE_SSCANF( _string, "%" SCNd64, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, uint64_t * _value )
        {
            if( MENGINE_SSCANF( _string, "%" SCNu64, _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, float * _value )
        {
            if( MENGINE_SSCANF( _string, "%f", _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, double * _value )
        {
            if( MENGINE_SSCANF( _string, "%lf", _value ) != 1 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, const Char ** _value )
        {
            *_value = _string;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, String * _value )
        {
            _value->assign( _string );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, ConstString * _value )
        {
            *_value = Helper::stringizeString( _string );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, FilePath * _value )
        {
            *_value = Helper::stringizeFilePath( _string );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, Tags * _value )
        {
            const Char * it_tag = nullptr;

            for( const Char
                * it = _string,
                *it_end = _string + ::strlen( _string );
                it != it_end;
                ++it )
            {
                if( *it == ' ' )
                {
                    if( it_tag != nullptr )
                    {
                        ConstString cstr_value = Helper::stringizeStringSize( it_tag, it - it_tag );

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
        bool stringalized( const Char * _string, Resolution * _value )
        {
            uint32_t width;
            uint32_t height;
            if( MENGINE_SSCANF( _string, "%" SCNu32" %" SCNu32, &width, &height ) != 2 )
            {
                return false;
            }

            _value->setWidth( width );
            _value->setHeight( height );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, Color * _value )
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
        bool stringalized( const Char * _string, AspectRatioViewport * _value )
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
        bool stringalized( bool _value, Char * _string, size_t _capacity )
        {
            uint32_t tmp_value = _value;
            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRIu32, tmp_value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( int8_t _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRId8, _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( uint8_t _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRIu8, _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( int32_t _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRId32, _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( uint32_t _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRIu32, _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( int64_t _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRId64, _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( uint64_t _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRIu64, _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( float _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%g", _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( double _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%lf", _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const String & _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value.c_str() ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const ConstString & _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value.c_str() ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const FilePath & _value, Char * _string, size_t _capacity )
        {
            if( MENGINE_SNPRINTF( _string, _capacity, "%s", _value.c_str() ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Tags & _value, Char * _string, size_t _capacity )
        {
            int offset = 0;

            for( const ConstString & tag : _value )
            {
                offset += MENGINE_SNPRINTF( _string + offset, _capacity - offset, "%s ", tag.c_str() );

                if( offset >= (int)_capacity )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Resolution & _value, Char * _string, size_t _capacity )
        {
            uint32_t width = _value.getWidth();
            uint32_t height = _value.getHeight();

            if( MENGINE_SNPRINTF( _string, _capacity, "%" PRIu32" %" PRIu32, width, height ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Color & _value, Char * _string, size_t _capacity )
        {
            float r = _value.getR();
            float g = _value.getG();
            float b = _value.getB();
            float a = _value.getA();

            if( MENGINE_SNPRINTF( _string, _capacity, "%f %f %f %f", r, g, b, a ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const AspectRatioViewport & _value, Char * _string, size_t _capacity )
        {
            float width = _value.width;
            float height = _value.height;
            float bx = _value.viewport.begin.x;
            float by = _value.viewport.begin.y;
            float ex = _value.viewport.end.x;
            float ey = _value.viewport.end.y;

            if( MENGINE_SNPRINTF( _string, _capacity, "%f %f %f %f %f %f", width, height, bx, by, ex, ey ) >= (int)_capacity )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}