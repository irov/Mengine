//#	include "ScriptWrapper.h"
//
//#	include <xxbind/xxbind.hpp>
//
//#	include "Core/Polygon.h"
//
//#	include "Utils/Math/angle.h"
//#	include "Utils/Math/vec4.h"
//#	include "Utils/Math/mat3.h"
//#	include "Utils/Math/mat4.h"
//#	include "Utils/Math/quat.h"
//#	include "Utils/Math/clamp.h"
//
//#	include "Utils/Core/ColourValue.h"
//#	include "Utils/Core/Resolution.h"
//#	include "Utils/Core/Viewport.h"
//#	include "Utils/Core/Rect.h"
//
//#	include <boost/geometry/geometry.hpp> 
//#	include <boost/geometry/geometries/point_xy.hpp>
//#	include <boost/geometry/geometries/polygon.hpp>
//#	include <boost/geometry/algorithms/intersects.hpp>
//
//#	include <sstream>
//
//namespace Menge
//{
//	namespace ScriptMethod
//	{
//		//////////////////////////////////////////////////////////////////////////
//		static xxbind::facade_object * s_getPoints( xxbind::facade_script * _facade, Polygon * _polygon )
//		{				
//			xxbind::facade_object * py_list_vec2f = _facade->list_new(0);
//
//			const Polygon::ring_type & ring = _polygon->outer();
//
//			for( Polygon::ring_type::const_iterator
//				it = ring.begin(),
//				it_end = ring.end();
//			it != it_end;
//			++it )
//			{
//				xxbind::facade_object * py_vec2f = xxbind::ptr(_facade, *it);
//
//				_facade->list_appenditem( py_list_vec2f, py_vec2f );
//
//				_facade->decref( py_vec2f );
//			}
//
//			return py_list_vec2f;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		std::string vec2f_repr( xxbind::facade_script * _facade, xxbind::facade_object * _obj, mt::vec2f * _v )
//		{
//			std::stringstream ss;
//			ss << "<vec2f: " << _v->x << ", " << _v->y << ">";
//			return ss.str();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		float vec2_sequence( mt::vec2f * _vec, size_t _index )
//		{
//			if( _index > 2 )
//			{				
//				throw xxbind::xxbind_exception();
//			}
//
//			return _vec->operator [] (_index);
//		}
//		//////////////////////////////////////////////////////////////////////////
//		float vec3_sequence( mt::vec3f * _vec, size_t _index )
//		{
//			if( _index > 3 )
//			{
//				throw xxbind::xxbind_exception();
//			}
//
//			return _vec->operator [] (_index);
//		}
//		//////////////////////////////////////////////////////////////////////////
//		float vec4_sequence( mt::vec4f * _vec, size_t _index )
//		{
//			if( _index > 4 )
//			{
//				throw xxbind::xxbind_exception();
//			}
//
//			return _vec->operator [] (_index);
//		}
//		//////////////////////////////////////////////////////////////////////////
//		std::string color_repr( xxbind::facade_script * _facade, xxbind::facade_object * _obj, ColourValue * _v )
//		{
//			std::stringstream ss;
//			ss << "<color: " << _v->getA() << ", " << _v->getR() << ", " << _v->getG() << ", " << _v->getB() << ">";
//			return ss.str();
//		}
//		//////////////////////////////////////////////////////////////////////////
//		bool vec2f_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( xxbind::instance_of<mt::vec3f>( _facade, _obj ) == true )
//			{
//				mt::vec3f v3 = xxbind::extract<mt::vec3f>(_facade, _obj);
//
//				mt::vec2f * impl = (mt::vec2f *)_place;
//
//				impl->x = v3.x;
//				impl->y = v3.y;
//
//				return true;
//			}			
//			else if( _facade->tuple_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) != 2 )
//				{
//					return false;
//				}
//
//				mt::vec2f * impl = (mt::vec2f *)_place;
//
//				xxbind::facade_object * i0 = _facade->tuple_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->tuple_getitem( _obj, 1 );
//
//				impl->x = xxbind::extract<float>(_facade, i0);
//				impl->y = xxbind::extract<float>(_facade, i1);
//
//				return true;
//			}
//			else if( _facade->list_check( _obj ) == true )
//			{
//				if( _facade->list_size( _obj ) != 2 )
//				{
//					return false;
//				}
//
//				mt::vec2f * impl = (mt::vec2f *)_place;
//
//				xxbind::facade_object * i0 = _facade->list_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->list_getitem( _obj, 1 );
//
//				impl->x = xxbind::extract<float>(_facade, i0);
//				impl->y = xxbind::extract<float>(_facade, i1);
//
//				return true;
//			}
//
//			return false;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		bool vec3f_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( xxbind::instance_of<mt::vec2f>( _facade, _obj ) == true )
//			{
//				mt::vec2f v2 = xxbind::extract<mt::vec2f>( _facade, _obj );
//
//				mt::vec3f * impl = (mt::vec3f *)_place;
//
//				impl->x = v2.x;
//				impl->y = v2.y;
//				impl->z = 0.f;
//
//				return true;
//			}
//			else if( _facade->tuple_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) < 2 || _facade->tuple_size( _obj ) > 3 )
//				{
//					return false;
//				}
//
//				mt::vec3f * impl = (mt::vec3f *)_place;
//
//				xxbind::facade_object * i0 = _facade->tuple_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->tuple_getitem( _obj, 1 );
//
//				impl->x = xxbind::extract<float>(_facade, i0);
//				impl->y = xxbind::extract<float>(_facade, i1);
//
//				if( _facade->tuple_size( _obj ) == 3 )
//				{
//					xxbind::facade_object * i2 = _facade->tuple_getitem( _obj, 2 );
//
//					impl->z = xxbind::extract<float>(_facade, i2);
//				}
//				else
//				{
//					impl->z = 0.f;
//				}
//
//				return true;
//			}
//			else if( _facade->list_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) < 2 || _facade->tuple_size( _obj ) > 3 )
//				{
//					return false;
//				}
//
//				mt::vec4f * impl = (mt::vec4f *)_place;
//
//				xxbind::facade_object * i0 = _facade->list_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->list_getitem( _obj, 1 );
//
//				impl->x = xxbind::extract<float>(_facade, i0);
//				impl->y = xxbind::extract<float>(_facade, i1);
//
//				if( _facade->tuple_size( _obj ) == 3 )
//				{
//					xxbind::facade_object * i2 = _facade->list_getitem( _obj, 2 );
//
//					impl->z = xxbind::extract<float>(_facade, i2);
//				}
//				else
//				{
//					impl->z = 0.f;
//				}
//
//				return true;
//			}
//
//			return false;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		bool vec4f_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( _facade->tuple_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) != 4 )
//				{
//					return false;
//				}
//				
//				mt::vec4f * impl = (mt::vec4f *)_place;
//
//				xxbind::facade_object * i0 = _facade->tuple_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->tuple_getitem( _obj, 1 );
//				xxbind::facade_object * i2 = _facade->tuple_getitem( _obj, 2 );
//				xxbind::facade_object * i3 = _facade->tuple_getitem( _obj, 3 );
//
//				impl->x = xxbind::extract<float>(_facade, i0);
//				impl->y = xxbind::extract<float>(_facade, i1);
//				impl->z = xxbind::extract<float>(_facade, i2);
//				impl->w = xxbind::extract<float>(_facade, i3);
//
//				return true;
//			}
//			else if( _facade->list_check( _obj ) == true )
//			{
//				if( _facade->list_size( _obj ) != 4 )
//				{
//					return false;
//				}
//
//				mt::vec4f * impl = (mt::vec4f *)_place;
//
//				xxbind::facade_object * i0 = _facade->list_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->list_getitem( _obj, 1 );
//				xxbind::facade_object * i2 = _facade->list_getitem( _obj, 2 );
//				xxbind::facade_object * i3 = _facade->list_getitem( _obj, 3 );
//
//				impl->x = xxbind::extract<float>(_facade, i0);
//				impl->y = xxbind::extract<float>(_facade, i1);
//				impl->z = xxbind::extract<float>(_facade, i2);
//				impl->w = xxbind::extract<float>(_facade, i3);
//
//				return true;
//			}
//
//			return false;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static bool Polygon_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( _facade->list_check( _obj ) == false )
//			{
//				return false;
//			}
//
//			Polygon * polygon = (Polygon*)_place;
//
//			size_t size = _facade->list_size( _obj );
//
//			for( size_t i = 0; i != size; ++i )
//			{
//				xxbind::facade_object * py_point = _facade->list_getitem(_obj, i);
//
//				mt::vec2f point = xxbind::extract<mt::vec2f>(_facade, py_point);
//
//				boost::geometry::append(*polygon, point);
//			}
//
//			return true;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static bool color_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( _facade->tuple_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) != 4 )
//				{
//					return false;
//				}
//
//				ColourValue * impl = (ColourValue *)_place;
//
//				xxbind::facade_object * i0 = _facade->tuple_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->tuple_getitem( _obj, 1 );
//				xxbind::facade_object * i2 = _facade->tuple_getitem( _obj, 2 );
//				xxbind::facade_object * i3 = _facade->tuple_getitem( _obj, 3 );
//
//				impl->r = xxbind::extract<float>(_facade, i0);
//				impl->g = xxbind::extract<float>(_facade, i1);
//				impl->b = xxbind::extract<float>(_facade, i2);
//				impl->a = xxbind::extract<float>(_facade, i3);
//				
//				impl->invalidate();
//
//				return true;
//			}
//			else if( _facade->list_check( _obj ) == true )
//			{
//				if( _facade->list_size( _obj ) != 4 )
//				{
//					return false;
//				}
//
//				ColourValue * impl = (ColourValue *)_place;
//
//				xxbind::facade_object * i0 = _facade->list_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->list_getitem( _obj, 1 );
//				xxbind::facade_object * i2 = _facade->list_getitem( _obj, 2 );
//				xxbind::facade_object * i3 = _facade->list_getitem( _obj, 3 );
//
//				impl->r = xxbind::extract<float>(_facade, i0);
//				impl->g = xxbind::extract<float>(_facade, i1);
//				impl->b = xxbind::extract<float>(_facade, i2);
//				impl->a = xxbind::extract<float>(_facade, i3);
//
//				return true;
//			}
//
//			return false;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static bool Resolution_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( _facade->tuple_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) != 2 )
//				{
//					return false;
//				}
//
//				Resolution * impl = (Resolution *)_place;
//
//				xxbind::facade_object * i0 = _facade->tuple_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->tuple_getitem( _obj, 1 );
//
//				size_t width = xxbind::extract<size_t>(_facade, i0);
//				size_t height = xxbind::extract<size_t>(_facade, i1);
//
//				impl->setWidth( width );
//				impl->setHeight( height );
//
//				return true;
//			}
//			else if( _facade->list_check( _obj ) == true )
//			{
//				if( _facade->list_size( _obj ) != 2 )
//				{
//					return false;
//				}
//
//				Resolution * impl = (Resolution *)_place;
//
//				xxbind::facade_object * i0 = _facade->list_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->list_getitem( _obj, 1 );
//
//				size_t width = xxbind::extract<size_t>(_facade, i0);
//				size_t height = xxbind::extract<size_t>(_facade, i1);
//
//				impl->setWidth( width );
//				impl->setHeight( height );
//
//				return true;
//			}
//
//			return false;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		static bool Viewport_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( _facade->tuple_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) != 2 )
//				{
//					return false;
//				}
//
//				Viewport * impl = (Viewport *)_place;
//
//				xxbind::facade_object * i0 = _facade->tuple_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->tuple_getitem( _obj, 1 );
//
//				mt::vec2f begin = xxbind::extract<mt::vec2f>(_facade, i0);
//				mt::vec2f end = xxbind::extract<mt::vec2f>(_facade, i1);
//
//				impl->initialize( begin, end );
//
//				return true;
//			}
//			else if( _facade->list_check( _obj ) == true )
//			{
//				if( _facade->list_size( _obj ) != 2 )
//				{
//					return false;
//				}
//
//				Viewport * impl = (Viewport *)_place;
//
//				xxbind::facade_object * i0 = _facade->list_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->list_getitem( _obj, 1 );
//
//				mt::vec2f begin = xxbind::extract<mt::vec2f>(_facade, i0);
//				mt::vec2f end = xxbind::extract<mt::vec2f>(_facade, i1);
//
//				impl->initialize( begin, end );
//
//				return true;
//			}
//
//			return false;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		bool Rect_convert( xxbind::facade_script * _facade, xxbind::facade_object * _obj, void * _place )
//		{
//			if( _facade->tuple_check( _obj ) == true )
//			{
//				if( _facade->tuple_size( _obj ) != 4 )
//				{
//					return false;
//				}
//
//				Rect * impl = (Rect *)_place;
//
//				xxbind::facade_object * i0 = _facade->tuple_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->tuple_getitem( _obj, 1 );
//				xxbind::facade_object * i2 = _facade->tuple_getitem( _obj, 2 );
//				xxbind::facade_object * i3 = _facade->tuple_getitem( _obj, 3 );
//
//				impl->left = xxbind::extract<int>(_facade, i0);
//				impl->top = xxbind::extract<int>(_facade, i1);
//				impl->right = xxbind::extract<int>(_facade, i2);
//				impl->bottom = xxbind::extract<int>(_facade, i3);
//
//				return true;
//			}
//			else if( _facade->list_check( _obj ) == true )
//			{
//				if( _facade->list_size( _obj ) != 4 )
//				{
//					return false;
//				}
//
//				Rect * impl = (Rect *)_place;
//
//				xxbind::facade_object * i0 = _facade->list_getitem( _obj, 0 );
//				xxbind::facade_object * i1 = _facade->list_getitem( _obj, 1 );
//				xxbind::facade_object * i2 = _facade->list_getitem( _obj, 2 );
//				xxbind::facade_object * i3 = _facade->list_getitem( _obj, 3 );
//
//				impl->left = xxbind::extract<int>(_facade, i0);
//				impl->top = xxbind::extract<int>(_facade, i1);
//				impl->right = xxbind::extract<int>(_facade, i2);
//				impl->bottom = xxbind::extract<int>(_facade, i3);
//
//				return true;
//			}
//
//			return false;
//		}
//	}
//
//	void mathWrap( xxbind::facade_script * _facade, xxbind::module * _module )
//	{
//		xxbind::struct_<mt::vec2f>(_facade, _module, "vec2f")
//			.def_constructor( xxbind::init<float,float>() )
//			.def_convert( &ScriptMethod::vec2f_convert )
//			.def_static_sequence( &ScriptMethod::vec2_sequence )
//			.def_repr( &ScriptMethod::vec2f_repr )
//
//			.def_member( "x", &mt::vec2f::x )
//			.def_member( "y", &mt::vec2f::y )
//			//.attr( "x", &vec2f::x )
//			//.def( boost::python::init<float,float>() )
//			//.def( boost::python::self + boost::python::self )	// __add__
//			//.def( boost::python::self - boost::python::self )          // __radd__
//			//.def( boost::python::self * float() )           // __sub__
//			//.def( boost::python::self / float() )          // __sub__
//			//.def( float() * boost::python::self )         // __iadd__
//			//.def( boost::python::self += boost::python::self )
//			//.def( boost::python::self -= boost::python::self )
//			//.def( boost::python::self *= float() )
//			//.def( boost::python::self /= float() )
//			;
//
//		xxbind::struct_<mt::vec3f>(_facade, _module, "vec3f")
//			.def_constructor( xxbind::init<float,float,float>() )
//			.def_convert( &ScriptMethod::vec3f_convert )
//			.def_static_sequence( &ScriptMethod::vec3_sequence )
//			.def_member( "x", &mt::vec3f::x )
//			.def_member( "y", &mt::vec3f::y )
//			.def_member( "z", &mt::vec3f::z )
//			;
//
//		xxbind::struct_<mt::vec4f>(_facade, _module, "vec4f")
//			.def_constructor( xxbind::init<float,float,float,float>() )
//			.def_convert( &ScriptMethod::vec4f_convert )
//			.def_static_sequence( &ScriptMethod::vec4_sequence )
//			.def_member( "x", &mt::vec4f::x )
//			.def_member( "y", &mt::vec4f::y )
//			.def_member( "z", &mt::vec4f::z )
//			.def_member( "w", &mt::vec4f::w )
//			;
//
//		xxbind::struct_<Rect>(_facade, _module, "Rect")
//			.def_constructor( xxbind::init<size_t,size_t,size_t,size_t>() )
//			.def_convert( &ScriptMethod::Rect_convert )
//			.def_member( "left", &Rect::left )
//			.def_member( "top",  &Rect::top ) 
//			.def_member( "right",  &Rect::right )
//			.def_member( "bottom",  &Rect::bottom )
//			.def( "getWidth", &Rect::getWidth )
//			.def( "getHeight", &Rect::getHeight )
//			;
//
//		
//		xxbind::struct_<mt::quatf>(_facade, _module, "quatf")
//			.def_constructor( xxbind::init<float,float,float,float>() )
//			.def_member( "w", &mt::quatf::w )
//			.def_member( "x", &mt::quatf::x )
//			.def_member( "y", &mt::quatf::y )
//			.def_member( "z", &mt::quatf::z )			
//			;
//
//		xxbind::struct_<Polygon>(_facade, _module, "Polygon")
//			.def_convert( &ScriptMethod::Polygon_convert )
//			.def_static_facade("getPoints", &ScriptMethod::s_getPoints)
//			;
//
//		xxbind::struct_<Viewport>(_facade, _module, "Viewport")
//			.def_constructor( xxbind::init<mt::vec2f,mt::vec2f>() )
//			.def_convert( &ScriptMethod::Viewport_convert )
//			.def_member( "begin", &Viewport::begin )
//			.def_member( "end", &Viewport::end )
//			;
//		
//		xxbind::struct_<ColourValue>(_facade, _module, "Color")
//			.def_constructor( xxbind::init<float, float, float, float>() )
//			.def_convert( &ScriptMethod::color_convert )
//			.def( "getA", &ColourValue::getA )
//			.def( "getR", &ColourValue::getR )
//			.def( "getG", &ColourValue::getG )
//			.def( "getB", &ColourValue::getB )
//			.def_property( "a", &ColourValue::getA, &ColourValue::setA )
//			.def_property( "r", &ColourValue::getR, &ColourValue::setR )
//			.def_property( "g", &ColourValue::getG, &ColourValue::setG )
//			.def_property( "b", &ColourValue::getB, &ColourValue::setB )
//			.def_repr( &ScriptMethod::color_repr )
//			;
//
//		//pybind::interface_<Join>("Join")
//		//	.def( "getLeft", Join::getLeft )
//		//	.def( "getRight", Join::getRight )
//		//	;
//
//		xxbind::struct_<Resolution>(_facade, _module, "Resolution")
//			.def_constructor( xxbind::init<size_t, size_t>() )
//			.def_convert( &ScriptMethod::Resolution_convert )
//			.def( "setWidth", &Resolution::setWidth )
//			.def( "setHeight", &Resolution::setHeight )
//			.def( "getWidth", &Resolution::getWidth )
//			.def( "getHeight", &Resolution::getHeight )
//			.def( "getAspectRatio", &Resolution::getAspectRatio )
//			;
//	}
//}
