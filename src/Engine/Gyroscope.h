#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
	class Gyroscope
        : public Node
	{
        DECLARE_VISITABLE( Node )

    public:
        Gyroscope();
        ~Gyroscope() override;

	protected:
		void updateWorldMatrix() const override;
	};
}
