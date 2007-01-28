#	pragma once

#	include "NodeImpl.h"

namespace Menge
{
	class Camera2D;

	class CameraBehavior
		: public NodeImpl
	{
		OBJECT_DECLARE(CameraBehavior)
	
	public:
		CameraBehavior();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _update( float _timing ) override;
		void _updateParent() override;

	private:
		Camera2D * m_camera;
	};
}