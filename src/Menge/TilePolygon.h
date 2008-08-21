#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "math\polygon.h"
#	include "RigidBody2D.h"

namespace Menge
{
	class ResourceImage;
	class RenderImageInterface;

	class TilePolygon
		: public RigidBody2D
	{
		OBJECT_DECLARE(TilePolygon)

	public:
	
		TilePolygon();

	public:
		void loader( XmlElement * _xml ) override;
		void _render( unsigned int _debugMask ) override;

	protected:

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:
		String m_resourcename;
		String m_penumbraName;
		
		std::vector<mt::vec2f> m_poly;
		std::vector<mt::polygon> polys;
		std::vector<mt::vec2f> m_triangles;
		std::vector<mt::vec2f> m_penumbra_triangles;
		
		ResourceImage * m_resource;
		ResourceImage * m_imagePenumbra;

		void _renderPass( const std::vector<mt::vec2f> & _triangles, const RenderImageInterface* _image );
		void _addVertex( const mt::vec2f & _vertex );
	};
};