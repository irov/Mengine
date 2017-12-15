#	include "MatrixProxy.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MatrixProxy::MatrixProxy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MatrixProxy::~MatrixProxy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MatrixProxy::setProxyMatrix( const mt::mat4f & _proxyMatrix )
    {
        m_localMatrix = _proxyMatrix;

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void MatrixProxy::updateLocalMatrix() const
    {
        m_invalidateLocalMatrix = false;
        m_identityLocalMatrix = false;
    }
}
