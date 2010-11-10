#	include "RigidBody2D.h"
#	include "Camera2D.h"

#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"
#	include "BinParser.h"

#	include "Math/vec2.h"
#	include "Math/angle.h"

#	include "Application.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::RigidBody2D()
		: m_interface( NULL )
		, m_density( 0.0f )
		, m_friction( 1.0f )
		, m_restitution( 0.0f )
		, m_force( 0.0f, 0.0f )
		, m_forcePoint( 0.0f, 0.0f )
		, m_constantForce( false )
		, m_directionForce( false )
		, m_linearDamping( 0.0f )
		, m_angularDamping( 0.0f )
		, m_allowSleep( true )
		, m_isBullet( false )
		, m_fixedRotation( false )
		, m_collisionMask( 0xFFFF )
		, m_categoryBits( 1 )
		, m_groupIndex( 0 )
		, m_isSensor( false )
		, m_linearVelocity( false )
		, m_countGravity( true )
		, m_unsetLinearVelocity( false )
		, m_stabilityAngle( 0.0f )
		, m_stabilization( false )
		, m_stabilityForce( 1.0f )
		, m_shapeMaterial( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::~RigidBody2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onCollide( PhysicBody2DInterface * _otherObj, float _worldX, float _worldY, float _normalX, float _normalY )
	{
		RigidBody2D * other = static_cast<RigidBody2D*>( _otherObj->getUserData() );
		callEvent( EVENT_COLLIDE, "(OOffff)", this->getEmbed(), other->getEmbed(), _worldX, _worldY, _normalX, _normalY );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loader( BinParser * _parser )
	{
		Node::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Shape )
			{
				m_shapeList.push_back( mt::polygon() );
				mt::polygon & n = m_shapeList.back();
				BIN_PARSE_METHOD_ARG1( this, &RigidBody2D::loaderShape_, n );
			}
			BIN_CASE_NODE( Protocol::ShapeCircle )
			{
				m_shapeCircleList.push_back( TShapeCircleList::value_type() );
				TShapeCircleList::value_type & n = m_shapeCircleList.back();
				mt::ident_v2( n.second );
				BIN_PARSE_METHOD_ARG1( this, &RigidBody2D::loaderShapeCircle_, n );
			}
			BIN_CASE_NODE( Protocol::ShapeBox )
			{
				m_shapeBoxList.push_back( TShapeBoxList::value_type() );
				TShapeBoxList::value_type & n = m_shapeBoxList.back();
				mt::ident_v2( n.second.first );
				BIN_PARSE_METHOD_ARG1( this, &RigidBody2D::loaderShapeBox_, n );
			}

			BIN_CASE_ATTRIBUTE( Protocol::Density_Value, m_density );
			BIN_CASE_ATTRIBUTE( Protocol::Friction_Value, m_friction );
			BIN_CASE_ATTRIBUTE( Protocol::Restitution_Value, m_restitution );
			BIN_CASE_ATTRIBUTE( Protocol::CollisionMask_Value, m_collisionMask );
			BIN_CASE_ATTRIBUTE( Protocol::CategoryBits_Value, m_categoryBits );
			BIN_CASE_ATTRIBUTE( Protocol::GroupIndex_Value, m_groupIndex );
			BIN_CASE_ATTRIBUTE( Protocol::LinearDamping_Value, m_linearDamping );
			BIN_CASE_ATTRIBUTE( Protocol::AngularDamping_Value, m_angularDamping );
			BIN_CASE_ATTRIBUTE( Protocol::AllowSleep_Value, m_allowSleep );
			BIN_CASE_ATTRIBUTE( Protocol::IsBullet_Value, m_isBullet );
			BIN_CASE_ATTRIBUTE( Protocol::FixedRotation_Value, m_fixedRotation );
			BIN_CASE_ATTRIBUTE( Protocol::IsSensor_Value, m_isSensor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loaderShape_( BinParser * _parser, mt::polygon & _shape )
	{
		BIN_SWITCH_ID( _parser )
		{			
			BIN_CASE_NODE( Protocol::Point )
			{
				mt::vec2f point;
				BIN_FOR_EACH_ATTRIBUTES()
				{					
					BIN_CASE_ATTRIBUTE( Protocol::Point_Value, point );
				}

				_shape.add_point( point );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loaderShapeCircle_( BinParser * _parser, TShapeCircleList::value_type & _circle )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Radius_Value, _circle.first );
			BIN_CASE_ATTRIBUTE( Protocol::Position_Value, _circle.second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loaderShapeBox_( BinParser * _parser, TShapeBoxList::value_type & _box )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Width_Value, _box.first.first );
			BIN_CASE_ATTRIBUTE( Protocol::Height_Value, _box.first.second );
			BIN_CASE_ATTRIBUTE( Protocol::Position_Value, _box.second.first );
			BIN_CASE_ATTRIBUTE( Protocol::Angle_Value, _box.second.second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_update( float _timing )
	{
		if( !(m_interface->isFrozen() || m_interface->isStatic() || m_interface->isSleeping()) )
		{
			const float phase = PhysicEngine2D::get()->getPhase();

			const float * pos = m_interface->getPosition();
			const mt::vec2f& prevPos = getLocalPosition();
			mt::vec2f currPos( pos[0], pos[1] );
			//currPos = currPos * phase + prevPos * ( 1.0f - phase );
			setLocalPosition( currPos );

			const float * orient = m_interface->getOrientation();
			setLocalDirection( mt::vec2f( orient[0], orient[1] ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody2D::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		m_shapeMaterial = RenderEngine::get()->createMaterial();

		if( m_shapeMaterial != NULL )
		{
			m_shapeMaterial->isSolidColor = false;
			m_shapeMaterial->blendSrc = BF_SOURCE_ALPHA;
			m_shapeMaterial->blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

			m_shapeMaterial->textureStage[0].colorOp = TOP_SELECTARG2;
			m_shapeMaterial->textureStage[0].alphaOp = TOP_SELECTARG2;
		}
		//ApplyColor2D applyColor( 0xFF00CCFF );


		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_deactivate()
	{
		RenderEngine::get()->releaseMaterial( m_shapeMaterial );

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody2D::_compile()
	{
		const mt::vec2f & position = getWorldPosition();

		m_interface = PhysicEngine2D::get()->createBody( position, 0.0f, m_linearDamping, m_angularDamping, m_allowSleep,
																			m_isBullet, m_fixedRotation );

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR( "RigidBody2D: Error while compiling. can't create Body"
				);

			return false;
		}


		m_interface->setBodyListener( this );
		m_interface->setUserData( this );

		compileShapes_();

		for( TShapeList::iterator 
			it = m_shapeList.begin(),
			it_end = m_shapeList.end();
		it != it_end;
		it++ )
		{
			std::size_t numPoint = it->num_points();

			const mt::TVectorPoints & vectorPoints = it->get_points();

			m_interface->addShapeConvex( 
				numPoint, 
				vectorPoints.front().buff(), 
				m_density, 
				m_friction, 
				m_restitution, 
				m_isSensor,
				m_collisionMask, 
				m_categoryBits, 
				m_groupIndex );

			addShapeData_( vectorPoints );
		}

		for( TShapeCircleList::iterator 
			it = m_shapeCircleList.begin(),
			it_end = m_shapeCircleList.end();
		it != it_end;
		it++ )
		{
			m_interface->addShapeCircle( it->first, it->second.buff(), m_density, m_friction, m_restitution, m_isSensor,
											m_collisionMask, m_categoryBits, m_groupIndex );
		}

		for( TShapeBoxList::iterator it = m_shapeBoxList.begin(),
			it_end = m_shapeBoxList.end();
			it != it_end;
			it++ )
		{
			m_interface->addShapeBox( it->first.first, it->first.second, it->second.first.buff(), it->second.second, m_density, m_friction, m_restitution, m_isSensor,
										m_collisionMask, m_categoryBits, m_groupIndex );

			addShapeBoxData_( it->first.first, it->first.second
							, it->second.first, it->second.second );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_release()
	{
		PhysicEngine2D::get()->destroyPhysicBody( m_interface );
		m_interface = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::applyForce( float _forceX, float _forceY, float _pointX, float _pointY )
	{
		const mt::vec2f & position = getWorldPosition();

		m_interface->applyForce( _forceX, _forceY, position.x + _pointX, position.y + _pointY );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY )
	{
		const mt::vec2f & position = getWorldPosition();

		m_interface->applyImpulse( _impulseX, _impulseY, position.x + _pointX, position.y + _pointY );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::applyConstantForce( float _forceX, float _forceY, float _pointX, float _pointY )
	{
		m_force.x = _forceX;
		m_force.y = _forceY;
		m_forcePoint.x = _pointX;
		m_forcePoint.y = _pointY;
		m_constantForce = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::removeConstantForce()
	{
		m_constantForce = false;
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBody2DInterface * RigidBody2D::getInterface()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setDirectionForce( bool _relative )
	{
		m_directionForce = _relative;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::wakeUp()
	{
		if( m_interface )
		{
			m_interface->wakeUp();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float RigidBody2D::getMass() const
	{
		return m_interface->getMass();
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f RigidBody2D::getLinearVelocity() const
	{
		const float * v = m_interface->getLinearVelocity();
		return mt::vec2f( v[0], v[1] );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setLinearVelocity( float _x, float _y, bool _countGravity )
	{
		m_constantForce = false;
		m_linearVelocity = true;
		m_velocity.x = _x;
		m_velocity.y = _y;
		m_countGravity = _countGravity;

		if( m_interface )
		{
			m_interface->wakeUp();
		}
		//m_interface->setLinearVelocity( _x, _y );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::addShapeBox( float _width, float _heigth, const mt::vec2f& _pos, float _angle )
	{
		m_shapeBoxList.push_back( std::make_pair( std::make_pair( _width, _heigth ), std::make_pair( _pos, _angle ) ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::addShapeConvex( const mt::TVectorPoints & _points, bool _isSensor )
	{
		/*m_interface->addShapeConvex( 
		_points.size(), 
		_points.front().m, 
		m_density, 
		m_friction, 
		m_restitution, 
		_isSensor,
		m_collisionMask, 
		m_categoryBits, 
		m_groupIndex );*/
		mt::polygon poly;
		for( mt::TVectorPoints::size_type i = 0; i < _points.size(); i++ )
		{
			poly.add_point( _points[i] );
		}
		m_shapeList.push_back( poly );
		m_isSensor = _isSensor;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_setListener( PyObject * _listener )
	{
		Node::_setListener( _listener );
		
		Eventable::registerEvent( EVENT_COLLIDE, ("onCollide"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onApplyForceAndTorque_()
	{
		if( m_constantForce )
		{
			//const mt::vec2f & position = getWorldPosition();
			mt::vec2f force;
			mt::vec2f point;

			if( m_directionForce )
			{
				mt::mul_v2_m3_r( force, m_force, getWorldMatrix() );
			}
			else
			{
				force = m_force;
			}

			//force *= /*m_interface->getMass() **/ _timing;

			mt::mul_v2_m3( point, m_forcePoint, getWorldMatrix() );

			m_interface->applyForce( force.x, force.y, point.x, point.y );
		}
		else if( m_linearVelocity )
		{
			const float* v = m_interface->getLinearVelocity();
			mt::vec2f cv( v[0], v[1] );
			mt::vec2f force = ( m_velocity - cv ) * m_interface->getMass() * 60.0f;
			if( m_countGravity )
			{
				force -= PhysicEngine2D::get()->getGravity() * m_interface->getMass();
			}
			const float* pos = m_interface->getPosition();
			m_interface->applyForce( force.x, force.y, pos[0], pos[1] );
			if( m_unsetLinearVelocity )
			{
				m_linearVelocity = false;
				m_unsetLinearVelocity = false;
			}
		}
		if( m_stabilization )
		{
			/*float dif = m_interface->getAngle() - m_stabilityAngle;
			if( fabsf( dif ) > 0.1f )
			{
			m_interface->applyTorque( -dif * fabsf( dif ) * m_stabilityForce );
			}*/
			float f = ( m_stabilityAngle - m_interface->getAngle() ) * 10.0f;//  * 0.0005f;
			float angvel = m_interface->getAngularVelocity();
			float torque = ( f - m_interface->getAngularVelocity() ) * m_interface->getInertia() * 1.0f;
			m_interface->applyTorque( torque );
		}

	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_debugRender( Camera2D * _camera, unsigned int _debugMask )
	{
		if( _debugMask & MENGE_DEBUG_PHYSICS )
		{
			for( TVectorShapeData::iterator it = m_shapeData.begin(), it_end = m_shapeData.end();
				it != it_end;
				++it )
			{
				TVertex2DVector& shapeData = (*it);
				RenderEngine::get()
					->renderObject2D( m_shapeMaterial, NULL, 0
					, &(shapeData[0]), shapeData.size(), LPT_LINE );
			}
		}
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::unsetLinearVelocity()
	{
		if( m_linearVelocity )
		{
			m_unsetLinearVelocity = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_freeze( bool _value )
	{
		if( _value == true )	// completly remove from simulation
		{
			_release();
		}
		else
		{
			_compile();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setCollisionMask( int _mask )
	{
		m_collisionMask = (uint16)_mask;
		updateFilterData_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::updateFilterData_()
	{
		m_interface->updateFilterData( m_categoryBits, m_collisionMask, m_groupIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onUpdate()
	{
		onApplyForceAndTorque_();

		const float * pos = m_interface->getPosition();
		mt::vec2f currPos( pos[0], pos[1] );
		setLocalPosition( currPos );

		const float * orient = m_interface->getOrientation();
		setLocalDirection( mt::vec2f( orient[0], orient[1] ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::compileShapes_()
	{
		/*for( int i = 0; i < m_shapeList.size(); )
		{
			if( m_shapeList[i].num_points() == 4 )
			{
				const mt::TVectorPoints& pts = m_shapeList[i].get_points();
				if( pts[0].x == pts[3].x &&
					pts[0].y == pts[1].y &&
					pts[1].x == pts[2].x &&
					pts[2].y == pts[3].y )
				{
					float w = pts[1].x - pts[0].x;
					float h = pts[2].y - pts[1].y;
					mt::vec2f c( w * 0.5f, h * 0.5f );
					//mt::vec2f c( 0.0f, 0.0f );
					m_shapeBoxList.push_back( std::make_pair( std::make_pair( w, h ), std::make_pair( c, 0.0f ) ) );
					m_shapeList.erase( std::remove( m_shapeList.begin(), m_shapeList.end(), m_shapeList[i] ) );
					continue;
				}
			}
			i++;
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

		const mt::mat3f& wm = getWorldMatrix();
		for( TShapeList::iterator it = m_shapeList.begin(),
			it_end = m_shapeList.end();
			it != it_end;
		it++ )
		{
			for( std::size_t 
				it_point = 0,
				it_point_end = (*it).num_points(); 
			it_point != it_point_end;
			++it_point )
			{
				mt::vec2f p;
				mt::mul_v2_m3( p, (*it)[it_point], wm );
				mt::add_internal_point( _boundingBox, p );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::enableStabilization( bool _enable, float _stabilityAngle, float _stabilityForce )
	{
		m_stabilization = _enable;
		m_stabilityAngle = _stabilityAngle;
		m_stabilityForce = _stabilityForce;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::addShapeData_( const mt::TVectorPoints& _points )
	{
		if( _points.empty() == true )
		{
			return;
		}

		m_shapeData.push_back( TVertex2DVector( _points.size() + 1 ) );
		TVertex2DVector& shapeData = m_shapeData.back();
		TVertex2DVector::iterator shapeData_it = shapeData.begin();
		for( mt::TVectorPoints::const_iterator it = _points.begin(), it_end = _points.end();
			it != it_end;
			++it )
		{
			Vertex2D& vtx = (*shapeData_it);
			vtx.pos[0] = it->x;
			vtx.pos[1] = it->y;
			vtx.color = 0xFF00CCFF;
			++shapeData_it;
		}
		Vertex2D& vtx = (*shapeData_it);
		vtx.pos[0] = _points[0].x;
		vtx.pos[1] = _points[0].x;
		vtx.color = 0xFF00CCFF;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::addShapeBoxData_( float _width, float _heigth, const mt::vec2f& _position, float _angle )
	{
		// TODO
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::addShapeCircleData_( float _radius, const mt::vec2f& _position )
	{
		// TODO
	}
	//////////////////////////////////////////////////////////////////////////
}
