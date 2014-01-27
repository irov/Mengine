#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Camera2D;

	class CameraTarget2D
		: public Node
	{
	public:
		CameraTarget2D();
		~CameraTarget2D();

	public:
		void setCamera2D( RenderCameraInterface * _camera );
		RenderCameraInterface * getCamera2D() const;

	public:
		void setSpeed( float _speed );
		float getSpeed() const;

	public:
		void setFixedHorizont( float _horizont );
		float getFixedHorizont() const;
		bool isFixedHorizont() const;

	public:
		void _update( float _current, float _timing ) override;

	protected:
		Camera2D * m_camera;
		float m_speed;

		bool m_fixedHorizont;
		float m_horizont;
	};	
}
