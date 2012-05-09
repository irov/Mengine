#	pragma once

namespace Menge
{
	class Updatable
	{
	public:
		virtual void update( float _timing );

	protected:
		virtual void _update( float _timing ) = 0;
	};
}
