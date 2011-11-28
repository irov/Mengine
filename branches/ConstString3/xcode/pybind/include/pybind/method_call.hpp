#	pragma once

#	include "pybind/exports.hpp"
#	include "pybind/function_parser.hpp"
#	include "pybind/types.hpp"
#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

namespace pybind
{
	template<class C, class F,class Ret, int i>
	struct method_call_impl
	{
		static Ret call( C * _obj, F f, PyObject * _arg );
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,0>
	{
		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			return (_obj->*f)();
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,1>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );

			return (_obj->*f)( 
				extract<typename f_info::param0>( _arg0 ) 
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,2>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );

			return (_obj->*f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ) 
				);
		}	
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,3>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );

			return (_obj->*f)( 
				extract<typename f_info::param0>( _arg0 ),
				extract<typename f_info::param1>( _arg1 ),
				extract<typename f_info::param2>( _arg2 )
				);
		}	
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,4>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );

			return (_obj->*f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ),
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_impl<C,F,Ret,5>
	{
		typedef typename function_parser<F>::result f_info;

		static Ret call( C * _obj, F f, PyObject * _arg )
		{
			PyObject * _arg0 = tuple_getitem( _arg, 0 );
			PyObject * _arg1 = tuple_getitem( _arg, 1 );
			PyObject * _arg2 = tuple_getitem( _arg, 2 );
			PyObject * _arg3 = tuple_getitem( _arg, 3 );
			PyObject * _arg4 = tuple_getitem( _arg, 4 );

			return (_obj->*f)( 
				extract<typename f_info::param0>( _arg0 ), 
				extract<typename f_info::param1>( _arg1 ),
				extract<typename f_info::param2>( _arg2 ),
				extract<typename f_info::param3>( _arg3 ),
				extract<typename f_info::param4>( _arg4 )
				);
		}
	};

	template<class C, class F,class Ret>
	struct method_call_ret_impl
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( C * _obj, F f, PyObject * _arg )
		{
			Ret result = method_call_impl<C,F,Ret,f_info::arity>::call( _obj, f, _arg );
			return ptr<Ret>( result );
		}
	};

	template<class C, class F>
	struct method_call_ret_impl<C,F,void>
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( C * _obj, F f, PyObject * _arg )
		{
			method_call_impl<C,F,void,f_info::arity>::call( _obj, f, _arg );
			return ret_none();
		}
	};

	template<class C, class F>
	struct method_call
	{
		typedef typename function_parser<F>::result f_info;

		static PyObject * call( C * _obj, F f, PyObject * _arg, const char * _tag )
		{
			size_t arg_size = (_arg)?tuple_size(_arg):0;

			if( arg_size != f_info::arity )
			{
				error_message("invalid method call args is not equal %d != %d in '%s'\n"
					, arg_size
					, f_info::arity
					, _tag
					);

				return 0;
			}

			try
			{
				return method_call_ret_impl<C,F,typename f_info::ret_type>::call( _obj, f, _arg );
			}
			catch( const pybind_exception & )
			{
				error_message("invalid method call '%s'\n"
					, _tag
					);
			}

			return 0;			
		}
	};
}

