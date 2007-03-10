#	pragma once

#	include "NodeImpl.h"

namespace Menge
{
	class Layer;

	class Scene
		: public NodeImpl
	{
		OBJECT_DECLARE(Scene);

	public:
		Scene();

	public:
		Node * getEntity( const std::string & _name );

		template<class T>
		T * getEntityT( const std::string & _name )
		{
			return static_cast<T *>(getEntity(_name));
		}

	public:
		void loader(TiXmlElement *_xml) override;

	protected:
		bool _compile() override;

	private:
		std::string m_scriptFile;
	};
}