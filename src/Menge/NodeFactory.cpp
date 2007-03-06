#	include "NodeFactory.h"

#	include "Factory.h"

namespace Menge
{
	namespace NodeFactory
	{
		typedef Utility::Factory<Node*,std::string,Node*,FactoryGenStruct> TClassFactory;
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