// ***************************************************************
//  Factory.h   version:  1.0   ·  date: 05/30/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include <map>

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
		class T_GEN_STRUCT = void,
		template<class> class T_POLICE_DEFAULT = PoliceNullPtr
	>
	class Factory
	{
	public:
		typedef T_KEY			TKey;
		typedef T_BASE			TBase;		
		typedef T_GEN_STRUCT	TGenStruct;

		template<class T>
		struct gen_function_type
		{
			typedef T_BASE (*type)( const T_GEN_STRUCT & );
		};

		template<>
		struct gen_function_type<void>
		{
			typedef T_BASE (*type)( );
		};

		typedef typename gen_function_type<T_GEN_STRUCT>::type TGenFunc;

	private:
		typedef std::map<TKey,TGenFunc>	TMapGenerator;

		static TMapGenerator & mapGenerator()
		{
			static TMapGenerator m;
			return m;
		}

	public:
		static void erase(const TKey &name)
		{
			mapGenerator().erase(name);
		}

		static size_t registration(const TKey &_key, TGenFunc _func)
		{				
			TMapGenerator::iterator it_find = 
				mapGenerator().find(_key);

			if( it_find == mapGenerator().end() )
			{
				mapGenerator().insert(std::make_pair(_key,_func));
			}

			return mapGenerator().size();
		}

		static TBase generate( const TKey & _name, const TGenStruct & _struct  )
		{
			typename TMapGenerator::const_iterator it_find = mapGenerator().find(_name);
			
			if( it_find != mapGenerator().end())
			{
				return it_find->second( _struct );
			}
			return T_POLICE_DEFAULT<TBase>::Police();
		}

		static void implement( const TKey &_name, const TGenStruct & _struct )
		{
			typename TMapGenerator::const_iterator it_find = mapGenerator().find(_name);

			if( it_find != mapGenerator().end())
			{
				it_find->second( _struct );
			}
		}

		static void implementAll( const TGenStruct & _struct )
		{
			for(
				typename TMapGenerator::const_iterator 
				it = mapGenerator().begin(),
				it_end = mapGenerator().end();
			it!=it_end;
			++it)
			{
				it->second( _struct );
			}
		}
	};
}