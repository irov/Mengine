// ***************************************************************
//  Factory.h   version:  1.0   ·  date: 05/30/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#if !BOOST_PP_IS_ITERATING

#	ifndef IROV_AUTO_CLASS_FACTORY_INCLUDE
#	define IROV_AUTO_CLASS_FACTORY_INCLUDE

#	include <map>

#	include <boost/preprocessor/repetition/enum.hpp>
#	include <boost/preprocessor/repetition/enum_params.hpp>
#	include <boost/preprocessor/repetition/enum_binary_params.hpp>
#	include <boost/preprocessor/iteration/iterate.hpp>


namespace Utility
{

	template<class T>
	struct PoliceNullPtr
	{
		static T Police()
		{
			return static_cast<T>(0);
		}
	};

	template<
		class T_KEY,
		class T_BASE,
		class T_GEN_FUNC,
			template<class> class T_POLICE_DEFAULT = PoliceNullPtr
	>
	class Factory
	{
	public:
		typedef T_KEY		TKey;
		typedef T_GEN_FUNC	TGenFunc;
		typedef T_BASE		TBase;

	private:
		typedef std::map<TKey,T_GEN_FUNC>	TMapGenerator;

		static TMapGenerator & mapGenerator()
		{
			static TMapGenerator m;
			return m;
		}

	public:
		static void Erase(const TKey &name)
		{
			mapGenerator().erase(name);
		}

		static size_t Registration(const TKey &_key, TGenFunc _func)
		{				
			TMapGenerator::iterator it_find = 
				mapGenerator().find(_key);

			if( it_find == mapGenerator().end() )
			{
				mapGenerator().insert(it_find,std::make_pair(_key,_func));
			}

			return mapGenerator().size();
		}

	public:
#	define BOOST_PP_ITERATION_PARAMS_1 (4, (0, 4, "Utility/factory.h", 1))
#	include BOOST_PP_ITERATE()
	};
}

#endif

#else 

#	if BOOST_PP_ITERATION() 
template<
BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename T_VALUE_)
>
#	endif

inline static TBase Generation
(
 const TKey &name
 BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
 BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), const T_VALUE_ ,  &t_value_ )
 )
{
	typename TMapGenerator::const_iterator it_find = mapGenerator().find(name);
	if( it_find != mapGenerator().end())
	{
		return it_find->second( BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), t_value_ ) );
	}
	return static_cast<TBase>(T_POLICE_DEFAULT<TBase>::Police());
}


#	if BOOST_PP_ITERATION() 
template<
BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename T_VALUE_)
>
#	endif

inline static void ImplementAll
(
 BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), T_VALUE_ ,  &t_value_ )
 )
{
	for(
		typename TMapGenerator::const_iterator 
		it = mapGenerator().begin(),
		it_end = mapGenerator().end();
		it!=it_end;
	++it)
	{
		it->second( BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), t_value_ ) );
	}
}

#	if BOOST_PP_ITERATION() 
template<
BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename T_VALUE_)
>
#	endif

inline static void Implement
(
 const TKey &name
 BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
 BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), const T_VALUE_ ,  &t_value_ )
 )
{
	typename TMapGenerator::const_iterator it_find = mapGenerator().find(name);
	if( it_find != mapGenerator().end())
	{
		it_find->second( BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), t_value_ ) );
	}
}


#endif
