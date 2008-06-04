#	pragma once

#	include "Node.h"

namespace Menge
{
	class ResourceTileMap;

	class TileMap
		: public Node
	{
		OBJECT_DECLARE( TileMap );
	public:
		TileMap();
		~TileMap();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _render(  const Viewport & _viewport  ) override;
		//void _debugRender() override;

		//bool _activate() override;
		//void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _updateBoundingBox( mt::box2f & _localBoundingBox ) override;

		//void _update( float _timing ) override;

	private:
		std::string m_resourcename;
		ResourceTileMap* m_resourceMap;
		unsigned int m_width;
		unsigned int m_height;
	};
}	// namespace Menge