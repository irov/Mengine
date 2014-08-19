#	include "ScriptWrapper.h"

#   include "Config/String.h"
#   include "Config/Stringstream.h"

#	include "pybind/pybind.hpp"

#	include "Core/Polygon.h"

#	include "Utils/Math/angle.h"
#	include "Utils/Math/vec4.h"
#	include "Utils/Math/mat3.h"
#	include "Utils/Math/mat4.h"
#	include "Utils/Math/quat.h"
#	include "Utils/Math/clamp.h"

#	include "Utils/Core/ColourValue.h"
#	include "Utils/Core/Resolution.h"
#	include "Utils/Core/Viewport.h"
#	include "Utils/Core/Rect.h"

#	include "pybind/stl_type_cast.hpp"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#   pragma warning(disable:4800)
#   endif

#	include <boost/geometry/geometry.hpp> 
#	include <boost/geometry/geometries/point_xy.hpp>
#	include <boost/geometry/geometries/polygon.hpp>
#	include <boost/geometry/algorithms/intersects.hpp>

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#   endif

namespace Menge
{
	namespace ScriptMethod
	{
		//////////////////////////////////////////////////////////////////////////
		static PyObject * s_getPoints( Polygon * _polygon )
		{				
			PyObject * py_list_vec2f = pybind::list_new(0);

			const Polygon::ring_type & ring = _polygon->outer();

			for( Polygon::ring_type::const_iterator
				it = ring.begin(),
				it_end = ring.end();
			it != it_end;
			++it )
			{
				PyObject * py_vec2f = pybind::ptr(*it);

				pybind::list_appenditem( py_list_vec2f, py_vec2f );

				pybind::decref( py_vec2f );
			}

			return py_list_vec2f;
		}
		//////////////////////////////////////////////////////////////////////////
		static PyObject * vec2f_repr( PyObject * _obj, mt::vec2f * _v )
		{
            (void)_obj;

			Stringstream ss;
			ss << "<vec2f: " << _v->x << ", " << _v->y << ">";
			
            String repr = ss.str();
            
            return pybind::ptr(repr);
		}
		//////////////////////////////////////////////////////////////////////////
		static float vec2_sequence( mt::vec2f * _vec, size_t _index )
		{
			if( _index > 2 )
			{
				pybind::throw_exception("vec2 index == 2");
			}

			return _vec->operator [] (_index);
		}
        //////////////////////////////////////////////////////////////////////////
        static PyObject * vec3f_repr( PyObject * _obj, mt::vec3f * _v )
        {
            (void)_obj;

            Stringstream ss;
            ss << "<vec3f: " << _v->x << ", " << _v->y << ", " << _v->z << ">";
            
            String repr = ss.str();

            return pybind::ptr(repr);
        }
		//////////////////////////////////////////////////////////////////////////
		static float vec3_sequence( mt::vec3f * _vec, size_t _index )
		{
			if( _index > 3 )
			{
				pybind::throw_exception("vec3 index == 3");
			}

			return _vec->operator [] (_index);
		}
        //////////////////////////////////////////////////////////////////////////
        static PyObject * vec4f_repr( PyObject * _obj, mt::vec4f * _v )
        {
            (void)_obj;

            Stringstream ss;
            ss << "<vec4f: " << _v->x << ", " << _v->y << ", " << _v->z << ", " << _v->w << ">";

            String repr = ss.str();

            return pybind::ptr(repr);
        }
		//////////////////////////////////////////////////////////////////////////
		static float vec4_sequence( mt::vec4f * _vec, size_t _index )
		{
			if( _index > 4 )
			{
				pybind::throw_exception("vec4 index == 4");
			}

			return _vec->operator [] (_index);
		}
		//////////////////////////////////////////////////////////////////////////
		static bool vec2f_convert( PyObject * _obj, void * _place, void * _user )
		{
            (void)_user;

			if( pybind::instance_of<mt::vec3f>( _obj ) == true )
			{
				mt::vec3f v3 = pybind::extract<mt::vec3f>(_obj);

				mt::vec2f * impl = (mt::vec2f *)_place;

				impl->x = v3.x;
				impl->y = v3.y;

				return true;
			}
			else if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				mt::vec2f * impl = (mt::vec2f *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				mt::vec2f * impl = (mt::vec2f *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool vec3f_convert( PyObject * _obj, void * _place, void * _user )
		{
            (void)_user;

			if( pybind::instance_of<mt::vec2f>( _obj ) == true )
			{
				mt::vec2f v2 = pybind::extract<mt::vec2f>(_obj);

				mt::vec3f * impl = (mt::vec3f *)_place;

				impl->x = v2.x;
				impl->y = v2.y;
				impl->z = 0.f;

				return true;
			}
			else if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) < 2 || pybind::tuple_size( _obj ) > 3 )
				{
					return false;
				}

				mt::vec3f * impl = (mt::vec3f *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);

				if( pybind::tuple_size( _obj ) == 3 )
				{
					PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
					impl->z = pybind::extract<float>(i2);
				}
				else
				{
					impl->z = 0.f;
				}

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) < 2 || pybind::tuple_size( _obj ) > 3 )
				{
					return false;
				}

				mt::vec4f * impl = (mt::vec4f *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);

				if( pybind::tuple_size( _obj ) == 3 )
				{
					PyObject * i2 = pybind::list_getitem( _obj, 2 );
					impl->z = pybind::extract<float>(i2);
				}
				else
				{
					impl->z = 0.f;
				}

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool vec4f_convert( PyObject * _obj, void * _place, void * _user )
		{
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				mt::vec4f * impl = (mt::vec4f *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);
				impl->z = pybind::extract<float>(i2);
				impl->w = pybind::extract<float>(i3);

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				mt::vec4f * impl = (mt::vec4f *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				impl->x = pybind::extract<float>(i0);
				impl->y = pybind::extract<float>(i1);
				impl->z = pybind::extract<float>(i2);
				impl->w = pybind::extract<float>(i3);

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool box2f_convert( PyObject * _obj, void * _place, void * _user )
        {
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				mt::box2f * impl = (mt::box2f *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				impl->minimum.x = pybind::extract<float>(i0);
				impl->minimum.y = pybind::extract<float>(i1);
				impl->maximum.x = pybind::extract<float>(i2);
				impl->maximum.y = pybind::extract<float>(i3);

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				mt::box2f * impl = (mt::box2f *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				impl->minimum.x = pybind::extract<float>(i0);
				impl->minimum.y = pybind::extract<float>(i1);
				impl->maximum.x = pybind::extract<float>(i2);
				impl->maximum.y = pybind::extract<float>(i3);

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Polygon_convert( PyObject * _obj, void * _place, void * _user )
        {
            (void)_user;

			if( pybind::list_check( _obj ) == false )
			{
				return false;
			}

			Polygon * polygon = (Polygon*)_place;

			size_t size = pybind::list_size( _obj );

			for( size_t i = 0; i != size; ++i )
			{
				PyObject * py_point = pybind::list_getitem( _obj, i );

				mt::vec2f point = pybind::extract<mt::vec2f>( py_point );

				boost::geometry::append(*polygon, point);
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		static PyObject * color_repr( PyObject * _obj, ColourValue * _v )
        {
            (void)_obj;

			Stringstream ss;
			ss << "<color: " << _v->getA() << ", " << _v->getR() << ", " << _v->getG() << ", " << _v->getB() << ">";
            
            String repr = ss.str();

            return pybind::ptr(repr);
		}
		//////////////////////////////////////////////////////////////////////////
		static bool color_convert( PyObject * _obj, void * _place, void * _user )
        {
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				ColourValue * impl = (ColourValue *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				float r = pybind::extract<float>(i0);
				float g = pybind::extract<float>(i1);
				float b = pybind::extract<float>(i2);
				float a = pybind::extract<float>(i3);

				impl->setARGB( a, r, g, b );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				ColourValue * impl = (ColourValue *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				float r = pybind::extract<float>(i0);
				float g = pybind::extract<float>(i1);
				float b = pybind::extract<float>(i2);
				float a = pybind::extract<float>(i3);

				impl->setARGB( a, r, g, b );

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Resolution_convert( PyObject * _obj, void * _place, void * _user )
        {
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				Resolution * impl = (Resolution *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );

				size_t width = pybind::extract<size_t>(i0);
				size_t height = pybind::extract<size_t>(i1);

				impl->setWidth( width );
				impl->setHeight( height );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				Resolution * impl = (Resolution *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );

				size_t width = pybind::extract<size_t>(i0);
				size_t height = pybind::extract<size_t>(i1);

				impl->setWidth( width );
				impl->setHeight( height );

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static PyObject * Resolution_repr( PyObject * _obj, Resolution * _v )
		{
			(void)_obj;

			Stringstream ss;
			ss << "<Resolution: " << _v->getWidth() << ", " << _v->getHeight() << ">";

			String repr = ss.str();

			return pybind::ptr(repr);
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Viewport_convert( PyObject * _obj, void * _place, void * _user )
        {
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				Viewport * impl = (Viewport *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );

				mt::vec2f begin = pybind::extract<mt::vec2f>(i0);
				mt::vec2f end = pybind::extract<mt::vec2f>(i1);

				impl->initialize( begin, end );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				Viewport * impl = (Viewport *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );

				mt::vec2f begin = pybind::extract<mt::vec2f>(i0);
				mt::vec2f end = pybind::extract<mt::vec2f>(i1);

				impl->initialize( begin, end );

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static PyObject * Viewport_repr( PyObject * _obj, Viewport * _v )
		{
			(void)_obj;

			Stringstream ss;
			ss << "<Viewport: " << _v->begin.x << ", " << _v->begin.y << ", " << _v->end.x << ", " << _v->end.y << ", " << ">";

			String repr = ss.str();

			return pybind::ptr(repr);
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Rect_convert( PyObject * _obj, void * _place, void * _user )
        {
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				Rect * impl = (Rect *)_place;

				PyObject * i0 = pybind::tuple_getitem( _obj, 0 );
				PyObject * i1 = pybind::tuple_getitem( _obj, 1 );
				PyObject * i2 = pybind::tuple_getitem( _obj, 2 );
				PyObject * i3 = pybind::tuple_getitem( _obj, 3 );

				impl->left = pybind::extract<int>(i0);
				impl->top = pybind::extract<int>(i1);
				impl->right = pybind::extract<int>(i2);
				impl->bottom = pybind::extract<int>(i3);

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				Rect * impl = (Rect *)_place;

				PyObject * i0 = pybind::list_getitem( _obj, 0 );
				PyObject * i1 = pybind::list_getitem( _obj, 1 );
				PyObject * i2 = pybind::list_getitem( _obj, 2 );
				PyObject * i3 = pybind::list_getitem( _obj, 3 );

				impl->left = pybind::extract<int>(i0);
				impl->top = pybind::extract<int>(i1);
				impl->right = pybind::extract<int>(i2);
				impl->bottom = pybind::extract<int>(i3);

				return true;
			}

			return false;
		}
	}
    //////////////////////////////////////////////////////////////////////////
	void ScriptWrapper::mathWrap( ServiceProviderInterface * _serviceProvider )
	{
        (void)_serviceProvider;

		pybind::registration_stl_vector_type_cast<Polygon>();

		pybind::struct_<mt::vec2f>("vec2f")
			.def_constructor( pybind::init<float,float>() )
			.def_convert( &ScriptMethod::vec2f_convert, nullptr )
			.def_static_sequence( &ScriptMethod::vec2_sequence )
			.def_repr( &ScriptMethod::vec2f_repr )

			.def_member( "x", &mt::vec2f::x )
			.def_member( "y", &mt::vec2f::y )
			//.attr( "x", &vec2f::x )
			//.def( boost::python::init<float,float>() )
			//.def( boost::python::self + boost::python::self )	// __add__
			//.def( boost::python::self - boost::python::self )          // __radd__
			//.def( boost::python::self * float() )           // __sub__
			//.def( boost::python::self / float() )          // __sub__
			//.def( float() * boost::python::self )         // __iadd__
			//.def( boost::python::self += boost::python::self )
			//.def( boost::python::self -= boost::python::self )
			//.def( boost::python::self *= float() )
			//.def( boost::python::self /= float() )
			;

		pybind::struct_<mt::vec3f>("vec3f")
			.def_constructor( pybind::init<float,float,float>() )
			.def_convert( &ScriptMethod::vec3f_convert, nullptr )
			.def_static_sequence( &ScriptMethod::vec3_sequence )
            .def_repr( &ScriptMethod::vec3f_repr )
			.def_member( "x", &mt::vec3f::x )
			.def_member( "y", &mt::vec3f::y )
			.def_member( "z", &mt::vec3f::z )
			;

		pybind::struct_<mt::vec4f>("vec4f")
			.def_constructor( pybind::init<float,float,float,float>() )
			.def_convert( &ScriptMethod::vec4f_convert, nullptr )
			.def_static_sequence( &ScriptMethod::vec4_sequence )
            .def_repr( &ScriptMethod::vec4f_repr )
			.def_member( "x", &mt::vec4f::x )
			.def_member( "y", &mt::vec4f::y )
			.def_member( "z", &mt::vec4f::z )
			.def_member( "w", &mt::vec4f::w )
			;

		pybind::struct_<mt::box2f>("box2f")
			.def_constructor( pybind::init<mt::vec2f,mt::vec2f>() )
			.def_convert( &ScriptMethod::box2f_convert, nullptr )
			.def_member( "minimum", &mt::box2f::minimum )
			.def_member( "maximum", &mt::box2f::maximum )
			;

		pybind::struct_<Rect>("Rect")
			.def_constructor( pybind::init<size_t,size_t,size_t,size_t>() )
			.def_convert( &ScriptMethod::Rect_convert, nullptr )
			.def_member( "left", &Rect::left )
			.def_member( "top",  &Rect::top ) 
			.def_member( "right",  &Rect::right )
			.def_member( "bottom",  &Rect::bottom )
			.def( "getWidth", &Rect::getWidth )
			.def( "getHeight", &Rect::getHeight )
			;


		pybind::struct_<mt::quatf>("quatf")
			.def_constructor( pybind::init<float,float,float,float>() )
			.def_member( "w", &mt::quatf::w )
			.def_member( "x", &mt::quatf::x )
			.def_member( "y", &mt::quatf::y )
			.def_member( "z", &mt::quatf::z )			
			;

		pybind::struct_<Polygon>("Polygon")
			.def_convert( &ScriptMethod::Polygon_convert, nullptr )
			.def_static("getPoints", &ScriptMethod::s_getPoints)
			;

		pybind::struct_<Viewport>("Viewport")
			.def_constructor( pybind::init<mt::vec2f,mt::vec2f>() )
			.def_convert( &ScriptMethod::Viewport_convert, nullptr )
			.def_repr( &ScriptMethod::Viewport_repr )
			.def_member( "begin", &Viewport::begin )
			.def_member( "end", &Viewport::end )
			.def( "getWidth", &Viewport::getWidth )
			.def( "getHeight", &Viewport::getHeight )
			;

		pybind::struct_<ColourValue>("Color")
			.def_constructor( pybind::init<float, float, float, float>() )
			.def_convert( &ScriptMethod::color_convert, nullptr )
			.def_repr( &ScriptMethod::color_repr )
			.def( "getA", &ColourValue::getA )
			.def( "getR", &ColourValue::getR )
			.def( "getG", &ColourValue::getG )
			.def( "getB", &ColourValue::getB )
			.def_property( "a", &ColourValue::getA, &ColourValue::setA )
			.def_property( "r", &ColourValue::getR, &ColourValue::setR )
			.def_property( "g", &ColourValue::getG, &ColourValue::setG )
			.def_property( "b", &ColourValue::getB, &ColourValue::setB )			
			;

		pybind::struct_<Resolution>("Resolution")
			.def_constructor( pybind::init<size_t, size_t>() )
			.def_convert( &ScriptMethod::Resolution_convert, nullptr )
			.def_repr( &ScriptMethod::Resolution_repr )
			.def( "setWidth", &Resolution::setWidth )
			.def( "setHeight", &Resolution::setHeight )
			.def( "getWidth", &Resolution::getWidth )
			.def( "getHeight", &Resolution::getHeight )
			.def( "getAspectRatio", &Resolution::getAspectRatio )
			;	
	}
}

