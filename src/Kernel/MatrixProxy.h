#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    class MatrixProxy
        : public Node
        , public BaseRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        MatrixProxy();
        ~MatrixProxy() override;

    public:
        void setProxyMatrix( const mt::mat4f & _proxyMatrix );

    protected:
        void updateLocalMatrix() const override;

    protected:
        void calcWorldMatrix( mt::mat4f & _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec2f & _skew, const mt::vec3f& _orientation ) const override;

    protected:
        void _render( const RenderContext * _context ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MatrixProxy> MatrixProxyPtr;
    //////////////////////////////////////////////////////////////////////////
}
