#	pragma once

#	include "pybind/extract.hpp"
#	include "pybind/exports.hpp"

namespace pybind
{
	class PYBIND_API constructor
	{
	public:
		constructor( std::size_t _arity );
		virtual ~constructor();

	public:
		bool valid( PyObject * _args ) const;
		size_t arity() const;

	public:
		virtual void * call( PyObject * _args ) = 0;

	protected:
		size_t m_arity;
	};

	template<class C, class P, int i>
	struct call_ctr_impl
	{
		static C * call_ctr( PyObject * _args );
	};

	template<class C, class P>
	struct call_ctr_impl<C, P, 0>
	{
		static C * call_ctr( PyObject * _args )
		{
			return new C();	
		}
	};

	template<class C, class P>
	struct call_ctr_impl<C, P, 1>
	{
		static C * call_ctr( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );

			return new C( param0 );
		}
	};

	template<class C, class P>
	struct call_ctr_impl<C, P, 2>
	{
		static C * call_ctr( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );
			PyObject * arg1 = tuple_getitem( _args, 1 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );

			return new C( param0, param1 );
		}
	};

	template<class C, class P>
	struct call_ctr_impl<C, P, 3>
	{
		static C * call_ctr( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );
			PyObject * arg1 = tuple_getitem( _args, 1 );
			PyObject * arg2 = tuple_getitem( _args, 2 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );
			typename P::param2 param2 = extract<typename P::param2>( arg2 );

			return new C( param0, param1, param2 );
		}
	};

	template<class C, class P>
	struct call_ctr_impl<C, P, 4>
	{
		static C * call_ctr( PyObject * _args )
		{
			PyObject * arg0 = tuple_getitem( _args, 0 );
			PyObject * arg1 = tuple_getitem( _args, 1 );
			PyObject * arg2 = tuple_getitem( _args, 2 );
			PyObject * arg3 = tuple_getitem( _args, 3 );

			typename P::param0 param0 = extract<typename P::param0>( arg0 );
			typename P::param1 param1 = extract<typename P::param1>( arg1 );
			typename P::param2 param2 = extract<typename P::param2>( arg2 );
			typename P::param3 param3 = extract<typename P::param3>( arg3 );

			return new C( param0, param1, param2, param3 );
		}
	};

	template<class C, class P>
	class constructor_params
		: public constructor
	{
	public:
		constructor_params()
			: constructor( P::base_arity )
		{
		}

	public:
		void * call( PyObject * _args ) override
		{
			if( this->valid( _args ) == false )
			{
				return 0;
			}

			try
			{
				return call_ctr_impl<C, P, P::base_arity>::call_ctr( _args );
			}
			catch( const pybind_exception & )
			{
			}

			return 0;
		}
	};
}

