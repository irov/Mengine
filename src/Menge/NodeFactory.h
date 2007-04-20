#	pragma once

#	include "ObjectDeclare.h"
#	include <string>

namespace Menge
{
	class Node;

	namespace NodeFactory
	{
		typedef Node *(*TGenFunc)(const FactoryGenStruct &);

		Node * genNode(const std::string &_type);

		void registration(const std::string &_key, TGenFunc _func);

		struct Auto
		{
			Auto(const std::string &_key, TGenFunc _func);
		};
	}
}