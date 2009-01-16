#	pragma once

#	include "Math/mat4.h"
#	include "Math/plane.h"

class TiXmlElement;

namespace Menge
{
	class Frustum
	{
	public:
		Frustum();

	public:
		void setFov( float _fov );
		float getFov() const;

		void setNear( float _near );
		float getNear() const;

		void setFar( float _far );
		float getFar() const;

		void setAspect( float _aspect );
		float getAspect() const;

	public:
		const mt::mat4f & getProjectionMatrix();

	public:
		void recalc( const mt::mat4f & _wm );
	
	public:
		void loader( TiXmlElement * _xml );

	protected:
		void recalcProjection();

	protected:
		mt::mat4f m_projectionMatrix;

		bool m_recalcProjection;

		float m_fov;
		float m_far;
		float m_near;
		float m_aspect;
		float m_focalLength;
		mt::vec2f m_frustumOffset;

		float m_coeffL[2];
		float m_coeffR[2];
		float m_coeffB[2];
		float m_coeffT[2];

	public:
		mt::planef	m_planes[6];
	};
}
