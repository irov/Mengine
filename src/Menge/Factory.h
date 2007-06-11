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

	struct EmptyFactoryParam{};

	template<
		class T_KEY,
		class T_BASE,
		class T_GEN_STRUCT = EmptyFactoryParam,
		template<class> class T_POLICE_DEFAULT = PoliceNullPtr
	>
	class Factory
	{
	public:
		typedef T_KEY			TKey;
		typedef T_BASE			TBase;		
		typedef T_GEN_STRUCT	TGenStruct;

		typedef T_BASE (*TGenFunc)( const T_GEN_STRUCT & ); ;

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
			TMapGenerator::iterator it_find = mapGenerator().find(_key);

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

	public:
		struct Auto
		{
			Auto( const TKey & _key, TGenFunc _func)
			{
				Factory::registration( _key, _func );
			}
		};
	};
}