#include "MathScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Kernel/String.h"
#include "Kernel/Stringstream.h"

#include "pybind/pybind.hpp"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/plane.h"
#include "math/uv4.h"
#include "math/utils.h"

#include "Kernel/Color.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/Polygon.h"
#include "Kernel/Rect.h"

#include "pybind/stl/stl_type_cast.hpp"

namespace Mengine
{
    namespace ScriptMethod
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
        static bool vec2f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec2f * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
            else if( _kernel->list_check( _obj ) == true )
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
            else if( pybind::instance_of<mt::vec3f>( _kernel, _obj ) == true )
            {
                mt::vec3f v3 = pybind::extract_t( _kernel, _obj );

                _place->x = v3.x;
                _place->y = v3.y;

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool vec3f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec3f * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
            else if( _kernel->list_check( _obj ) == true )
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
            else if( pybind::instance_of<mt::vec2f>( _kernel, _obj ) == true )
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
        static bool vec4f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec4f * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
            else if( _kernel->list_check( _obj ) == true )
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
        static bool uv4f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::uv4f * _uv, void * const _user )
        {
            MENGINE_UNUSED( _user );

            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) != 4 )
                {
                    return false;
                }

                _uv->p0 = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                _uv->p1 = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                _uv->p2 = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                _uv->p3 = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                return true;
            }
            else if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) != 4 )
                {
                    return false;
                }

                _uv->p0 = pybind::list_getitem_t( _kernel, _obj, 0 );
                _uv->p1 = pybind::list_getitem_t( _kernel, _obj, 1 );
                _uv->p2 = pybind::list_getitem_t( _kernel, _obj, 2 );
                _uv->p3 = pybind::list_getitem_t( _kernel, _obj, 3 );

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
        static bool box2f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::box2f * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
            else if( _kernel->list_check( _obj ) == true )
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
        static bool Polygon_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
        static bool color_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Color * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

            if( _kernel->tuple_check( _obj ) == true )
            {
                if( _kernel->tuple_size( _obj ) == 4 )
                {
                    float r = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                    float a = pybind::tuple_getitem_t( _kernel, _obj, 3 );

                    _place->setRGBA( r, g, b, a );
                }
                else if( _kernel->tuple_size( _obj ) == 3 )
                {
                    float r = pybind::tuple_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::tuple_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::tuple_getitem_t( _kernel, _obj, 2 );
                    float a = 1.f;

                    _place->setRGBA( r, g, b, a );
                }
                else
                {
                    return false;
                }

                return true;
            }
            else if( _kernel->list_check( _obj ) == true )
            {
                if( _kernel->list_size( _obj ) == 4 )
                {
                    float r = pybind::list_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::list_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::list_getitem_t( _kernel, _obj, 2 );
                    float a = pybind::list_getitem_t( _kernel, _obj, 3 );

                    _place->setRGBA( r, g, b, a );
                }
                else if( _kernel->list_size( _obj ) == 3 )
                {
                    float r = pybind::list_getitem_t( _kernel, _obj, 0 );
                    float g = pybind::list_getitem_t( _kernel, _obj, 1 );
                    float b = pybind::list_getitem_t( _kernel, _obj, 2 );
                    float a = 1.f;

                    _place->setRGBA( r, g, b, a );
                }
                else
                {
                    return false;
                }

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Resolution_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Resolution * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
            else if( _kernel->list_check( _obj ) == true )
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
        static bool Viewport_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Viewport * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
            else if( _kernel->list_check( _obj ) == true )
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
        static bool Rect_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Rect * _place, void * const _user )
        {
            MENGINE_UNUSED( _user );

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
            else if( _kernel->list_check( _obj ) == true )
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
            .def_convert( &ScriptMethod::vec2f_convert, nullptr )
            .def_repr( &ScriptMethod::vec2f_repr )
            .def_operator_getset()
            .def_operator_add()
            .def_operator_sub()
            .def_operator_mul()
            .def_operator_div()
            .def_operator_mul_t<float, int32_t>()
            .def_operator_div_t<float, int32_t>()
            .def_operator_mul_t<float>()
            .def_operator_div_t<float>()
            .def_member( "x", &mt::vec2f::x )
            .def_member( "y", &mt::vec2f::y )
            ;

        pybind::struct_<mt::vec3f>( _kernel, "vec3f" )
            .def_constructor( pybind::init<float, float, float>() )
            .def_convert( &ScriptMethod::vec3f_convert, nullptr )
            .def_repr( &ScriptMethod::vec3f_repr )
            .def_operator_getset()
            .def_operator_add()
            .def_operator_sub()
            .def_operator_mul()
            .def_operator_div()
            .def_operator_mul_t<float, int32_t>()
            .def_operator_div_t<float, int32_t>()
            .def_operator_mul_t<float>()
            .def_operator_div_t<float>()
            .def_member( "x", &mt::vec3f::x )
            .def_member( "y", &mt::vec3f::y )
            .def_member( "z", &mt::vec3f::z )
            ;

        pybind::struct_<mt::vec4f>( _kernel, "vec4f" )
            .def_constructor( pybind::init<float, float, float, float>() )
            .def_convert( &ScriptMethod::vec4f_convert, nullptr )
            .def_repr( &ScriptMethod::vec4f_repr )
            .def_operator_getset()
            .def_operator_add()
            .def_operator_sub()
            .def_operator_mul()
            .def_operator_div()
            .def_operator_mul_t<float, int32_t>()
            .def_operator_div_t<float, int32_t>()
            .def_operator_mul_t<float>()
            .def_operator_div_t<float>()
            .def_member( "x", &mt::vec4f::x )
            .def_member( "y", &mt::vec4f::y )
            .def_member( "z", &mt::vec4f::z )
            .def_member( "w", &mt::vec4f::w )
            ;

        pybind::struct_<mt::uv4f>( _kernel, "uv4f" )
            .def_constructor( pybind::init<mt::vec2f, mt::vec2f, mt::vec2f, mt::vec2f>() )
            .def_convert( &ScriptMethod::uv4f_convert, nullptr )
            .def_repr( &ScriptMethod::uv4f_repr )
            .def_operator_getset()
            .def_member( "p0", &mt::uv4f::p0 )
            .def_member( "p1", &mt::uv4f::p1 )
            .def_member( "p2", &mt::uv4f::p2 )
            .def_member( "p3", &mt::uv4f::p3 )
            ;

        pybind::struct_<mt::box2f>( _kernel, "box2f" )
            .def_constructor( pybind::init<mt::vec2f, mt::vec2f>() )
            .def_convert( &ScriptMethod::box2f_convert, nullptr )
            .def_repr( &ScriptMethod::box2f_repr )
            .def_member( "minimum", &mt::box2f::minimum )
            .def_member( "maximum", &mt::box2f::maximum )
            ;

        pybind::struct_<Rect>( _kernel, "Rect" )
            .def_constructor( pybind::init<uint32_t, uint32_t, uint32_t, uint32_t>() )
            .def_convert( &ScriptMethod::Rect_convert, nullptr )
            .def_member( "left", &Rect::left )
            .def_member( "top", &Rect::top )
            .def_member( "right", &Rect::right )
            .def_member( "bottom", &Rect::bottom )
            .def( "getWidth", &Rect::getWidth )
            .def( "getHeight", &Rect::getHeight )
            ;


        pybind::struct_<mt::quatf>( _kernel, "quatf" )
            .def_constructor( pybind::init<float, float, float, float>() )
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
            .def_convert( &ScriptMethod::Polygon_convert, nullptr )
            .def( "getPoints", &Polygon::getPoints )
            .def( "empty", &Polygon::empty )
            .def( "size", &Polygon::size )
            .def( "area", &Polygon::area )
            .def( "append", &Polygon::append )
            .def( "clear", &Polygon::clear )
            ;

        pybind::struct_<Viewport>( _kernel, "Viewport" )
            .def_constructor( pybind::init<mt::vec2f, mt::vec2f>() )
            .def_convert( &ScriptMethod::Viewport_convert, nullptr )
            .def_repr( &ScriptMethod::Viewport_repr )
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
            .def_convert( &ScriptMethod::color_convert, nullptr )
            .def_repr( &ScriptMethod::color_repr )
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
            .def_convert( &ScriptMethod::Resolution_convert, nullptr )
            .def_repr( &ScriptMethod::Resolution_repr )
            .def( "setWidth", &Resolution::setWidth )
            .def( "setHeight", &Resolution::setHeight )
            .def( "getWidth", &Resolution::getWidth )
            .def( "getHeight", &Resolution::getHeight )
            .def( "getAspectRatio", &Resolution::getAspectRatio )
            ;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MathScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_stl_vector_type_cast<Vector<mt::vec2f>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<Vector<Polygon>>(_kernel);
    }
}