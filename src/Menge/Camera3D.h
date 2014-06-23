#	pragma once

#	include "Camera.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Observer;

	class Camera3D
        : public Node
		, public Camera		
	{
	public:
		Camera3D();

	protected:
		bool _activate() override;
		void _deactivate() override;
		
	public:
        void setCameraPosition( const mt::vec3f & _pos );
		void setCameraDir( const mt::vec3f & _dir );
		void setCameraUp( const mt::vec3f & _up );
		void setCameraFOV( float _fov );
		void setCameraAspect( float _aspect );
		void setCameraNear( float _near );
		void setCameraFar( float _far );
		void setCameraRightSign( float _rightSign );

	public:
		void setRenderport( const Viewport & _renderport );

	public:
		const Viewport & getCameraRenderport() const override;

	public:		
		const mt::mat4f & getCameraWorldMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraViewMatrix() const override;
		const mt::mat4f & getCameraViewMatrixInv() const override;

	public:
		bool isOrthogonalProjection() const override;

	protected:
		void _invalidateWorldMatrix() override;

	protected:
		void invalidateMatrix_();

	protected:
		void updateMatrix_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, Resolution _resolution );
		
	protected:
        mt::vec3f m_cameraPosition;
		mt::vec3f m_cameraDir;
		mt::vec3f m_cameraUp;

		float m_cameraFov;
		float m_cameraAspect;
		float m_cameraNear;
		float m_cameraFar;
		float m_cameraRightSign;

		Viewport m_renderport;

		Observer * m_notifyChangeWindowResolution;

		mt::mat4f m_worldMatrix;

		mutable mt::mat4f m_viewMatrix;
		mutable mt::mat4f m_viewMatrixInv;
		mutable mt::mat4f m_projectionMatrix;
		
		mutable bool m_invalidateMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void Camera3D::invalidateMatrix_()
	{
		m_invalidateMatrix = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const Viewport & Camera3D::getCameraRenderport() const
	{
		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraWorldMatrix() const
	{
		return m_worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraProjectionMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraViewMatrix() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & Camera3D::getCameraViewMatrixInv() const
	{
		if( m_invalidateMatrix == true )
		{
			this->updateMatrix_();
		}

		return m_viewMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Camera3D::isOrthogonalProjection() const
	{
		return false;
	}
}
