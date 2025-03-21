#include "MathScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/String.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Color.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/Polygon.h"
#include "Kernel/Rect.h"

#include "Config/StdMath.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/plane.h"
#include "math/uv4.h"
#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static String vec2f_repr( mt::vec2f * _v )
        {
            Stringstream ss;
            ss << "<vec2f: " << _v->x << ", " << _v->y << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static String vec3f_repr( mt::vec3f * _v )
        {
            Stringstream ss;
            ss << "<vec3f: " << _v->x << ", " << _v->y << ", " << _v->z << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static String vec4f_repr( mt::vec4f * _v )
        {
            Stringstream ss;
            ss << "<vec4f: " << _v->x << ", " << _v->y << ", " << _v->z << ", " << _v->w << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static String mat4f_repr( mt::mat4f * _v )
        {
            Stringstream ss;
            ss << "<mat4f: "
                << _v->v0.x << ", " << _v->v0.y << ", " << _v->v0.z << ", " << _v->v0.w
                << _v->v1.x << ", " << _v->v1.y << ", " << _v->v1.z << ", " << _v->v1.w
                << _v->v2.x << ", " << _v->v2.y << ", " << _v->v2.z << ", " << _v->v2.w
                << _v->v3.x << ", " << _v->v3.y << ", " << _v->v3.z << ", " << _v->v3.w
                << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static String uv4f_repr( mt::uv4f * _v )
        {
            Stringstream ss;
            ss << "<uv4f: "
                << "(" << _v->p0.x << ", " << _v->p0.y << ")"
                << ", (" << _v->p1.x << ", " << _v->p1.y << ")"
                << ", (" << _v->p2.x << ", " << _v->p2.y << ")"
                << ", (" << _v->p3.x << ", " << _v->p3.y << ")"
                << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool vec2f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec2f * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                size_t size = _kernel->tuple_size( _obj );

                switch( size )
                {
                case 2:
                    {
                        _place->x = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                    }break;
                case 3:
                    {
                        _place->x = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                    }break;
                default:
                    {
                        return false;
                    }break;
                }

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                size_t size = _kernel->list_size( _obj );

                switch( size )
                {
                case 2:
                    {
                        _place->x = pybind::list_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::list_getitem_t( _kernel, _obj, 1 );
                    }break;
                case 3:
                    {
                        _place->x = pybind::list_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::list_getitem_t( _kernel, _obj, 1 );
                    }break;
                default:
                    {
                        return false;
                    }break;
                }

                return true;
            }

            if( pybind::instance_of<mt::vec3f>( _kernel, _obj ) == true )
            {
                mt::vec3f v3 = pybind::extract_t( _kernel, _obj );

                _place->x = v3.x;
                _place->y = v3.y;

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool vec3f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec3f * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                size_t size = _kernel->tuple_size( _obj );

                switch( size )
                {
                case 2:
                    {
                        _place->x = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                        _place->z = 0.f;
                    }break;
                case 3:
                    {
                        _place->x = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                        _place->z = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                    }break;
                default:
                    {
                        return false;
                    }break;
                }

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                size_t size = _kernel->list_size( _obj );

                switch( size )
                {
                case 2:
                    {
                        _place->x = pybind::list_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::list_getitem_t( _kernel, _obj, 1 );
                        _place->z = 0.f;
                    }break;
                case 3:
                    {
                        _place->x = pybind::list_getitem_t( _kernel, _obj, 0 );
                        _place->y = pybind::list_getitem_t( _kernel, _obj, 1 );
                        _place->z = pybind::list_getitem_t( _kernel, _obj, 2 );
                    }break;
                default:
                    {
                        return false;
                    }break;
                }

                return true;
            }

            if( pybind::instance_of<mt::vec2f>( _kernel, _obj ) == true )
            {
                mt::vec2f v2 = pybind::extract_t( _kernel, _obj );

                _place->x = v2.x;
                _place->y = v2.y;
                _place->z = 0.f;

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool vec4f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec4f * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->x = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                _place->y = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                _place->z = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                _place->w = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->x = pybind::list_getitem_t( _kernel, _obj, 0 );
                _place->y = pybind::list_getitem_t( _kernel, _obj, 1 );
                _place->z = pybind::list_getitem_t( _kernel, _obj, 2 );
                _place->w = pybind::list_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool mat4f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::mat4f * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->v0 = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                _place->v1 = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                _place->v2 = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                _place->v3 = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->v0 = pybind::list_getitem_t( _kernel, _obj, 0 );
                _place->v1 = pybind::list_getitem_t( _kernel, _obj, 1 );
                _place->v2 = pybind::list_getitem_t( _kernel, _obj, 2 );
                _place->v3 = pybind::list_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool uv4f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::uv4f * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->p0 = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                _place->p1 = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                _place->p2 = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                _place->p3 = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->p0 = pybind::list_getitem_t( _kernel, _obj, 0 );
                _place->p1 = pybind::list_getitem_t( _kernel, _obj, 1 );
                _place->p2 = pybind::list_getitem_t( _kernel, _obj, 2 );
                _place->p3 = pybind::list_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static String box2f_repr( mt::box2f * _v )
        {
            Stringstream ss;
            ss << "<box2f: " << _v->minimum.x << ", " << _v->minimum.y << ", " << _v->maximum.x << ", " << _v->maximum.y << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool box2f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::box2f * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->minimum.x = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                _place->minimum.y = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                _place->maximum.x = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                _place->maximum.y = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->minimum.x = pybind::list_getitem_t( _kernel, _obj, 0 );
                _place->minimum.y = pybind::list_getitem_t( _kernel, _obj, 1 );
                _place->maximum.x = pybind::list_getitem_t( _kernel, _obj, 2 );
                _place->maximum.y = pybind::list_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Polygon_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * const _place )
        {
            if( _kernel->list_check( _obj ) == false )
            {
                return false;
            }

            Polygon * polygon = (Polygon *)_place;

            pybind::list py_list( _kernel, _obj );

            for( const mt::vec2f & point : py_list )
            {
                polygon->append( point );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static String color_repr( Color * _v )
        {
            Stringstream ss;
            ss << "<color: " << _v->getA() << ", " << _v->getR() << ", " << _v->getG() << ", " << _v->getB() << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool color_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Color * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) == 4 )
                {
                    float r = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                    float a = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                    _place->setRGBA( r, g, b, a );

                    return true;
                }

                if( _kernel->tuple_size( _obj ) == 3 )
                {
                    float r = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                    float a = 1.f;

                    _place->setRGBA( r, g, b, a );

                    return true;
                }

                return false;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) == 4 )
                {
                    float r = pybind::list_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::list_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::list_getitem_t( _kernel, _obj, 2 );
                    float a = pybind::list_getitem_t( _kernel, _obj, 3 );

                    _place->setRGBA( r, g, b, a );

                    return true;
                }
                else if( _kernel->list_size( _obj ) == 3 )
                {
                    float r = pybind::list_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::list_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::list_getitem_t( _kernel, _obj, 2 );
                    float a = 1.f;

                    _place->setRGBA( r, g, b, a );

                    return true;
                }

                return false;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Resolution_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Resolution * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 2 )
                {
                    return false;
                }

                uint32_t width = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                uint32_t height = pybind::tuple_getitem_t( _kernel, _obj, 1 );

                _place->setWidth( width );
                _place->setHeight( height );

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 2 )
                {
                    return false;
                }

                uint32_t width = pybind::list_getitem_t( _kernel, _obj, 0 );
                uint32_t height = pybind::list_getitem_t( _kernel, _obj, 1 );

                _place->setWidth( width );
                _place->setHeight( height );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static String Resolution_repr( Resolution * _v )
        {
            Stringstream ss;
            ss << "<Resolution: " << _v->getWidth() << ", " << _v->getHeight() << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Viewport_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Viewport * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 2 )
                {
                    return false;
                }

                mt::vec2f begin = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                mt::vec2f end = pybind::tuple_getitem_t( _kernel, _obj, 1 );

                _place->setRectangle( begin, end );

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 2 )
                {
                    return false;
                }

                mt::vec2f begin = pybind::list_getitem_t( _kernel, _obj, 0 );
                mt::vec2f end = pybind::list_getitem_t( _kernel, _obj, 1 );

                _place->setRectangle( begin, end );

                return true;
            }

            Resolution r;
            if( pybind::extract_value( _kernel, _obj, r, true ) == true )
            {
                mt::vec2f begin;
                begin.x = 0.f;
                begin.y = 0.f;

                mt::vec2f end;
                r.calcSize( &end );

                _place->setRectangle( begin, end );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static String Viewport_repr( Viewport * _v )
        {
            Stringstream ss;
            ss << "<Viewport: " << _v->begin.x << ", " << _v->begin.y << ", " << _v->end.x << ", " << _v->end.y << ", " << ">";

            String repr = ss.str();

            return repr;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Rect_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Rect * const _place )
        {
            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->left = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                _place->top = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                _place->right = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                _place->bottom = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 4 )
                {
                    return false;
                }

                _place->left = pybind::list_getitem_t( _kernel, _obj, 0 );
                _place->top = pybind::list_getitem_t( _kernel, _obj, 1 );
                _place->right = pybind::list_getitem_t( _kernel, _obj, 2 );
                _place->bottom = pybind::list_getitem_t( _kernel, _obj, 3 );

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t rounding( uint32_t _value, uint32_t _round )
        {
            uint32_t floor_value = _value / _round;
            uint32_t round_value = floor_value * _round;

            return round_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float roundingf( float _value, float _round )
        {
            float floor_value = StdMath::floorf( _value / _round );
            float round_value = floor_value * _round;

            return round_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float pi()
        {
            return mt::constant::pi;
        }
        //////////////////////////////////////////////////////////////////////////
        static float deg2rad( float _deg )
        {
            return _deg * mt::constant::deg2rad;
        }
        //////////////////////////////////////////////////////////////////////////
        static float rad2deg( float _rad )
        {
            return _rad * mt::constant::rad2deg;
        }
        //////////////////////////////////////////////////////////////////////////
        static float sqrtf( float a )
        {
            return StdMath::sqrtf( a );
        }
        //////////////////////////////////////////////////////////////////////////
        static float powf( float a, float b )
        {
            return StdMath::powf( a, b );
        }
        //////////////////////////////////////////////////////////////////////////
        static float fabsf( float a )
        {
            return StdMath::fabsf( a );
        }
        //////////////////////////////////////////////////////////////////////////
        static float cosf( float a )
        {
            return StdMath::cosf( a );
        }
        //////////////////////////////////////////////////////////////////////////
        static float sinf( float a )
        {
            return StdMath::sinf( a );
        }
        //////////////////////////////////////////////////////////////////////////
        static float cosf_deg( float a )
        {
            return StdMath::cosf( a * mt::constant::deg2rad );
        }
        //////////////////////////////////////////////////////////////////////////
        static float sinf_deg( float a )
        {
            return StdMath::sinf( a * mt::constant::deg2rad );
        }
        //////////////////////////////////////////////////////////////////////////
        static float tanf( float a )
        {
            return StdMath::tanf( a );
        }
        //////////////////////////////////////////////////////////////////////////
        static float acosf( float a )
        {
            return StdMath::acosf( a );
        }
        //////////////////////////////////////////////////////////////////////////
        static float atanf( float _x )
        {
            return StdMath::atan( _x );
        }
        //////////////////////////////////////////////////////////////////////////
        static float logf( float _x )
        {
            return StdMath::logf( _x );
        }
        //////////////////////////////////////////////////////////////////////////
        static float log10f( float _x )
        {
            return StdMath::log10f( _x );
        }
        //////////////////////////////////////////////////////////////////////////
        static double log10( double _x )
        {
            return StdMath::log10( _x );
        }
        //////////////////////////////////////////////////////////////////////////
        static float dot_v2_v2( const mt::vec2f & _a, const mt::vec2f & _b )
        {
            float d = mt::dot_v2_v2( _a, _b );

            return d;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f direction_v2_v2( const mt::vec2f & _from, const mt::vec2f & _to )
        {
            mt::vec2f direction;
            mt::dir_v2_v2( &direction, _to, _from );

            return direction;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f direction_v3_v3( const mt::vec3f & _from, const mt::vec3f & _to )
        {
            mt::vec3f direction;
            mt::dir_v3_v3( &direction, _from, _to );

            return direction;
        }
        //////////////////////////////////////////////////////////////////////////
        static float angle_from_v2_v2( const mt::vec2f & _from, const mt::vec2f & _to )
        {
            mt::vec2f direction;
            mt::sub_v2_v2( &direction, _from, _to );

            mt::vec2f direction_n;
            mt::norm_v2_v2( &direction_n, direction );

            float angle = mt::signed_angle( direction_n );

            return angle;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f norm_v2( const mt::vec2f & _vec )
        {
            mt::vec2f v_n;
            mt::norm_v2_v2( &v_n, _vec );

            return v_n;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f norm_v3( const mt::vec3f & _vec )
        {
            mt::vec3f v_n;
            mt::norm_v3_v3( &v_n, _vec );

            return v_n;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec4f norm_v4( const mt::vec4f & _vec )
        {
            mt::vec4f v_n;
            mt::norm_v4_v4( &v_n, _vec );

            return v_n;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MathScriptEmbedding::MathScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MathScriptEmbedding::~MathScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MathScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::registration_stl_vector_type_cast<Vector<mt::vec2f>>(_kernel);
        pybind::registration_stl_vector_type_cast<Vector<Polygon>>(_kernel);

        pybind::struct_<mt::vec2f>( _kernel, "vec2f" )
            .def_constructor( pybind::init<float, float>() )
            .def_compare_equal()
            .def_convert( &Detail::vec2f_convert )
            .def_repr( &Detail::vec2f_repr )
            .def_operator_getset()
            .def_operator_add()
            .def_operator_sub()
            .def_operator_mul()
            .def_operator_div()
            .def_operator_inplace_add()
            .def_operator_inplace_sub()
            .def_operator_inplace_mul()
            .def_operator_inplace_div()
            .def_operator_inplace_mul_t<float>()
            .def_operator_inplace_div_t<float>()
            .def_operator_inplace_mul_t<float, int32_t>()
            .def_operator_inplace_div_t<float, int32_t>()
            .def_operator_mul_t<float>()
            .def_operator_div_t<float>()
            .def_operator_mul_t<float, int32_t>()
            .def_operator_div_t<float, int32_t>()
            .def_abs( &mt::vec2f::length )
            .def_member( "x", &mt::vec2f::x )
            .def_member( "y", &mt::vec2f::y )
            .def( "set", &mt::vec2f::set )
            .def( "sqrlength", &mt::vec2f::sqrlength )
            .def( "length", &mt::vec2f::length )
            ;

        pybind::struct_<mt::vec3f>( _kernel, "vec3f" )
            .def_constructor( pybind::init<float, float, float>() )
            .def_compare_equal()
            .def_convert( &Detail::vec3f_convert )
            .def_repr( &Detail::vec3f_repr )
            .def_operator_getset()
            .def_operator_add()
            .def_operator_sub()
            .def_operator_mul()
            .def_operator_div()
            .def_operator_inplace_add()
            .def_operator_inplace_sub()
            .def_operator_inplace_mul()
            .def_operator_inplace_div()
            .def_operator_inplace_mul_t<float>()
            .def_operator_inplace_div_t<float>()
            .def_operator_inplace_mul_t<float, int32_t>()
            .def_operator_inplace_div_t<float, int32_t>()
            .def_operator_mul_t<float>()
            .def_operator_div_t<float>()
            .def_operator_mul_t<float, int32_t>()
            .def_operator_div_t<float, int32_t>()
            .def_abs( &mt::vec3f::length )
            .def_member( "x", &mt::vec3f::x )
            .def_member( "y", &mt::vec3f::y )
            .def_member( "z", &mt::vec3f::z )
            .def( "set", &mt::vec3f::set )
            .def( "sqrlength", &mt::vec3f::sqrlength )
            .def( "length", &mt::vec3f::length )
            ;

        pybind::struct_<mt::vec4f>( _kernel, "vec4f" )
            .def_constructor( pybind::init<float, float, float, float>() )
            .def_compare_equal()
            .def_convert( &Detail::vec4f_convert )
            .def_repr( &Detail::vec4f_repr )
            .def_operator_getset()
            .def_operator_add()
            .def_operator_sub()
            .def_operator_mul()
            .def_operator_div()
            .def_operator_inplace_add()
            .def_operator_inplace_sub()
            .def_operator_inplace_mul()
            .def_operator_inplace_div()
            .def_operator_inplace_mul_t<float>()
            .def_operator_inplace_div_t<float>()
            .def_operator_inplace_mul_t<float, int32_t>()
            .def_operator_inplace_div_t<float, int32_t>()
            .def_operator_mul_t<float>()
            .def_operator_div_t<float>()
            .def_operator_mul_t<float, int32_t>()
            .def_operator_div_t<float, int32_t>()
            .def_abs( &mt::vec4f::length )
            .def_member( "x", &mt::vec4f::x )
            .def_member( "y", &mt::vec4f::y )
            .def_member( "z", &mt::vec4f::z )
            .def_member( "w", &mt::vec4f::w )
            .def( "set", &mt::vec4f::set )
            .def( "sqrlength", &mt::vec4f::sqrlength )
            .def( "length", &mt::vec4f::length )
            ;

        pybind::struct_<mt::mat4f>( _kernel, "mat4f" )
            .def_constructor( pybind::init<>() )
            .def_compare_equal()
            .def_convert( &Detail::mat4f_convert )
            .def_repr( &Detail::mat4f_repr )
            .def_operator_getset()
            .def_operator_add()
            .def_operator_sub()
            .def_operator_mul()
            .def_operator_inplace_add()
            .def_operator_inplace_sub()
            .def_operator_inplace_mul()
            .def_member( "v0", &mt::mat4f::v0 )
            .def_member( "v1", &mt::mat4f::v1 )
            .def_member( "v2", &mt::mat4f::v2 )
            .def_member( "v3", &mt::mat4f::v3 )
            ;

        pybind::struct_<mt::uv4f>( _kernel, "uv4f" )
            .def_constructor( pybind::init<mt::vec2f, mt::vec2f, mt::vec2f, mt::vec2f>() )
            .def_compare_equal()
            .def_convert( &Detail::uv4f_convert )
            .def_repr( &Detail::uv4f_repr )
            .def_operator_getset()
            .def_member( "p0", &mt::uv4f::p0 )
            .def_member( "p1", &mt::uv4f::p1 )
            .def_member( "p2", &mt::uv4f::p2 )
            .def_member( "p3", &mt::uv4f::p3 )
            .def( "set", &mt::uv4f::set )
            ;

        pybind::struct_<mt::box2f>( _kernel, "box2f" )
            .def_constructor( pybind::init<mt::vec2f, mt::vec2f>() )
            .def_convert( &Detail::box2f_convert )
            .def_repr( &Detail::box2f_repr )
            .def_member( "minimum", &mt::box2f::minimum )
            .def_member( "maximum", &mt::box2f::maximum )
            ;

        pybind::struct_<Rect>( _kernel, "Rect" )
            .def_constructor( pybind::init<uint32_t, uint32_t, uint32_t, uint32_t>() )
            .def_convert( &Detail::Rect_convert )
            .def_member( "left", &Rect::left )
            .def_member( "top", &Rect::top )
            .def_member( "right", &Rect::right )
            .def_member( "bottom", &Rect::bottom )
            .def( "getWidth", &Rect::getWidth )
            .def( "getHeight", &Rect::getHeight )
            ;


        pybind::struct_<mt::quatf>( _kernel, "quatf" )
            .def_constructor( pybind::init<float, float, float, float>() )
            .def_compare_equal()
            .def_member( "w", &mt::quatf::w )
            .def_member( "x", &mt::quatf::x )
            .def_member( "y", &mt::quatf::y )
            .def_member( "z", &mt::quatf::z )
            ;

        pybind::struct_<mt::planef>( _kernel, "planef" )
            .def_constructor( pybind::init<float, float, float, float>() )
            .def_member( "a", &mt::planef::a )
            .def_member( "b", &mt::planef::b )
            .def_member( "c", &mt::planef::c )
            .def_member( "d", &mt::planef::d )
            ;

        pybind::struct_<Polygon>( _kernel, "Polygon" )
            .def_constructor( pybind::init<>() )
            .def_convert( &Detail::Polygon_convert )
            .def( "getPoints", &Polygon::getPoints )
            .def( "empty", &Polygon::empty )
            .def( "size", &Polygon::size )
            .def( "area", &Polygon::area )
            .def( "append", &Polygon::append )
            .def( "clear", &Polygon::clear )
            ;

        pybind::struct_<Viewport>( _kernel, "Viewport" )
            .def_constructor( pybind::init<mt::vec2f, mt::vec2f>() )
            .def_convert( &Detail::Viewport_convert )
            .def_repr( &Detail::Viewport_repr )
            .def_member( "begin", &Viewport::begin )
            .def_member( "end", &Viewport::end )
            .def( "getWidth", &Viewport::getWidth )
            .def( "getHeight", &Viewport::getHeight )
            .def( "getSquare", &Viewport::getSquare )
            .def( "parallax", &Viewport::parallax )
            .def( "scale", &Viewport::scale )
            .def( "inflate", &Viewport::inflate )
            .def( "testPoint", &Viewport::testPoint )
            .def( "testBBox", &Viewport::testBBox )
            .def( "testRectangle", &Viewport::testRectangle )
            .def( "existBBox", &Viewport::existBBox )
            .def( "existRectangle", &Viewport::existRectangle )
            .def( "intersectBBox", &Viewport::intersectBBox )
            .def( "getIntersectionSquareBBox", &Viewport::getIntersectionSquareBBox )
            ;

        pybind::struct_<Color>( _kernel, "Color" )
            .def_constructor( pybind::init<float, float, float, float>() )
            .def_convert( &Detail::color_convert )
            .def_repr( &Detail::color_repr )
            .def( "getA", &Color::getA )
            .def( "getR", &Color::getR )
            .def( "getG", &Color::getG )
            .def( "getB", &Color::getB )
            .def_property( "a", &Color::getA, &Color::setA )
            .def_property( "r", &Color::getR, &Color::setR )
            .def_property( "g", &Color::getG, &Color::setG )
            .def_property( "b", &Color::getB, &Color::setB )
            .def( "isSolid", &Color::isSolid )
            .def( "isIdentity", &Color::isIdentity )
            .def( "isSolid", &Color::isSolid )
            ;

        pybind::struct_<Resolution>( _kernel, "Resolution" )
            .def_constructor( pybind::init<uint32_t, uint32_t>() )
            .def_convert( &Detail::Resolution_convert )
            .def_repr( &Detail::Resolution_repr )
            .def( "setWidth", &Resolution::setWidth )
            .def( "setHeight", &Resolution::setHeight )
            .def( "getWidth", &Resolution::getWidth )
            .def( "getHeight", &Resolution::getHeight )
            .def( "getWidthF", &Resolution::getWidthF )
            .def( "getHeightF", &Resolution::getHeightF )
            .def( "getAspectRatio", &Resolution::getAspectRatio )
            ;

        pybind::def_function( _kernel, "fibo", &mt::fibo64 );
        pybind::def_function( _kernel, "fibo_bine", &mt::fibo_bine );
        pybind::def_function( _kernel, "factorial", &mt::factorial64 );
        pybind::def_function( _kernel, "factorialf", &mt::factorialf );
        pybind::def_function( _kernel, "integral_pow", &mt::integral_pow64 );
        pybind::def_function( _kernel, "integral_powf", &mt::integral_powf );
        pybind::def_function( _kernel, "is_pow2", &mt::is_pow2 );
        pybind::def_function( _kernel, "next_pow2", &mt::next_pow2 );
        pybind::def_function( _kernel, "get_np2", &mt::get_np2 );

        pybind::def_function( _kernel, "rounding", &Detail::rounding );
        pybind::def_function( _kernel, "roundingf", &Detail::roundingf );

        pybind::def_function( _kernel, "pi", &Detail::pi );
        pybind::def_function( _kernel, "deg2rad", &Detail::deg2rad );
        pybind::def_function( _kernel, "rad2deg", &Detail::rad2deg );

        pybind::def_function( _kernel, "sqrtf", &Detail::sqrtf );
        pybind::def_function( _kernel, "powf", &Detail::powf );
        pybind::def_function( _kernel, "fabsf", &Detail::fabsf );
        pybind::def_function( _kernel, "cosf", &Detail::cosf );
        pybind::def_function( _kernel, "sinf", &Detail::sinf );
        pybind::def_function( _kernel, "tanf", &Detail::tanf );
        pybind::def_function( _kernel, "cosf_deg", &Detail::cosf_deg );
        pybind::def_function( _kernel, "sinf_deg", &Detail::sinf_deg );
        pybind::def_function( _kernel, "acosf", &Detail::acosf );
        pybind::def_function( _kernel, "atanf", &Detail::atanf );
        pybind::def_function( _kernel, "logf", &Detail::logf );
        pybind::def_function( _kernel, "log10f", &Detail::log10f );
        pybind::def_function( _kernel, "log10", &Detail::log10 );
        pybind::def_function( _kernel, "dot_v2_v2", &Detail::dot_v2_v2 );
        pybind::def_function( _kernel, "direction_v2_v2", &Detail::direction_v2_v2 );
        pybind::def_function( _kernel, "direction_v3_v3", &Detail::direction_v3_v3 );
        pybind::def_function( _kernel, "angle_from_v2_v2", &Detail::angle_from_v2_v2 );
        pybind::def_function( _kernel, "norm_v2", &Detail::norm_v2 );
        pybind::def_function( _kernel, "norm_v3", &Detail::norm_v3 );
        pybind::def_function( _kernel, "norm_v4", &Detail::norm_v4 );

        pybind::def_function( _kernel, "angle_norm", &mt::angle_norm );
        pybind::def_function( _kernel, "angle_delta_deg", &mt::angle_delta_deg );

        pybind::def_function( _kernel, "length_v2", &mt::length_v2 );

        pybind::def_function( _kernel, "length_v2_v2", &mt::length_v2_v2 );
        pybind::def_function( _kernel, "sqrlength_v2_v2", &mt::sqrlength_v2_v2 );

        pybind::def_function( _kernel, "length_v3_v3", &mt::length_v3_v3 );
        pybind::def_function( _kernel, "sqrlength_v3_v3", &mt::sqrlength_v3_v3 );

        pybind::def_function( _kernel, "length_v3", &mt::length_v3 );
        pybind::def_function( _kernel, "sqrlength_v3", &mt::sqrlength_v3 );

        pybind::def_function( _kernel, "signed_angle", &mt::signed_angle );
        pybind::def_function( _kernel, "angle_length", &mt::angle_length );
        pybind::def_function( _kernel, "perp_v2", &mt::perp );
        pybind::def_function( _kernel, "perp_left", &mt::perp_left );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MathScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<Vector<mt::vec2f>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<Vector<Polygon>>(_kernel);
    }
    //////////////////////////////////////////////////////////////////////////
}