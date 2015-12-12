#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Camera2D.h"

namespace Menge
{
	class BurritoCameraTarget
		: public Node
	{
	public:
		BurritoCameraTarget();
		~BurritoCameraTarget();

	public:
		void setCamera2D( Camera2D * _camera );
		Camera2D * getCamera2D() const;

	public:
		void setSpeed( float _speed );
		float getSpeed() const;

	public:
		void setMaxScale( float _scale );
		float getMaxScale() const;

	public:
		void setRenderport( const Viewport & _renderport );
		const Viewport & getRenderport() const;

	public:
		void setHorizont( float _horizont );
		float getHorizont() const;

	public:
		void _invalidateWorldMatrix() override;

	protected:
		Camera2D * m_camera;
		float m_speed;
		float m_maxScale;
		Viewport m_renderport;

		float m_horizont;
	};	
}
