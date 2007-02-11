#	pragma once

namespace lua_boost
{
	namespace detail
	{
		template<class C, class F>
		struct member_types_impl
		{
			template<class T>
			struct types
			{
				typedef void ret_type;
			};

			template<class R>
			struct types< R (C::*)() >
			{
				typedef R ret_type;

				static const size_t arity = 0;
			};

			template<class R, class P1>
			struct types< R (C::*)(P1) >
			{
				typedef R ret_type;
				typedef P1 param1;

				static const size_t arity = 1;
			};

			template<class R, class P1, class P2>
			struct types< R (C::*)(P1, P2) >
			{
				typedef R ret_type;
				typedef P1 param1;
				typedef P2 param2;

				static const size_t arity = 2;
			};

			template<class R, class P1, class P2, class P3>
			struct types< R (C::*)(P1, P2, P3) >
			{
				typedef R ret_type;
				typedef P1 param1;
				typedef P2 param2;
				typedef P3 param3;

				static const size_t arity = 3;
			};

			template<class R, class P1, class P2, class P3, class P4>
			struct types< R (C::*)(P1, P2, P3, P4) >
			{
				typedef R ret_type;
				typedef P1 param1;
				typedef P2 param2;
				typedef P3 param3;
				typedef P4 param4;

				static const size_t arity = 4;
			};

			typedef types<F> result;
		};
	}

	template<class C, class F>
	struct member_types
		: public detail::member_types_impl<C,F>::result
	{

	};
}