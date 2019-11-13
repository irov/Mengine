#include "Box.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box::Box( const Box2DBodyInterfacePtr & _body, const VectorizatorPtr & _vectorizator )
        : m_body( _body )
        , m_vectorizator( _vectorizator )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box::~Box()
    {
        m_body = nullptr;

        if( m_vectorizator != nullptr )
        {
            m_vectorizator->removeFromParent();
            m_vectorizator->release();

            m_vectorizator = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Box::update() const
    {
        if( m_vectorizator == nullptr || m_body == nullptr )
        {
            return;
        }

        mt::vec2f position = m_body->getPosition();
        float angle = m_body->getAngle();

        m_vectorizator->setLocalPosition( { position.x, -position.y, 0.f } );
        m_vectorizator->setLocalOrientationX( -angle );

        LOGGER_MESSAGE( "[%s] %4.2f %4.2f %4.2f"
            , m_vectorizator->getName().c_str()
            , position.x
            , position.y
            , angle 
        );
    }
}