#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/types.hpp"
#	include <typeinfo>

namespace pybind
{
	class type_cast;

	namespace detail
	{
		PYBIND_API void register_type_info_extract( const std::type_info & _info, type_cast * _type );
		PYBIND_API type_cast * find_type_info_extract( const std::type_info & _info );

		PYBIND_API void error_invalid_extract( PyObject * _obj, const std::type_info & _tinfo );
		PYBIND_API bool convert_object( PyObject * _obj, const std::type_info & _tinfo, void * _place );

		template<class T> struct type_down_cast;

		template<class T, class B>
		struct type_down_cast_find
		{
			static type_cast * find()
			{  
				const std::type_info & tinfo =  typeid(T);

				type_cast * etype = find_type_info_extract( tinfo );

				if( etype == 0 )
				{
					return type_down_cast<B>::find();
				}

				return etype;
			}
		};

		template<class B>
		struct type_down_cast_find<PyObject, B>
		{
			static type_cast * find()
			{  
				return (type_cast *)0;
			}
		};

		template<class T>
		struct type_down_cast
		{
			static type_cast * find()
			{
				return type_down_cast_find<T,void>::find();
			}
		};

		template<>
		struct type_down_cast<void>
		{
			static type_cast * find()
			{
				return 0;
			}
		};

		template<class T>
		struct type_down_cast<T *>
		{
			static type_cast * find()
			{
				return type_down_cast_find<T *,T>::find();
			}
		};

		template<class T>
		struct type_down_cast<const T *>
		{
			static type_cast * find()
			{
				return type_down_cast_find<const T *,T>::find();
			}
		};

		template<class T>
		struct type_down_cast<T &>
		{
			static type_cast * find()
			{
				return type_down_cast_find<T &,T>::find();
			}
		};

		template<class T>
		struct type_down_cast<const T &>
		{
			static type_cast * find()
			{
				return type_down_cast_find<const T &,T>::find();
			}
		};
	}


	template<class T>
	void registration_type_cast( type_cast * _type )
	{
		const std::type_info & tinfo = typeid(T);

		detail::register_type_info_extract( tinfo, _type );
	}


	class type_cast
	{
	public:
		type_cast()
		{
		}

		virtual ~type_cast()
		{
		}

	protected:
		bool type_info_cast( PyObject * _obj, const std::type_info & _tinfo, const std::type_info & _tptrinfo, void ** _impl );
	};

	template<class T, bool V> 
	class type_cast_result_cr
		: public type_cast
	{
	public:
		typedef const T & TCastRef;

	public:
		virtual PyObject * wrap( const T & t ) = 0;
	};

	template<class T> 
	class type_cast_result_cr<T, false>
		: public type_cast
	{
	public:
		typedef T TCastRef;

	public:
		virtual PyObject * wrap( T t ) = 0;		
	};

	template<class T, bool V = (sizeof(T) > 4)>
	class type_cast_result
		: public type_cast_result_cr<T, V>
	{
	public:
		type_cast_result()
			: type_cast_result_cr<T, V>()
		{
			registration_type_cast<T>( this );
		}

	public:
		virtual bool apply( PyObject * _obj, T & _value ) = 0;
	};

	void initialize_default_type_cast();
	void finialize_default_type_cast();
}