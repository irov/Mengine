#	include "pybind/type_cast.hpp"
#	include "pybind/class_core.hpp"
#	include "pybind/class_type.hpp"

#	include "pybind/convert_adapter.hpp"

#	include "config/python.hpp"
#	include "pybind/system.hpp"

#	include <map>
#	include <string>

namespace pybind
{
	namespace detail
	{
		typedef std::map<const char *, type_cast *, pybind_ltstr> TMapExtractTypes;

		static TMapExtractTypes & mapExtractTypesInstance()
		{
			static TMapExtractTypes s_mapExtractTypes;
			return s_mapExtractTypes;
		}

		void register_type_info_extract( const std::type_info & _info, type_cast * _type )
		{
			const char * name = _info.name();
			TMapExtractTypes & types = mapExtractTypesInstance();
			types[ name ] = _type;
		}

		type_cast * find_type_info_extract( const std::type_info & _info )
		{
			const char * name = _info.name();
			TMapExtractTypes & types = mapExtractTypesInstance();
			TMapExtractTypes::iterator it_find = types.find( name );

			if( it_find == types.end() )
			{
				return 0;
			}

			return it_find->second;
		}

		void error_invalid_extract( PyObject * _obj, const std::type_info & _tinfo )
		{
			if( const char * repr = pybind::object_repr( _obj ) )
			{
				pybind::error_message( "invalid extract %s from %.256s"
					, _tinfo.name()
					, repr
					);
			}
			else
			{
				pybind::error_message( "invalid extract %s from unknown object type %s"
					, _tinfo.name()
					, _obj->ob_type->tp_name
					);
			}
		}

		bool convert_object( PyObject * _obj, const std::type_info & _tinfo, void * _place )
		{
			class_type_scope * scope = detail::get_class_type_scope( _tinfo );

			convert_adapter_interface * convert = scope->get_convert();

			if( convert == 0 )
			{
				return false;
			}

			bool result = convert->convert( _obj, _place );
			
			return result;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool type_cast::type_info_cast( PyObject * _obj, const std::type_info & _tinfo, const std::type_info & _tptrinfo, void ** _impl )
	{
		if( detail::is_class( _obj ) == false )
		{
			return false;
		}

		void * impl = detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "type_info_cast: unbind object" );
			return false;
		}

		class_type_scope * scope = detail::get_class_scope( _obj->ob_type );
		class_type_scope * cur_scope = detail::get_class_type_scope( _tinfo );

		void * result = 0;

		if( cur_scope != scope )
		{
			const std::type_info & tinfo = _tptrinfo;
			const char * name = tinfo.name();
			impl = class_core::meta_cast( impl, scope, name );
		}

		if( impl == 0 )
		{
			return false;
		}

		*_impl = impl;

		return true;
	}
}
