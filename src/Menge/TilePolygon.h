#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "RigidBody2D.h"
#	include "ResourceTilePolygon.h"

namespace Menge
{
	class ResourceImage;
	class RenderImageInterface;
	class ResourceTilePolygon;

	class TilePolygon
		: public RigidBody2D
	{
		OBJECT_DECLARE(TilePolygon)

	public:
		TilePolygon();

	public:
		void loader( XmlElement * _xml ) override;
		void _loaderVertices( XmlElement * _xml );
		void _render( unsigned int _debugMask ) override;

	protected:

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:

		void updateTileBoundingBox(mt::box2f& _boundingBox, const std::vector<Quad>& quads);

		std::vector<const std::vector<Quad>*> m_tileGeometry;
		std::vector<const std::vector<Quad>*> m_junkGeometry;

		ResourceTilePolygon * m_tilePolygonResource;
		String m_tileResource;

		const mt::TVectorPoints * m_triangles;
		const mt::TVectorPoints * m_uvs;

		RigidBody2D * m_rigidBodySensor;
	};
};