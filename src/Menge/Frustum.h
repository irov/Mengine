#	pragma once

class TiXmlElement;

namespace Menge
{
	class Frustum
	{
	public:
		void setFov( float fovy );
		float getFov() const;

		void setNear( float _near );
		float getNear() const;

		void setFar( float _far );
		float getFar() const;

		void setAspect( float _aspect );
		float getAspect() const;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		float m_fov;
		float m_far;
		float m_near;
		float m_aspect;

		Frustum m_frustum;
		mt::planef	m_planes[6];

		float m_coeffL[2];
		float m_coeffR[2];
		float m_coeffB[2];
		float m_coeffT[2];
	};
}