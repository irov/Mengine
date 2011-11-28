#	pragma once

namespace pybind
{
	namespace detail
	{
		template<class C>
		struct is_void
		{
			static const int result = 1; 
		};

		template<>
		struct is_void<void>
		{
			static const int result = 0;
		};
	}


	template<
		class C0 = void  
		, class C1 = void  
		, class C2 = void  
		, class C3 = void  
		, class C4 = void  
		, class C5 = void  
		, class C6 = void  
		, class C7 = void
		, class C8 = void
	>
	class bases
	{
	public:
		typedef C0 base0;
		typedef C1 base1;
		typedef C2 base2;
		typedef C3 base3;
		typedef C4 base4;
		typedef C5 base5;
		typedef C6 base6;
		typedef C7 base7;
		typedef C8 base8;

		static const int base_arity =
			detail::is_void<C0>::result
			+ detail::is_void<C1>::result
			+ detail::is_void<C2>::result 
			+ detail::is_void<C3>::result 
			+ detail::is_void<C4>::result 
			+ detail::is_void<C5>::result
			+ detail::is_void<C6>::result
			+ detail::is_void<C7>::result
			+ detail::is_void<C8>::result
			; 
	};


	template<
		class C0 = void  
		, class C1 = void  
		, class C2 = void  
		, class C3 = void  
		, class C4 = void  
		, class C5 = void  
		, class C6 = void  
		, class C7 = void
		, class C8 = void
	>
	class init
		: public bases<C0,C1,C2,C3,C4,C5,C6,C7,C8>
	{
	public:
		typedef C0 param0;
		typedef C1 param1;
		typedef C2 param2;
		typedef C3 param3;
		typedef C4 param4;
		typedef C5 param5;
		typedef C6 param6;
		typedef C7 param7;
		typedef C8 param8;
	};
}

