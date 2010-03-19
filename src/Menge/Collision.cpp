/*
 *	Collision.cpp
 *
 *	Created by _Berserk_ on 17.3.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#	include "Collision.h"

#	include "Logger/Logger.h"
#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"
#	include "Utils/Math/angle.h"
#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Collision::Collision()
		: m_physicBodyInterface( NULL )
		, m_collisionMask( 0xFFFF )
		, m_categoryBits( 1 )
		, m_groupIndex( 0 )
		, m_invalidateTransformation( true )
		, m_shapeMaterial( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Collision::~Collision()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::onCollide( PhysicBody2DInterface * _otherObj
		, float _worldX, float _worldY
		, float _normalX, float _normalY )
	{
		Collision* other = static_cast<Collision*>( _otherObj->getUserData() );
		if( isActivate() == true && other->isActivate() == true )
		{
			TVectorCollisionInfo::iterator it_find = 
				std::find_if( m_collisions.begin(), m_collisions.end(), CollisionInfoFind( other ) );
			if( it_find == m_collisions.end() )
			{
				CollisionInfo collisionInfo = { other, true };
				m_collisions.push_back( collisionInfo );
				callEvent( EVENT_COLLIDE_BEGIN, "(OO)", this->getEmbedding(), other->getEmbedding() );
			}
			else
			{
				it_find->active = true;
			}
			callEvent( EVENT_COLLIDE, "(OOffff)", this->getEmbedding(), other->getEmbedding(), _worldX, _worldY, _normalX, _normalY );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::onUpdate()
	{
		// ignore udpate from physics
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::loader( XmlElement* _xml )
	{
		Node::loader( _xml );
		
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Shape" )
			{
				m_shapesPoly.push_back( ShapePoly() );
				ShapePoly& shapePoly = m_shapesPoly.back();
				XML_PARSE_ELEMENT_ARG1( this, &Collision::loaderShapePoly_, shapePoly );

			}
			XML_CASE_NODE( "ShapeCircle" )
			{
				m_shapesCircle.push_back( ShapeCircle() );
				ShapeCircle& shapeCircle = m_shapesCircle.back();
				mt::ident_v2( shapeCircle.position );
				shapeCircle.radius = 0.0f;
				XML_PARSE_ELEMENT_ARG1( this, &Collision::loaderShapeCircle_, shapeCircle );
			}
			XML_CASE_NODE( "ShapeBox" )
			{
				m_shapesBox.push_back( ShapeBox() );
				ShapeBox& shapeBox = m_shapesBox.back();
				mt::ident_v2( shapeBox.position );
				shapeBox.width = 0.0f;
				shapeBox.height = 0.0f;
				shapeBox.angle = 0.0f;
				XML_PARSE_ELEMENT_ARG1( this, &Collision::loaderShapeBox_, shapeBox );
			}
			XML_CASE_ATTRIBUTE_NODE( "CollisionMask", "Value", m_collisionMask );
			XML_CASE_ATTRIBUTE_NODE( "CategoryBits", "Value", m_categoryBits );
			XML_CASE_ATTRIBUTE_NODE( "GroupIndex", "Value", m_groupIndex );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::loaderShapePoly_( XmlElement* _xml, ShapePoly& _shapePoly )
	{
		XML_SWITCH_NODE( _xml )
		{			
			XML_CASE_NODE( "Point" )
			{
				mt::vec2f point;
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", point );
				}

				_shapePoly.points.push_back( point );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::loaderShapeCircle_( XmlElement* _xml, ShapeCircle& _shapeCircle )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Radius", "Value", _shapeCircle.radius );
			XML_CASE_ATTRIBUTE_NODE( "Position", "Value", _shapeCircle.position );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::loaderShapeBox_( XmlElement* _xml, ShapeBox& _shapeBox )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Width", "Value", _shapeBox.width );
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", _shapeBox.height );
			XML_CASE_ATTRIBUTE_NODE( "Position", "Value", _shapeBox.position );
			XML_CASE_ATTRIBUTE_NODE( "Angle", "Value", _shapeBox.angle );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Collision::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		const mt::vec2f & position = getWorldPosition();

		m_physicBodyInterface = PhysicEngine2D::hostage()
									->createBody( position, 0.0f, 0.0f, 0.0f, false, false, false );

		if( m_physicBodyInterface == 0 )
		{
			MENGE_LOG_ERROR( "Collision: Error while compiling. can't create Body"	);
			return false;
		}


		m_physicBodyInterface->setBodyListener( this );
		m_physicBodyInterface->setUserData( this );

		for( TVectorShapePoly::iterator it = m_shapesPoly.begin(), it_end = m_shapesPoly.end();
			it != it_end; ++it )
		{
			ShapePoly& shapePoly = (*it);
			if( shapePoly.points.empty() == true )
			{
				continue;
			}
			m_physicBodyInterface->addShapeConvex( shapePoly.points.size(), shapePoly.points[0].buff()
				, 1.0f, 0.0f, 0.0f, true
				, static_cast<unsigned short>( m_collisionMask )
				, static_cast<unsigned short>( m_categoryBits )
				, static_cast<unsigned short>( m_groupIndex ) );
			addShapePolyData_( shapePoly );
		}

		for( TVectorShapeBox::iterator it = m_shapesBox.begin(), it_end = m_shapesBox.end();
			it != it_end;
			++it )
		{
			ShapeBox& shapeBox = (*it);
			m_physicBodyInterface->addShapeBox( shapeBox.width, shapeBox.height
				, shapeBox.position.buff(), shapeBox.angle
				, 1.0f, 0.0f, 0.0f, true
				, static_cast<unsigned short>( m_collisionMask )
				, static_cast<unsigned short>( m_categoryBits )
				, static_cast<unsigned short>( m_groupIndex ) );
			addShapeBoxData_( shapeBox );
		}

		for( TVectorShapeCircle::iterator it = m_shapesCircle.begin(), it_end = m_shapesCircle.end();
			it != it_end;
			++it )
		{
			ShapeCircle& shapeCircle = (*it);
			m_physicBodyInterface->addShapeCircle( shapeCircle.radius, shapeCircle.position.buff()
				, 1.0f, 0.0f, 0.0f, true
				, static_cast<unsigned short>( m_collisionMask )
				, static_cast<unsigned short>( m_categoryBits )
				, static_cast<unsigned short>( m_groupIndex ) );
			addShapeCircleData_( shapeCircle );
		}

		m_physicBodyInterface->sleep();

		m_shapeDataRender = m_shapeData;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::_release()
	{
		PhysicEngine2D::hostage()->destroyPhysicBody( m_physicBodyInterface );
		m_physicBodyInterface = NULL;
		Node::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::_debugRender( Camera2D* _camera, unsigned int _debugMask )
	{
		if( _debugMask & MENGE_DEBUG_PHYSICS )
		{
			if( m_invalidateShapeData == true )
			{
				updateShapeData_();
				m_invalidateShapeData = false;
			}

			for( TVectorShapeData::iterator it = m_shapeDataRender.begin(), it_end = m_shapeDataRender.end();
				it != it_end;
				++it )
			{
				TVertex2DVector& shapeData = (*it);
				RenderEngine::hostage()
					->renderObject2D( m_shapeMaterial, NULL, 0
					, &(shapeData[0]), shapeData.size(), LPT_LINE );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		m_invalidateTransformation = true;
		m_invalidateShapeData = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::_update( float _timing )
	{
		Node::_update( _timing );

		if( m_invalidateTransformation == true )
		{
			updateTransformation_();
			m_invalidateTransformation = false;
		}

		for( TVectorCollisionInfo::iterator it = m_collisions.begin(), it_end = m_collisions.end();
			it != it_end;
			++it )
		{
			CollisionInfo& ci = (*it);
			if( ci.active == false )
			{
				callEvent( EVENT_COLLIDE_END, "(OO)", this->getEmbedding(), ci.collision->getEmbedding() );
			}
		}
		TVectorCollisionInfo::iterator it_remove = 
			std::remove_if( m_collisions.begin(), m_collisions.end(), RemoveInactiveCollision() );
		m_collisions.erase( it_remove, m_collisions.end() );

		for( TVectorCollisionInfo::iterator it = m_collisions.begin(), it_end = m_collisions.end();
			it != it_end;
			++it )
		{
			it->active = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Collision::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		m_shapeMaterial = RenderEngine::hostage()->createMaterial();

		if( m_shapeMaterial != NULL )
		{
			m_shapeMaterial->isSolidColor = false;
			m_shapeMaterial->blendSrc = BF_SOURCE_ALPHA;
			m_shapeMaterial->blendDst = BF_ONE_MINUS_SOURCE_ALPHA;

			m_shapeMaterial->textureStage[0].colorOp = TOP_SELECTARG2;
			m_shapeMaterial->textureStage[0].alphaOp = TOP_SELECTARG2;
		}

		if( m_invalidateTransformation == true )
		{
			updateTransformation_();
			m_invalidateTransformation = false;
		}
		m_physicBodyInterface->wakeUp();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::_deactivate()
	{
		if( m_physicBodyInterface != NULL )
		{
			m_physicBodyInterface->sleep();
		}

		for( TVectorCollisionInfo::iterator it = m_collisions.begin(), it_end = m_collisions.end();
			it != it_end;
			++it )
		{
			CollisionInfo& ci = (*it);
			callEvent( EVENT_COLLIDE_END, "(OO)", this->getEmbedding(), ci.collision->getEmbedding() );
		}
		m_collisions.clear();

		RenderEngine::hostage()->releaseMaterial( m_shapeMaterial );
		m_shapeMaterial = NULL;

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::_setListener()
	{
		Node::_setListener();

		Eventable::registerEvent( EVENT_COLLIDE_BEGIN, ("onCollideBegin"), m_listener );
		Eventable::registerEvent( EVENT_COLLIDE, ("onCollide"), m_listener );
		Eventable::registerEvent( EVENT_COLLIDE_END, ("onCollideEnd"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::updateTransformation_()
	{
		if( m_physicBodyInterface != NULL )
		{
			const mt::mat3f& wm = getWorldMatrix();
			m_physicBodyInterface->setPosition( wm.v2.x, wm.v2.y );
			float angle = mt::signed_angle( wm.v0.to_vec2f() );
			m_physicBodyInterface->setOrientation( angle );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::updateShapeData_()
	{
		const mt::mat3f& wm = getWorldMatrix();
		TVectorShapeData::iterator it_r = m_shapeDataRender.begin();
		for( TVectorShapeData::iterator it = m_shapeData.begin(), it_end = m_shapeData.end();
			it != it_end;
			++it, ++it_r )
		{
			TVertex2DVector& vertices = (*it);
			TVertex2DVector::iterator v_it_r = it_r->begin();
			for( TVertex2DVector::iterator v_it = vertices.begin(), v_it_end = vertices.end();
				v_it != v_it_end;
				++v_it, ++v_it_r )
			{
				Vertex2D& vtx = (*v_it);
				Vertex2D& vtx_r = (*v_it_r);
				mt::vec2f out;
				mt::mul_v2_m3( out, mt::vec2f( vtx.pos[0], vtx.pos[1] ), wm );
				vtx_r.pos[0] = out.x;
				vtx_r.pos[1] = out.y;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::addShapePolyData_( const ShapePoly& _shapePoly )
	{
		if( _shapePoly.points.empty() == true )
		{
			return;
		}

		m_shapeData.push_back( TVertex2DVector( _shapePoly.points.size() + 1 ) );
		TVertex2DVector& shapeData = m_shapeData.back();
		TVertex2DVector::iterator shapeData_it = shapeData.begin();
		for( std::vector<mt::vec2f>::const_iterator it = _shapePoly.points.begin()
			, it_end = _shapePoly.points.end();
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
		vtx.pos[0] = _shapePoly.points[0].x;
		vtx.pos[1] = _shapePoly.points[0].y;
		vtx.color = 0xFF00CCFF;	
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::addShapeBoxData_( const ShapeBox& _shapeBox )
	{
		ShapePoly shapePoly;
		shapePoly.points.resize( 4 );
		float sin_a = ::sinf( _shapeBox.angle );
		float cos_a = ::cosf( _shapeBox.angle );
		mt::vec2f half_widht( _shapeBox.width * 0.5f * cos_a, _shapeBox.width* 0.5f * sin_a );
		mt::vec2f half_height( _shapeBox.height * 0.5f * -sin_a, _shapeBox.height * 0.5f * cos_a );
		shapePoly.points[0] = _shapeBox.position - half_widht - half_height;
		shapePoly.points[1] = _shapeBox.position + half_widht - half_height;
		shapePoly.points[2] = _shapeBox.position + half_widht + half_height;
		shapePoly.points[3] = _shapeBox.position - half_widht + half_height;
		addShapePolyData_( shapePoly );
	}
	//////////////////////////////////////////////////////////////////////////
	void Collision::addShapeCircleData_( const ShapeCircle& _shapeCircle )
	{
		const int pointsNum = 16;
		ShapePoly shapePoly;
		shapePoly.points.resize( pointsNum );
		float angle_step = mt::m_two_pi / pointsNum;
		for( int i = 0; i < pointsNum; ++i )
		{
			float angle = i * angle_step;
			float sin_a = ::sinf( angle );
			float cos_a = ::cosf( angle );
			shapePoly.points[i].x = cos_a * _shapeCircle.radius;
			shapePoly.points[i].y = sin_a * _shapeCircle.radius;
			shapePoly.points[i] += _shapeCircle.position;
		}
		addShapePolyData_( shapePoly );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
