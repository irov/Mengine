#	pragma once

#	include "SceneNode3D.h"

#	include "Interface/RenderSystemInterface.h"

#	include "math/mat4.h"

#	include <vector>

#	include <cal3d\cal3d.h>

namespace Menge
{
	class ResourceAnimationCal3d;

	struct Vertex
	{
		float pos[3];
		float normal[3];
		float t;
		float s;
	};

	class Mesh
		: public SceneNode3D
	{
		OBJECT_DECLARE(Mesh);
	public:
		Mesh();
		~Mesh();

	public:
		virtual bool isVisible( const Frustum & _frustum );

		void loader( TiXmlElement *xml ) override;

		void update( float _timing ) override;
	protected:
		bool _activate() override;
		void _deactivate() override;

		void _render( const mt::mat4f & _rwm, const Frustum & _frustum );
		void _debugRender() override;
	
	private:
		float	m_scale;
		float	m_leftAnimationTime;
		float	m_blendTime;
		int		m_currentAnimationId;
		std::string m_resourceName;

		CalCoreModel * m_calCoreModel;
		CalModel * m_calModel;
		ResourceAnimationCal3d * m_cal3dRes;

		VertexDeclaration * m_vertexDecl;
		PrimitiveData * m_primitiveData;

		void	initGeometry();
	};
}