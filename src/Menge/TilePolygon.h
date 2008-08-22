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
		void addVertex( const mt::vec2f & _vertex );

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
		typedef	std::pair< mt::TVectorPoints, mt::vec2f > TOutQuad;

		String m_resourcename;
		String m_penumbraUpName;
		String m_penumbraDownName;
		String m_penumbraLeftName;
		String m_penumbraRightName;
		
		mt::TVectorPoints m_poly;
		std::vector<mt::polygon> polys;
		mt::TVectorPoints m_triangles;
		//std::vector<mt::vec2f> m_penumbra_triangles;
		std::vector<TOutQuad> m_edgesUp;
		std::vector<TOutQuad> m_edgesDown;
		std::vector<TOutQuad> m_edgesLeft;
		std::vector<TOutQuad> m_edgesRight;
		
		ResourceImage * m_resource;
		ResourceImage * m_imagePenumbraUp;
		ResourceImage * m_imagePenumbraDown;
		ResourceImage * m_imagePenumbraLeft;
		ResourceImage * m_imagePenumbraRight;
		
		void compileEdges_( float _width, float _height, float _minAngle, float _maxAngle, std::vector<TOutQuad>& _edges );
		void renderEdges_( const std::vector<TOutQuad>& _edges, const RenderImageInterface* _image );
	};
};