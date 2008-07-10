#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "SceneNode2D.h"

namespace Menge
{
	class ResourceImage;

	class TilePolygon
		: public SceneNode2D
	{
		OBJECT_DECLARE(TilePolygon)

	public:
	
		TilePolygon();

	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( XmlElement * _xml ) override;

	protected:
		void _render( bool _enableDebug ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	private:
		std::string m_resourcename;
		
		std::vector<mt::vec2f> m_poly;
		std::vector<mt::vec2f> m_triangles;
		std::vector<mt::vec2f> m_penumbra_triangles;
		
		ResourceImage * m_resource;

		void _renderPass(const std::vector<mt::vec2f> & _triangles);
		void _addVertex(const mt::vec2f & _vertex);
	};
};