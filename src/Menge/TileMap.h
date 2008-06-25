#	pragma once
#	include "SceneNode2D.h"

namespace Menge
{
	class ResourceTileMap;

	class TileMap
		: public SceneNode2D
	{
		OBJECT_DECLARE( TileMap );
	public:
		TileMap();
		~TileMap();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _render( bool _enableDebug ) override;

		//bool _activate() override;
		//void _deactivate() override;

		bool _compile() override;
		void _release() override;

		//void _update( float _timing ) override;

	private:
		std::string m_resourcename;
		ResourceTileMap* m_resourceMap;
		unsigned int m_width;
		unsigned int m_height;
	};
}	// namespace Menge