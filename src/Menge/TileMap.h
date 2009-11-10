#	pragma once
#	include "Node.h"

namespace Menge
{
	class ResourceTileMap;

	class TileMap
		: public Node
	{
		FACTORABLE_DECLARE( TileMap );
	public:
		TileMap();
		~TileMap();

	public:
		void loader( XmlElement * _xml ) override;

	protected:

		//bool _activate() override;
		//void _deactivate() override;

		bool _compile() override;
		void _release() override;

		//void _update( float _timing ) override;
		void _render( Camera2D * _camera ) override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:
		String m_resourcename;
		ResourceTileMap* m_resourceMap;
		std::size_t m_width;
		std::size_t m_height;
	};
}	// namespace Menge
