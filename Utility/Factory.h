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

#	include "singleton.h"

#	include <map>

#	include <boost/preprocessor/repetition/enum.hpp>
#	include <boost/preprocessor/repetition/enum_params.hpp>
#	include <boost/preprocessor/repetition/enum_binary_params.hpp>
#	include <boost/preprocessor/iteration/iterate.hpp>


namespace Utility
{

	template<class T>
	struct CPoliceNullPtr
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
			template<class> class T_POLICE_DEFAULT = CPoliceNullPtr
	>
	class CFactory
	{
	public:
		typedef T_KEY		TKey;
		typedef T_GEN_FUNC	TGenFunc;
		typedef T_BASE		TBase;

	private:
		typedef std::map<TKey,T_GEN_FUNC>	TMapGenerator;

	public:
		void Erase(const TKey &name)
		{
			m_MapGeneration.erase(name);
		}

		size_t Registration(const TKey &Key, TGenFunc Func)
		{	
			TMapGenerator::iterator it_find = 
				m_MapGeneration.lower_bound(Key);

			if( it_find == m_MapGeneration.end() || it_find->first != Key )
			{
				m_MapGeneration.insert(it_find,std::make_pair(Key,Func));
			}

			return m_MapGeneration.size();
		}

		TGenFunc GetFunc(const TKey &name)const
		{
			TMapGenerator::iterator it_find = 
				m_MapGeneration.lower_bound(Key);

			if( it_find == m_MapGeneration.end() || it_find->first != Key )
			{
				return TGenFunc(0);
			}

			return it_find->second;
		}

	public:
		class CAuto
		{
			typedef CSingleton<CFactory,CAuto> TGlobalFactory;

		public:
			CAuto(const TKey &Key, TGenFunc Func)
			{
				TGlobalFactory::GetInstance()->Registration(Key,Func);
			}
		};

		typedef CSingleton<CFactory,CAuto> TGlobalFactory;

#	define BOOST_PP_ITERATION_PARAMS_1 (4, (0, 4, "Utility/factory.h", 1))
#	include BOOST_PP_ITERATE()

	private:
		TMapGenerator m_MapGeneration;
	};

}

#endif

#else 

#	if BOOST_PP_ITERATION() 
template<
BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename T_VALUE_)
>
#	endif

inline TBase Generation
(
 const TKey &name
 BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
 BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), const T_VALUE_ ,  &t_value_ )
 )const
{
	typename TMapGenerator::const_iterator it_f = m_MapGeneration.find(name);
	if( it_f != m_MapGeneration.end())
	{
		return it_f->second( BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), t_value_ ) );
	}
	return static_cast<TBase>(T_POLICE_DEFAULT<TBase>::Police());
}


#	if BOOST_PP_ITERATION() 
template<
BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), typename T_VALUE_)
>
#	endif

inline void ImplementAll
(
 BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), T_VALUE_ ,  &t_value_ )
 )const
{
	typename TMapGenerator::const_iterator it_end = m_MapGeneration.end();
	for(
		typename TMapGenerator::const_iterator it = m_MapGeneration.begin();
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

inline void Implement
(
 const TKey &name
 BOOST_PP_COMMA_IF(BOOST_PP_ITERATION())
 BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_ITERATION(), const T_VALUE_ ,  &t_value_ )
 )const
{
	typename TMapGenerator::const_iterator it_f = m_MapGeneration.find(name);
	if( it_f != m_MapGeneration.end())
	{
		it_f->second( BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), t_value_ ) );
	}
}


#endif
