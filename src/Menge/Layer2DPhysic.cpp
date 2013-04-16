#	include "Layer2DPhysic.h"

#   include "Interface/PhysicSystem2DInterface.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	Layer2DPhysic::Layer2DPhysic()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DPhysic::_compile()
	{
		if( Layer2D::_compile() == false )
		{
			return false;
		}

		mt::vec2f gravity( 0.0, -10.0f  );

		mt::vec2f lowerBound( m_position.x , m_position.y );
		mt::vec2f upperBound( m_position.x + m_size.x, m_position.y + m_size.y );
		bool canSleep = true;

		PHYSIC2D_SERVICE(m_serviceProvider)
			->createWorld( lowerBound, upperBound , gravity, canSleep );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DPhysic::_release()
	{
		//Layer2D::_release();
		PHYSIC2D_SERVICE(m_serviceProvider)
            ->destroyWorld();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DPhysic::_removeChildren(Node * _node)
	{
		//Layer2D::_release();
		_node->release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DPhysic::_update( float _current, float _timing )
	{
		// update physics first
		PHYSIC2D_SERVICE(m_serviceProvider)
            ->update( _timing );

		Node::_update( _current, _timing );
	}
	////////////////////////////////////////////////////////////////////////////
}	// namespace Menge
