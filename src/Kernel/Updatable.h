#pragma once

namespace Mengine
{
	class Updatable
	{
	public:
		virtual void update( float _current, float _timing );

	protected:
		virtual void _update( float _current, float _timing ) = 0;
	};
}
