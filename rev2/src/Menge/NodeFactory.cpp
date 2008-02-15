#	include "NodeFactory.h"

#	include "Factory.h"

namespace Menge
{
	namespace NodeFactory
	{
		typedef Menge::Factory<std::string,Node*,FactoryGenStruct> TClassFactory;
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