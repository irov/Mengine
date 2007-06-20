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
		Entity * createEntity( 
			const std::string & _type, 
			const std::string & _name,
			const std::string & _layer,
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir );

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