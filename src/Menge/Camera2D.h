#	pragma once

#	include "Allocator2D.h"
#	include "ViewPort.h"

namespace Menge
{
	class CameraBehavior;
	class Scene;
	class ViewPort;

	class Camera2D
		: public Allocator2D
	{
		OBJECT_DECLARE( Camera2D )
	public:
		Camera2D();

	public:
		const ViewPort & getViewPort() const;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _update( float _timing ) override;

	protected:
		Scene * m_renderScene;

		ViewPort m_viewPort;
	};
}