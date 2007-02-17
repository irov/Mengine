#	include "NodeFactory.h"

#	include "Utility/factory.h"

namespace Menge
{
	namespace NodeFactory
	{
		typedef Utility::Factory<Node*,std::string,Node*,FactoryGenStruct> TClassFactory;
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
			TClassFactory::registration(_key,_func);
		}
		//////////////////////////////////////////////////////////////////////////
		Node * genNode(const std::string &_type)
		{
			FactoryGenStruct gs;
			gs.type = _type;
			Node * Abstract = TClassFactory::generate(_type,gs);
			return Abstract;
		}
		//////////////////////////////////////////////////////////////////////////
		Auto::Auto(const std::string &_key, TGenFunc _func)
		{
			TClassFactory::registration(_key,_func);
		}
	}
}