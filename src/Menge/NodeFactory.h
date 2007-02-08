#	pragma once

#	include "ObjectDeclare.h"
#	include <string>

namespace Menge
{
	class SceneManager;

	class Node;

	namespace NodeFactory
	{
		typedef Node *(*TGenFunc)(const FactoryGenStruct &);

		size_t getId(const std::string &_key);
		Node * genNode(const std::string &_type);

		void registration(const std::string &_key, TGenFunc _func);

		class Auto
		{
		public:
			Auto(const std::string &_key, TGenFunc _func);
		};
	}
};