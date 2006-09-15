// ***************************************************************
//  FactoryObject.cpp   version:  1.0   ·  date: 05/07/2006
//  -------------------------------------------------------------
//  IROV
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#	include "NodeFactory.h"

#	include "Utility/factory.h"

namespace RvEngine
{
	namespace NodeFactory
	{
		typedef Utility::CFactory<std::string,Node*,TGenFunc> TClassFactory;

		typedef TClassFactory::TGlobalFactory TGlobalClassFactory;

		typedef std::map<std::string, size_t> TMapId;
		typedef Utility::CSingleton<TMapId> TGlobalMapId;


		//////////////////////////////////////////////////////////////////////////
		static TClassFactory *getClassFactory()
		{
			TClassFactory *ClassFactory = 
				TGlobalClassFactory::GetInstance();

			return ClassFactory;
		}

		//////////////////////////////////////////////////////////////////////////
		size_t getId(const std::string &_key)
		{
			static size_t Id = 0;

			TMapId *Map = TGlobalMapId::GetInstance();
			TMapId::iterator it_find = 
				Map->lower_bound(_key);

			if( it_find == Map->end() || it_find->first != _key )
			{
				size_t Id_insert = 
					Map->insert(it_find,std::make_pair(_key,++Id))->second;

				return Id_insert;
			}

			return it_find->second;
		}
		//////////////////////////////////////////////////////////////////////////
		void registration(const std::string &_key, TGenFunc _func)
		{
			getClassFactory()->Registration(_key,_func);
		}
		//////////////////////////////////////////////////////////////////////////
		Node * genNode(const std::string &_type,SceneManager *_sceneManager)
		{
			Node * Abstract = 
				getClassFactory()->Generation(_type,_sceneManager,_type);

			return Abstract;
		}
		//////////////////////////////////////////////////////////////////////////
		Auto::Auto(const std::string &_key, TGenFunc _func)
		{
			getClassFactory()->Registration(_key,_func);
		}

	}
}