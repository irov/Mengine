#pragma once

#include "Kernel/Node.h"
#include "Kernel/DummyRender.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MatrixProxy
        : public Node
        , protected DummyRender
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        MatrixProxy();
        ~MatrixProxy() override;

    public:
        void setProxyMatrix( const mt::mat4f & _proxyMatrix );

    protected:
        void updateLocalMatrix() const override;

    protected:
        void calcWorldMatrix( mt::mat4f * const _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<MatrixProxy> MatrixProxyPtr;
    //////////////////////////////////////////////////////////////////////////
}
