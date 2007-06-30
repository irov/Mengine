#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"
#	include "Layer.h"

namespace Menge
{
	class Camera2D;

	class Scene
		: public NodeCore
		, public NodeChildren<Layer>
	{
		OBJECT_DECLARE( Scene )
	public:
		Scene();

	public:
		void layerAppend( const std::string & _layer, Node * _node );

	public:
		void loader( TiXmlElement *_xml) override;
		void render( const Camera2D * _camera );

	protected:
		bool _activate() override;

	private:
		std::string m_scriptFile;
	};
}