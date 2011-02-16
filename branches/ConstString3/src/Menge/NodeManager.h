#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "Core/ConstString.h"

#	include "FactoryManager.h"


namespace Menge
{
	class Node;	
	class NodeFactory;

	class NodeManager
		: public Holder<NodeManager>
		, public FactoryManager
	{
	public:
		NodeManager();

	public:
		Node * createNode( const ConstString& _name, const ConstString& _type, const ConstString& _tag );
		
		template<class T>
		T * createNodeT( const ConstString& _name, const ConstString& _type, const ConstString& _tag )
		{
			return static_cast<T*>(this->createNode( _name, _type, _tag ));
		}

		//Node * createNodeFromXml( const ConstString& _pakName, const String& _filename );
		Node * createNodeFromBinary( const ConstString& _name, const ConstString& _tag, const ConstString & _binResource );

		template<class T>
		T * createNodeFromBinaryT( const ConstString& _name, const ConstString& _tag, const ConstString& _file)
		{
			return dynamic_cast<T*>(this->createNodeFromBinary( _name, _tag, _file ));
		}

	public:
		bool loadNode( Node *_node, const ConstString& _pakName, const String& _filename );

	protected:
		typedef std::map<ConstString, NodeFactory *> TMapGenerator;
		TMapGenerator m_generator;
	};
}
