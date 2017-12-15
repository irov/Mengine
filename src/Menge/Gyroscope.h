#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Gyroscope
        : public Node
	{
        DECLARE_VISITABLE( Node )

    public:
        Gyroscope();
        ~Gyroscope();

	protected:
		void updateWorldMatrix() const override;
	};
}
