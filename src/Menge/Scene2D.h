#	pragma once

#	include "NodeImpl.h"

namespace Menge
{
	class Layer;
	class Scene2DNode;
	class Viewport;

	class Scene2D
	{
		OBJECT_DECLARE(Scene);

	public:
		Scene2D();

	public:
		Scene2DNode * createNode(const std::string &_name, const std::string &_type);

		template<class T>
		T* createNodeT(const std::string &_name, const std::string &_type)
		{
			return static_cast<T*>(createNode(_name,_type));
		}

		Scene2DNode * createNodeFromXml( const std::string &_file);

		template<class T>
		Scene2DNode * createNodeFromXmlT( const std::string &_file)
		{
			return static_cast<T*>(createNodeFromXml(_file));
		}

		bool loadNode( Scene2DNode *_node, const std::string &_xml );
		void renderNode( Scene2DNode * _node, const Viewport & _viewPort );

		Scene2DNode * getNode( const std::string & _name );

		template<class T>
		T * getNodeT( const std::string & _name )
		{
			return static_cast<T *>(getEntity(_name));
		}

	public:
		void loader(TiXmlElement *_xml);
		bool compile() ;

	private:
		std::string m_scriptFile;

	};
}