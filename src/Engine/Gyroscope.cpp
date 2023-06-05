#include "Gyroscope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Gyroscope::Gyroscope()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Gyroscope::~Gyroscope()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Gyroscope::updateWorldMatrix() const
    {
        m_invalidateWorldMatrix = false;

        const mt::mat4f & localMatrix = this->getLocalMatrix();

        if( m_relationTransformation == nullptr )
        {
            m_worldMatrix = localMatrix;
        }
        else
        {
            mt::vec3f relationPosition = m_relationTransformation->getWorldPosition();

            mt::mat4f relationMatrix;
            mt::make_translation_m4_v3( &relationMatrix, relationPosition );

            mt::mul_m4_m4_r( &m_worldMatrix, localMatrix, relationMatrix );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
