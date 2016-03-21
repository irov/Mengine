#	pragma once

#	include "Kernel/RenderCamera.h"

namespace Menge
{
	class RenderCameraProjection
		: public RenderCamera
	{
	public:
		RenderCameraProjection();
				
	public:
		void setCameraPosition( const mt::vec3f & _position );
		void setCameraDirection( const mt::vec3f & _direction );
		void setCameraUp( const mt::vec3f & _up );
		void setCameraRightSign( float _rightSign );

	public:
		void setCameraFOV( float _fov );
		void setCameraAspect( float _aspect );
		void setCameraNear( float _near );
		void setCameraFar( float _far );
		
	protected:
		void _updateViewMatrix() const override;
		void _updateProjectionMatrix() const override;
		
	protected:
        mt::vec3f m_cameraPosition;
		mt::vec3f m_cameraDirection;
		mt::vec3f m_cameraUp;
		float m_cameraRightSign;

		float m_cameraFov;
		float m_cameraAspect;
		float m_cameraNear;
		float m_cameraFar;		
	};
}
