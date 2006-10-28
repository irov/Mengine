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

namespace Menge
{
	namespace NodeFactory
	{
		typedef Utility::Factory<std::string,Node*,TGenFunc> TClassFactory;
		//////////////////////////////////////////////////////////////////////////
		size_t getId(const std::string &_key)
		{
			typedef std::map<std::string, size_t> TMapId;
			static TMapId map_id;
			static size_t id = 0;

			TMapId::const_iterator it_find = map_id.find(_key);

			if( it_find == map_id.end() )
			{
				size_t id_insert = 
					map_id.insert(std::make_pair(_key,++id)).second;

				return id_insert;
			}

			return it_find->second;
		}
		//////////////////////////////////////////////////////////////////////////
		void registration(const std::string &_key, TGenFunc _func)
		{
			TClassFactory::Registration(_key,_func);
		}
		//////////////////////////////////////////////////////////////////////////
		Node * genNode(const std::string &_type)
		{
			Node * Abstract = TClassFactory::Generation(_type,_type);
			return Abstract;
		}
		//////////////////////////////////////////////////////////////////////////
		Auto::Auto(const std::string &_key, TGenFunc _func)
		{
			TClassFactory::Registration(_key,_func);
		}

	}
}