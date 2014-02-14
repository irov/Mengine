#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Gyroscope
        : public Node
	{
	protected:
		void updateWorldMatrix() const override;
	};
}
