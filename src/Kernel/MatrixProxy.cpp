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
    void MatrixProxy::calcWorldMatrix( mt::mat4f & _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f& _origin, const mt::vec3f& _scale, const mt::vec2f & _skew, const mt::vec3f& _orientation ) const
    {
        (void)_transformationFlag;
        (void)_position;
        (void)_origin;
        (void)_scale;
        (void)_skew;
        (void)_orientation;

        if( m_relationTransformation == nullptr )
        {
            _wm = m_localMatrix;
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
                _wm = m_localMatrix;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MatrixProxy::_render( const RenderContext * _context )
    {
        (void)_context;

        //Empty
    }
}
