#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/function_parser.hpp"
#	include "pybind/extract.hpp"

namespace pybind
{
	template<class F, class Ret, int i>
	struct def_call_impl
	{
		static Ret call( F f, PyObject * _arg );
	};

	template<class F, class Ret>
	struct def_call_impl<F,Ret,0>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			return (f)();
		}	
	};

	template<class F, class Ret>
	struct def_call_impl<F,Ret,1>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ) 
				);
		}	
	};

	template<class F, class Ret>
	struct def_call_impl<F,Ret,2>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ) 
				);
		}
	};

	template<class F, class Ret>
	struct def_call_impl<F,Ret,3>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ) 
				);
		}
	};

	template<class F, class Ret>
	struct def_call_impl<F,Ret,4>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 )
				);
		}
	};

	template<class F, class Ret>
	struct def_call_impl<F,Ret,5>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );

			return (f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ), 
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 )
				);
		}
	};

	template<class F, class Ret>
	struct def_call_ret_impl
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( F f, PyObject * _arg )
		{
			Ret result = def_call_impl<F,Ret,f_info::arity>::call( f, _arg );

			return ptr( result );
		}
	};

	template<class F>
	struct def_call_ret_impl<F,void>
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( F f, PyObject * _arg )
		{
			def_call_impl<F,void,f_info::arity>::call( f, _arg );

			return ret_none();
		}
	};

	template<class F>
	struct def_call
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( const char * _name, F f, PyObject * _arg )
		{
			size_t arg_size = (_arg)?tuple_size(_arg):0;

			if( arg_size != f_info::arity )
			{
				error_message("invalid function call '%s' args is not equal %d != %d\n"
					, _name
					, arg_size
					, f_info::arity
					);

				return 0;
			}

			try
			{
				return def_call_ret_impl<F,typename f_info::ret_type>::call( f, _arg );
			}
			catch( const pybind_exception & )
			{
			}

			return 0;
		}
	};
}

