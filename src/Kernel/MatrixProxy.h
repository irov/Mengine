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
        ~MatrixProxy() override;

    public:
        void setProxyMatrix( const mt::mat4f & _proxyMatrix );

    protected:
        void updateLocalMatrix() const override;

    protected:
        void calcWorldMatrix( mt::mat4f & _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec2f & _skew, const mt::vec3f& _orientation ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MatrixProxy> MatrixProxyPtr;
    //////////////////////////////////////////////////////////////////////////
}
