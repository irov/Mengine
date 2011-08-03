#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"

#	include "FactoryManager.h"

class XmlElement;

namespace Menge
{
	class Node;	
	class NodeFactory;

	class SceneManager
		: public FactoryManager
		, public Holder<SceneManager>
	{
	public:
		Node * createNode( const String& _type );
		
		template<class T>
		T * createNodeT( const String& _type )
		{
			return dynamic_cast<T*>( createNode( _type ) );
		}

		Node * createNodeFromXml( const String& _pakName, const String& _filename );
		Node * createNodeFromXmlData( const String& _xml_data );

		bool loadNode(Node *_node, const String& _pakName, const String& _filename );

		void initialize();

	protected:
		typedef std::map<String, NodeFactory *> TMapGenerator;
		TMapGenerator m_generator;
	};
}
