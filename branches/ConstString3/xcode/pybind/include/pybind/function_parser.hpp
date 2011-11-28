#	pragma once

#	include <cstddef>

namespace pybind
{
	template<class T>
	struct external_type_cast
	{
		typedef T Type;
	};

	namespace detail
	{
		template<class T>
		struct function_types
		{
			typedef void ret_type;
		};

		template<class R>
		struct function_types< R (*)() >
		{
			typedef R ret_type;

			static const bool member = false;
			static const std::size_t arity = 0;
		};

		template<class R, class P0>
		struct function_types< R (*)(P0) >
		{
			typedef R ret_type;

			typedef typename external_type_cast<P0>::Type param0;

			static const bool member = false;
			static const std::size_t arity = 1;
		};

		template<class R, class P0, class P1>
		struct function_types< R (*)(P0, P1) >
		{
			typedef R ret_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;

			static const bool member = false;
			static const std::size_t arity = 2;
		};

		template<class R, class P0, class P1, class P2>
		struct function_types< R (*)(P0, P1, P2) >
		{
			typedef R ret_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;

			static const bool member = false;
			static const std::size_t arity = 3;
		};


		template<class R, class P0, class P1, class P2, class P3>
		struct function_types< R (*)(P0, P1, P2, P3) >
		{
			typedef R ret_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;

			static const bool member = false;
			static const std::size_t arity = 4;
		};

		template<class R, class P0, class P1, class P2, class P3, class P4>
		struct function_types< R (*)(P0, P1, P2, P3, P4) >
		{
			typedef R ret_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;

			static const bool member = false;
			static const std::size_t arity = 5;
		};

		template<class R, class P0, class P1, class P2, class P3, class P4, class P5>
		struct function_types< R (*)(P0, P1, P2, P3, P4, P5) >
		{
			typedef R ret_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;
			typedef typename external_type_cast<P5>::Type param5;

			static const bool member = false;
			static const std::size_t arity = 6;
		};

		template<class R, class C>
		struct function_types< R (C::*)() >
		{
			typedef R ret_type;
			typedef C class_type;

			static const bool member = true;
			static const std::size_t arity = 0;
		};

		template<class R, class C>
		struct function_types< R (C::*)() const>
		{
			typedef R ret_type;
			typedef C class_type;

			static const bool member = true;
			static const std::size_t arity = 0;
		};

		template<class R, class C, class P0>
		struct function_types< R (C::*)(P0) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;

			static const bool member = true;
			static const std::size_t arity = 1;
		};

		template<class R, class C, class P0>
		struct function_types< R (C::*)(P0) const >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;

			static const bool member = true;
			static const std::size_t arity = 1;
		};

		template<class R, class C, class P0, class P1>
		struct function_types< R (C::*)(P0, P1) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;

			static const bool member = true;
			static const std::size_t arity = 2;
		};

		template<class R, class C, class P0, class P1>
		struct function_types< R (C::*)(P0, P1) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;

			static const bool member = true;
			static const std::size_t arity = 2;
		};

		template<class R, class C, class P0, class P1, class P2>
		struct function_types< R (C::*)(P0, P1, P2) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;

			static const bool member = true;
			static const std::size_t arity = 3;
		};

		template<class R, class C, class P0, class P1, class P2>
		struct function_types< R (C::*)(P0, P1, P2) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;

			static const bool member = true;
			static const std::size_t arity = 3;
		};

		template<class R, class C, class P0, class P1, class P2, class P3>
		struct function_types< R (C::*)(P0, P1, P2, P3) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;

			static const bool member = true;
			static const std::size_t arity = 4;
		};

		template<class R, class C, class P0, class P1, class P2, class P3>
		struct function_types< R (C::*)(P0, P1, P2, P3) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;

			static const bool member = true;
			static const std::size_t arity = 4;
		};

		template<class R, class C, class P0, class P1, class P2, class P3, class P4>
		struct function_types< R (C::*)(P0, P1, P2, P3, P4) >
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;

			static const bool member = true;
			static const std::size_t arity = 5;
		};

		template<class R, class C, class P0, class P1, class P2, class P3, class P4>
		struct function_types< R (C::*)(P0, P1, P2, P3, P4) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;

			static const bool member = true;
			static const std::size_t arity = 5;
		};

		template<class R, class C, class P0, class P1, class P2, class P3, class P4, class P5>
		struct function_types< R (C::*)(P0, P1, P2, P3, P4, P5) const>
		{
			typedef R ret_type;
			typedef C class_type;

			typedef typename external_type_cast<P0>::Type param0;
			typedef typename external_type_cast<P1>::Type param1;
			typedef typename external_type_cast<P2>::Type param2;
			typedef typename external_type_cast<P3>::Type param3;
			typedef typename external_type_cast<P4>::Type param4;
			typedef typename external_type_cast<P5>::Type param5;

			static const bool member = true;
			static const std::size_t arity = 6;
		};
	}

	template<class F>
	struct function_parser
	{
		typedef detail::function_types<F> result;
	};
}

