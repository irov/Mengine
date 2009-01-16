#	pragma once

#	include <map>

namespace Menge
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

		typedef T_BASE (*TGenFuncNew)( const T_GEN_STRUCT & ); ;
		typedef T_BASE (*TGenFuncPlacement)( void*, const T_GEN_STRUCT & ); ;

		class PoolBase
		{
		public:
			PoolBase( TGenFuncNew _genFuncNew, TGenFuncPlacement _genFuncPlacement )
				: m_genFuncNew( _genFuncNew )
				, m_genFuncPlacement( _genFuncPlacement )
			{
			}
			virtual ~PoolBase()
			{
			}

			virtual TBase generate( const TGenStruct& _genStruct ) = 0;
			virtual void release( TBase _pool_obj ) = 0;

		protected:
			TGenFuncNew m_genFuncNew;
			TGenFuncPlacement m_genFuncPlacement;
		};

		template< class T >
		class PoolManager : public PoolBase
		{
		public:
			PoolManager( TGenFuncNew _genFuncNew, TGenFuncPlacement _genFuncPlacement )
				: PoolBase( _genFuncNew, _genFuncPlacement )
			{
			}

			virtual ~PoolManager()
			{
				
				for( typename std::vector<T*>::iterator it = m_pool.begin(), it_end = m_pool.end();
					it != it_end;
					it++ )
				{

					m_genFuncPlacement( (*it), TGenStruct() );
					delete (*it);
					//::free( (*it) );
				}
				m_pool.clear();
			}

			virtual TBase generate( const TGenStruct& _genStruct )
			{
				if( m_pool.empty() == true )
				{
					return m_genFuncNew( _genStruct );
				}
				else
				{
					T* obj = m_pool.back();
					m_pool.pop_back();
					//obj->~T();
					//return new (obj) T();
					return m_genFuncPlacement( obj, _genStruct );
				}
			}
			virtual void release( TBase _pool_obj )
			{
				T* obj = static_cast<T*>( _pool_obj );
				obj->~T();
				m_pool.push_back( obj );
			}
		protected:
			std::vector<T*> m_pool;
		};

	private:
		//typedef std::map<TKey,TGenFunc>	TMapGenerator;
		typedef std::map<TKey, PoolBase*> TMapGenerator;

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

		//static void registration(const TKey &_key, TGenFunc _func)
		static void registration( const TKey &_key, PoolBase* _pool )
		{			
			typename TMapGenerator::iterator it_find = mapGenerator().find(_key);

			if( it_find == mapGenerator().end() )
			{
				//mapGenerator().insert(std::make_pair(_key,_func));
				mapGenerator().insert(std::make_pair(_key,_pool));
			}
		}

		static TBase generate( const TKey & _name, const TGenStruct & _struct  )
		{
			typename TMapGenerator::const_iterator it_find = mapGenerator().find(_name);
			
			if( it_find != mapGenerator().end())
			{
				//return it_find->second( _struct );
				return it_find->second->generate( _struct );
			}
			return T_POLICE_DEFAULT<TBase>::Police();
		}

		static void release( const TKey& _name, TBase _obj )
		{
			typename TMapGenerator::const_iterator it_find = mapGenerator().find(_name);

			if( it_find != mapGenerator().end())
			{
				//return it_find->second( _struct );
				it_find->second->release( _obj );
				return;
			}
			delete _obj;
		}

	public:
		struct Auto
		{
			/*Auto( const TKey & _key, TGenFunc _func)
			{
				Factory::registration( _key, _func );
			}*/
		};
	};
}
