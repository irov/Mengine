#	pragma once

#	include "Allocator2D.h"

namespace Menge
{
	class CameraBehavior;
	class Scene;

	class Camera2D
		: public Allocator2D
	{
		OBJECT_DECLARE( Camera )
	public:
		Camera2D();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _update( float _timing ) override;

	protected:
		Scene * m_renderScene;
	};
}