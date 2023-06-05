#include "MatrixProxy.h"

namespace Mengine
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
        m_transformationFlag = TRANSFORMATION_INVALIDATE_ALL;

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void MatrixProxy::updateLocalMatrix() const
    {
        m_invalidateLocalMatrix = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MatrixProxy::calcWorldMatrix( mt::mat4f * const _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) const
    {
        MENGINE_UNUSED( _transformationFlag );
        MENGINE_UNUSED( _position );
        MENGINE_UNUSED( _origin );
        MENGINE_UNUSED( _scale );
        MENGINE_UNUSED( _skew );
        MENGINE_UNUSED( _orientation );

        if( m_relationTransformation == nullptr )
        {
            *_wm = m_localMatrix;
        }
        else
        {
            bool identityWorldMatrix = m_relationTransformation->isIdentityWorldMatrix();

            if( identityWorldMatrix == false )
            {
                const mt::mat4f & relationMatrix = m_relationTransformation->getWorldMatrix();

                mt::mul_m4_m4( _wm, m_localMatrix, relationMatrix );
            }
            else
            {
                *_wm = m_localMatrix;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
