#include "Motor.h"

#include "Interface/TransformationInterface.h"

#include "Kernel/Node.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdNumeric.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Motor::Motor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Motor::~Motor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Motor::setNode( const NodePtr & _node )
    {
        m_node = _node;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Motor::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    void Motor::addVelocity( const ConstString & _name, const mt::vec3f & _velocity )
    {
        VelocityDesc desc;
        desc.name = _name;
        desc.velocity = _velocity;

        m_velocities.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Motor::hasVelocity( const ConstString & _name ) const
    {
        VectorVelocities::const_iterator it_found = StdAlgorithm::find_if( m_velocities.begin(), m_velocities.end(), [&_name]( const VelocityDesc & _desc )
        {
            return _desc.name == _name;
        } );

        if( it_found == m_velocities.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f Motor::getVelocity( const ConstString & _name ) const
    {
        VectorVelocities::const_iterator it_found = StdAlgorithm::find_if( m_velocities.begin(), m_velocities.end(), [&_name]( const VelocityDesc & _desc )
        {
            return _desc.name == _name;
        } );

        if( it_found == m_velocities.end() )
        {
            return mt::vec3f( 0.f, 0.f, 0.f );
        }

        const VelocityDesc & desc = *it_found;

        return desc.velocity;
    }
    //////////////////////////////////////////////////////////////////////////
    void Motor::update( const UpdateContext * _context )
    {
        if( m_node == nullptr )
        {
            return;
        }

        mt::vec3f velocity = StdNumeric::accumulate( m_velocities.begin(), m_velocities.end(), mt::vec3f( 0.f, 0.f, 0.f ), [_context]( const mt::vec3f & _v, const VelocityDesc & _desc )
        {
            return _v + _desc.velocity * _context->time;
        } );

        TransformationInterface * transformation = m_node->getTransformation();

        transformation->translate( velocity );
    }
    //////////////////////////////////////////////////////////////////////////
}