#	pragma once

#	include "NodeImpl.h"

#	include "math/vec2.h"

namespace Menge
{
	class Layer;
	class Entity;

	class Scene
		: public NodeImpl
	{
		OBJECT_DECLARE(Scene);

	public:
		Scene();

	public:
		void layerAppend( const std::string & _layer, Node * _node );

		Node * getEntity( const std::string & _name );

		template<class T>
		T * getEntityT( const std::string & _name )
		{
			return static_cast<T *>(getEntity(_name));
		}

	public:
		void loader(TiXmlElement *_xml) override;

	protected:
		bool _activate() override;

	private:
		std::string m_scriptFile;
	};
}