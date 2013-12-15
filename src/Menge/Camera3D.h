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
		void setCameraRightSign( float _rightSign );

	//public:
	//	void setRenderport( const Viewport & _renderport );
	//	const Viewport & getRenderport() const;

	public:		
		const mt::mat4f & getProjectionMatrix() const override;
		const mt::mat4f & getViewMatrix() const override;

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

		float m_cameraFOV;
		float m_cameraAspect;
		float m_cameraRightSign;

		//Viewport m_renderport;

		Observer * m_notifyChangeWindowResolution;

		mutable mt::mat4f m_viewMatrixWM;
		mutable mt::mat4f m_projectionMatrixWM;
		
		mutable bool m_invalidateMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void Camera3D::invalidateMatrix_()
	{
		m_invalidateMatrix = true;
	}
}
