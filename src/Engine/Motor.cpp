#include "Motor.h"

#include "Kernel/Node.h"

#include <algorithm>

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
        VectorVelocities::const_iterator it_found = std::find_if( m_velocities.begin(), m_velocities.end(), [&_name]( const VelocityDesc & _desc )
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
        VectorVelocities::const_iterator it_found = std::find_if( m_velocities.begin(), m_velocities.end(), [&_name]( const VelocityDesc & _desc )
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
    void Motor::_update( const UpdateContext * _context )
    {
        if( m_node == nullptr )
        {
            return;
        }

        mt::vec3f velocity( 0.f, 0.f, 0.f );

        for( const VelocityDesc & desc : m_velocities )
        {
            velocity += desc.velocity * _context->time;
        }

        m_node->translate( velocity );
    }
}