#include "NodeBox2DBody.h"
#include "Box2DScaler.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	NodeBox2DBody::NodeBox2DBody()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	NodeBox2DBody::~NodeBox2DBody()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeBox2DBody::setBox2DBody( const Box2DBodyPtr & _body )
	{
		m_body = _body;
	}
	//////////////////////////////////////////////////////////////////////////
	const Box2DBodyPtr & NodeBox2DBody::getBox2DBody() const
	{
		return m_body;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeBox2DBody::_update( float _current, float _timing )
	{
		(void)_current;
		(void)_timing;

		this->invalidateLocalMatrix();
	}
	//////////////////////////////////////////////////////////////////////////	
	void NodeBox2DBody::updateLocalMatrix() const
	{
		if( m_body == nullptr )
		{
			Node::updateLocalMatrix();

			return;
		}

		m_invalidateLocalMatrix = false;

		mt::vec2f position = m_body->GetPosition();

		float angle = m_body->GetAngle();

		mt::vec3f new_position = m_position + mt::vec3f( position, 0.f );
		mt::vec3f new_origin = m_origin;
		mt::vec3f new_scale = m_scale;
		mt::vec2f new_skew = m_skew;
		mt::vec3f new_orientation = m_orientation + mt::vec3f( angle, 0.f, 0.f );
						
		bool identityLocalMatrix = this->makeLocalMatrix_( m_localMatrix, new_position, new_origin, new_scale, new_skew, new_orientation );

		m_identityLocalMatrix = identityLocalMatrix;
	}
}