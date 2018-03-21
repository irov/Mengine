#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
    class MatrixProxy
        : public Node
    {
        DECLARE_VISITABLE( Node )

    public:
        MatrixProxy();
        ~MatrixProxy();

    public:
        void setProxyMatrix( const mt::mat4f & _proxyMatrix );

    protected:
        void updateLocalMatrix() const override;
    };
}
