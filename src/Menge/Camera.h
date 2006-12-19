#	pragma once

#	include "Allocator.h"

namespace Menge
{
	class CameraBehavior;

	class Camera
		: public Allocator
	{
		OBJECT_DECLARE( Camera )
	public:
		Camera();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _update( float _timing ) override;
	};
}