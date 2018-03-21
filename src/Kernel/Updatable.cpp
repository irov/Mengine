#include "Kernel/Updatable.h"

namespace Mengine
{
	void Updatable::update( float _current, float _timing )
	{
		this->_update( _current, _timing );
	}
}