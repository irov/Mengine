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
#	include "Utils/Math/uv4.h"
#	include "Utils/Math/utils.h"

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
		static PyObject * s_Polygon_getPoints( Polygon * _polygon )
		{				
			PyObject * py_list_vec2f = pybind::list_new(0);

			const Polygon::ring_type & ring = _polygon->outer();

			for( Polygon::ring_type::const_iterator
				it = ring.begin(),
				it_end = ring.end();
			it != it_end;
			++it )
			{
				pybind::list_appenditem_t( py_list_vec2f, *it );
			}

			return py_list_vec2f;
		}
		//////////////////////////////////////////////////////////////////////////
		static Polygon s_Polygon_unscrew( Polygon * _polygon )
		{
			Polygon unscrew;

			const Polygon::ring_type & ring = _polygon->outer();

			boost::geometry::interior_rings( unscrew ).push_back( ring );

			boost::geometry::correct( unscrew );

			return unscrew;
		}
		//////////////////////////////////////////////////////////////////////////
		static String vec2f_repr( pybind::kernel_interface * _kernel, mt::vec2f * _v )
		{
			(void)_kernel;

			Stringstream ss;
			ss << "<vec2f: " << _v->x << ", " << _v->y << ">";
			
            String repr = ss.str();
            
            return repr;
		}
		//////////////////////////////////////////////////////////////////////////
		static float vec2_sequence_get( pybind::kernel_interface * _kernel, mt::vec2f * _vec, uint32_t _index )
		{
			(void)_kernel;

			if( _index > 2 )
			{
				pybind::throw_exception("vec2 index == 2");
			}

			return _vec->operator [] (_index);
		}
		//////////////////////////////////////////////////////////////////////////
		static void vec2_sequence_set( pybind::kernel_interface * _kernel, mt::vec2f * _vec, uint32_t _index, float _value )
		{
			(void)_kernel;

			if( _index > 2 )
			{
				pybind::throw_exception("vec2 index == 2");
			}

			_vec->operator [] (_index) = _value;
		}
        //////////////////////////////////////////////////////////////////////////
		static String vec3f_repr( pybind::kernel_interface * _kernel, mt::vec3f * _v )
        {
			(void)_kernel;

            Stringstream ss;
            ss << "<vec3f: " << _v->x << ", " << _v->y << ", " << _v->z << ">";
            
            String repr = ss.str();

            return repr;
        }
		//////////////////////////////////////////////////////////////////////////
		static float vec3_sequence_get( pybind::kernel_interface * _kernel, mt::vec3f * _vec, uint32_t _index )
		{
			(void)_kernel;

			if( _index > 3 )
			{
				pybind::throw_exception("vec3 index == 3");
			}

			return _vec->operator [] (_index);
		}
		//////////////////////////////////////////////////////////////////////////
		static void vec3_sequence_set( pybind::kernel_interface * _kernel, mt::vec3f * _vec, uint32_t _index, float _value )
		{
			(void)_kernel;

			if( _index > 3 )
			{
				pybind::throw_exception("vec3 index == 3");
			}

			_vec->operator [] (_index) = _value;
		}
        //////////////////////////////////////////////////////////////////////////
		static String vec4f_repr( pybind::kernel_interface * _kernel, mt::vec4f * _v )
        {
			(void)_kernel;

            Stringstream ss;
            ss << "<vec4f: " << _v->x << ", " << _v->y << ", " << _v->z << ", " << _v->w << ">";

            String repr = ss.str();

            return repr;
        }
		//////////////////////////////////////////////////////////////////////////
		static float vec4_sequence_get( pybind::kernel_interface * _kernel, mt::vec4f * _vec, uint32_t _index )
		{
			(void)_kernel;

			if( _index > 4 )
			{
				pybind::throw_exception("vec4 index == 4");
			}

			return _vec->operator [] (_index);
		}
		//////////////////////////////////////////////////////////////////////////
		static void vec4_sequence_set( pybind::kernel_interface * _kernel, mt::vec4f * _vec, uint32_t _index, float _value )
		{
			(void)_kernel;

			if( _index > 4 )
			{
				pybind::throw_exception("vec4 index == 4");
			}

			_vec->operator [] (_index) = _value;
		}
		//////////////////////////////////////////////////////////////////////////
		static String uv4f_repr( pybind::kernel_interface * _kernel, mt::uv4f * _v )
		{
			(void)_kernel;

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
		static const mt::vec2f & uv4f_sequence_get( pybind::kernel_interface * _kernel, mt::uv4f * _uv, uint32_t _index )
		{
			(void)_kernel;

			if( _index > 4 )
			{
				pybind::throw_exception("uv4f index == 4");
			}

			return _uv->operator [] (_index);
		}
		//////////////////////////////////////////////////////////////////////////
		static void uv4f_sequence_set( pybind::kernel_interface * _kernel, mt::uv4f * _vec, uint32_t _index, const mt::vec2f & _value )
		{
			(void)_kernel;

			if( _index > 4 )
			{
				pybind::throw_exception("vec4 index == 4");
			}

			_vec->operator [] (_index) = _value;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool vec2f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec2f * _place, void * _user )
		{
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}
								
				_place->x = pybind::tuple_getitem_t( _obj, 0 );
				_place->y = pybind::tuple_getitem_t( _obj, 1 );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				_place->x = pybind::list_getitem_t( _obj, 0 );
				_place->y = pybind::list_getitem_t( _obj, 1 );

				return true;
			}
			else if( pybind::instance_of<mt::vec3f>( _kernel, _obj ) == true )
			{
				mt::vec3f v3 = pybind::extract_t( _obj );
								
				_place->x = v3.x;
				_place->y = v3.y;

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool vec3f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec3f * _place, void * _user )
		{
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) < 2 || pybind::tuple_size( _obj ) > 3 )
				{
					return false;
				}

				_place->x = pybind::tuple_getitem_t( _obj, 0 );
				_place->y = pybind::tuple_getitem_t( _obj, 1 );

				if( pybind::tuple_size( _obj ) == 3 )
				{					
					_place->z = pybind::tuple_getitem_t( _obj, 2 );
				}
				else
				{
					_place->z = 0.f;
				}

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) < 2 || pybind::list_size( _obj ) > 3 )
				{
					return false;
				}

				_place->x = pybind::list_getitem_t( _obj, 0 );
				_place->y = pybind::list_getitem_t( _obj, 1 );

				if( pybind::list_size( _obj ) == 3 )
				{					
					_place->z = pybind::list_getitem_t( _obj, 2 );
				}
				else
				{
					_place->z = 0.f;
				}

				return true;
			}
			else if( pybind::instance_of<mt::vec2f>( _kernel, _obj ) == true )
			{
				mt::vec2f v2 = pybind::extract_t( _obj );

				_place->x = v2.x;
				_place->y = v2.y;
				_place->z = 0.f;

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool vec4f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::vec4f * _place, void * _user )
		{
			(void)_kernel;
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				_place->x = pybind::tuple_getitem_t( _obj, 0 );
				_place->y = pybind::tuple_getitem_t( _obj, 1 );
				_place->z = pybind::tuple_getitem_t( _obj, 2 );
				_place->w = pybind::tuple_getitem_t( _obj, 3 );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				_place->x = pybind::list_getitem_t( _obj, 0 );
				_place->y = pybind::list_getitem_t( _obj, 1 );
				_place->z = pybind::list_getitem_t( _obj, 2 );
				_place->w = pybind::list_getitem_t( _obj, 3 );
				
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool uv4f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::uv4f * _uv, void * _user )
		{
			(void)_kernel;
			(void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				_uv->p0 = pybind::tuple_getitem_t( _obj, 0 );
				_uv->p1 = pybind::tuple_getitem_t( _obj, 1 );
				_uv->p2 = pybind::tuple_getitem_t( _obj, 2 );
				_uv->p3 = pybind::tuple_getitem_t( _obj, 3 );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}
								
				_uv->p0 = pybind::list_getitem_t( _obj, 0 );
				_uv->p1 = pybind::list_getitem_t( _obj, 1 );
				_uv->p2 = pybind::list_getitem_t( _obj, 2 );
				_uv->p3 = pybind::list_getitem_t( _obj, 3 );

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static String box2f_repr( pybind::kernel_interface * _kernel, mt::box2f * _v )
		{
			(void)_kernel;

			Stringstream ss;
			ss << "<box2f: " << _v->minimum.x << ", " << _v->minimum.y << ", " << _v->maximum.x << ", " << _v->maximum.y << ">";

			String repr = ss.str();

			return repr;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool box2f_convert( pybind::kernel_interface * _kernel, PyObject * _obj, mt::box2f * _place, void * _user )
        {
			(void)_kernel;

            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				_place->minimum.x = pybind::tuple_getitem_t( _obj, 0 );
				_place->minimum.y = pybind::tuple_getitem_t( _obj, 1 );
				_place->maximum.x = pybind::tuple_getitem_t( _obj, 2 );
				_place->maximum.y = pybind::tuple_getitem_t( _obj, 3 );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				_place->minimum.x = pybind::list_getitem_t( _obj, 0 );
				_place->minimum.y = pybind::list_getitem_t( _obj, 1 );
				_place->maximum.x = pybind::list_getitem_t( _obj, 2 );
				_place->maximum.y = pybind::list_getitem_t( _obj, 3 );

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Polygon_convert( pybind::kernel_interface * _kernel, PyObject * _obj, void * _place, void * _user )
        {
			(void)_kernel;
            (void)_user;

			if( pybind::list_check( _obj ) == false )
			{
				return false;
			}

			Polygon * polygon = (Polygon*)_place;

			uint32_t size = pybind::list_size( _obj );

			for( uint32_t i = 0; i != size; ++i )
			{
				PyObject * py_item = pybind::list_getitem( _obj, i );

				if( pybind::list_check( py_item ) == true )
				{
					Polygon inner;
					pybind::extract_value( py_item, inner );

					const Polygon::ring_type & ring = inner.outer();

					boost::geometry::interior_rings( *polygon ).push_back( ring );										
				}
				else
				{
					mt::vec2f point = pybind::list_getitem_t( _obj, i );

					boost::geometry::append( *polygon, point );
				}
			}

			boost::geometry::correct( *polygon );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		static String color_repr( pybind::kernel_interface * _kernel, ColourValue * _v )
        {
			(void)_kernel;

			Stringstream ss;
			ss << "<color: " << _v->getA() << ", " << _v->getR() << ", " << _v->getG() << ", " << _v->getB() << ">";
            
            String repr = ss.str();

            return repr;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool color_convert( pybind::kernel_interface * _kernel, PyObject * _obj, ColourValue * _place, void * _user )
        {
			(void)_kernel;
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) == 4 )
				{
					float r = pybind::tuple_getitem_t( _obj, 0 );
					float g = pybind::tuple_getitem_t( _obj, 1 );
					float b = pybind::tuple_getitem_t( _obj, 2 );
					float a = pybind::tuple_getitem_t( _obj, 3 );

					_place->setARGB( a, r, g, b );
				}
				else if( pybind::tuple_size( _obj ) == 3 )
				{
					float r = pybind::tuple_getitem_t( _obj, 0 );
					float g = pybind::tuple_getitem_t( _obj, 1 );
					float b = pybind::tuple_getitem_t( _obj, 2 );
					float a = 1.f;

					_place->setARGB( a, r, g, b );
				}
				else
				{
					return false;
				}

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) == 4 )
				{
					float r = pybind::list_getitem_t( _obj, 0 );
					float g = pybind::list_getitem_t( _obj, 1 );
					float b = pybind::list_getitem_t( _obj, 2 );
					float a = pybind::list_getitem_t( _obj, 3 );

					_place->setARGB( a, r, g, b );
				}				
				else if( pybind::list_size( _obj ) == 3 )
				{
					float r = pybind::list_getitem_t( _obj, 0 );
					float g = pybind::list_getitem_t( _obj, 1 );
					float b = pybind::list_getitem_t( _obj, 2 );
					float a = 1.f;

					_place->setARGB( a, r, g, b );
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
		static bool Resolution_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Resolution * _place, void * _user )
        {
			(void)_kernel;
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				uint32_t width = pybind::tuple_getitem_t( _obj, 0 );
				uint32_t height = pybind::tuple_getitem_t( _obj, 1 );

				_place->setWidth( width );
				_place->setHeight( height );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				uint32_t width = pybind::list_getitem_t( _obj, 0 );
				uint32_t height = pybind::list_getitem_t( _obj, 1 );

				_place->setWidth( width );
				_place->setHeight( height );

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static String Resolution_repr( pybind::kernel_interface * _kernel, Resolution * _v )
		{
			(void)_kernel;

			Stringstream ss;
			ss << "<Resolution: " << _v->getWidth() << ", " << _v->getHeight() << ">";

			String repr = ss.str();

			return repr;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Viewport_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Viewport * _place, void * _user )
        {
			(void)_kernel;
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 2 )
				{
					return false;
				}

				mt::vec2f begin = pybind::tuple_getitem_t( _obj, 0 );
				mt::vec2f end = pybind::tuple_getitem_t( _obj, 1 );

				_place->initialize( begin, end );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 2 )
				{
					return false;
				}

				mt::vec2f begin = pybind::list_getitem_t( _obj, 0 );
				mt::vec2f end = pybind::list_getitem_t( _obj, 1 );

				_place->initialize( begin, end );

				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static String Viewport_repr( pybind::kernel_interface * _kernel, Viewport * _v )
		{
			(void)_kernel;

			Stringstream ss;
			ss << "<Viewport: " << _v->begin.x << ", " << _v->begin.y << ", " << _v->end.x << ", " << _v->end.y << ", " << ">";

			String repr = ss.str();

			return repr;
		}
		//////////////////////////////////////////////////////////////////////////
		static bool Rect_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Rect * _place, void * _user )
        {
			(void)_kernel;
            (void)_user;

			if( pybind::tuple_check( _obj ) == true )
			{
				if( pybind::tuple_size( _obj ) != 4 )
				{
					return false;
				}

				_place->left = pybind::tuple_getitem_t( _obj, 0 );
				_place->top = pybind::tuple_getitem_t( _obj, 1 );
				_place->right = pybind::tuple_getitem_t( _obj, 2 );
				_place->bottom = pybind::tuple_getitem_t( _obj, 3 );

				return true;
			}
			else if( pybind::list_check( _obj ) == true )
			{
				if( pybind::list_size( _obj ) != 4 )
				{
					return false;
				}

				_place->left = pybind::list_getitem_t( _obj, 0 );
				_place->top = pybind::list_getitem_t( _obj, 1 );
				_place->right = pybind::list_getitem_t( _obj, 2 );
				_place->bottom = pybind::list_getitem_t( _obj, 3 );

				return true;
			}

			return false;
		}
	}
    //////////////////////////////////////////////////////////////////////////
	void ScriptWrapper::mathWrap( ServiceProviderInterface * _serviceProvider )
	{
        (void)_serviceProvider;

		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::registration_stl_vector_type_cast<Polygon, stdex::vector<Polygon> >(kernel);

		pybind::struct_<mt::vec2f>("vec2f")
			.def_constructor( pybind::init<float,float>() )
			.def_convert( &ScriptMethod::vec2f_convert, nullptr )
			.def_static_sequence_get( &ScriptMethod::vec2_sequence_get )
			.def_static_sequence_set( &ScriptMethod::vec2_sequence_set )
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
			.def_static_sequence_get( &ScriptMethod::vec3_sequence_get )
			.def_static_sequence_set( &ScriptMethod::vec3_sequence_set )
            .def_repr( &ScriptMethod::vec3f_repr )
			.def_member( "x", &mt::vec3f::x )
			.def_member( "y", &mt::vec3f::y )
			.def_member( "z", &mt::vec3f::z )
			;

		pybind::struct_<mt::vec4f>("vec4f")
			.def_constructor( pybind::init<float,float,float,float>() )
			.def_convert( &ScriptMethod::vec4f_convert, nullptr )
			.def_static_sequence_get( &ScriptMethod::vec4_sequence_get )
			.def_static_sequence_set( &ScriptMethod::vec4_sequence_set )
            .def_repr( &ScriptMethod::vec4f_repr )
			.def_member( "x", &mt::vec4f::x )
			.def_member( "y", &mt::vec4f::y )
			.def_member( "z", &mt::vec4f::z )
			.def_member( "w", &mt::vec4f::w )
			;

		pybind::struct_<mt::uv4f>("uv4f")
			.def_constructor( pybind::init<mt::vec2f,mt::vec2f,mt::vec2f,mt::vec2f>() )
			.def_convert( &ScriptMethod::uv4f_convert, nullptr )
			.def_static_sequence_get( &ScriptMethod::uv4f_sequence_get )
			.def_static_sequence_set( &ScriptMethod::uv4f_sequence_set )
			.def_repr( &ScriptMethod::uv4f_repr )
			.def_member( "p0", &mt::uv4f::p0 )
			.def_member( "p1", &mt::uv4f::p1 )
			.def_member( "p2", &mt::uv4f::p2 )
			.def_member( "p3", &mt::uv4f::p3 )
			;

		pybind::struct_<mt::box2f>("box2f")
			.def_constructor( pybind::init<mt::vec2f,mt::vec2f>() )
			.def_convert( &ScriptMethod::box2f_convert, nullptr )
			.def_repr( &ScriptMethod::box2f_repr )
			.def_member( "minimum", &mt::box2f::minimum )
			.def_member( "maximum", &mt::box2f::maximum )
			;

		pybind::struct_<Rect>("Rect")
			.def_constructor( pybind::init<uint32_t,uint32_t,uint32_t,uint32_t>() )
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
			.def_static("getPoints", &ScriptMethod::s_Polygon_getPoints)
			.def_static( "unscrew", &ScriptMethod::s_Polygon_unscrew )
			;

		pybind::struct_<Viewport>("Viewport")
			.def_constructor( pybind::init<mt::vec2f,mt::vec2f>() )
			.def_convert( &ScriptMethod::Viewport_convert, nullptr )
			.def_repr( &ScriptMethod::Viewport_repr )
			.def_member( "begin", &Viewport::begin )
			.def_member( "end", &Viewport::end )
			.def( "getWidth", &Viewport::getWidth )
			.def( "getHeight", &Viewport::getHeight )
			.def( "testPoint", &Viewport::testPoint )
			.def( "testBBox", &Viewport::testBBox )
			.def( "testRectangle", &Viewport::testRectangle )
			.def( "existBBox", &Viewport::existBBox )
			.def( "existRectangle", &Viewport::existRectangle )
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
			.def_constructor( pybind::init<uint32_t, uint32_t>() )
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

