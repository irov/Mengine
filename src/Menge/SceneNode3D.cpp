#	include "SceneNode3D.h"
#	include "ObjectImplement.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( SceneNode3D );
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::SceneNode3D()
		: m_yaw( 0.0f )
		, m_pitch( 0.0f )
	{}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & SceneNode3D::getWorldMatrix3D()
	{
		SceneNode3D * sceneNode = dynamic_cast<SceneNode3D*>(m_parent);
		if( sceneNode == 0 )
		{
			return getLocalMatrix3D();
		}


		if( sceneNode )
		{
			const mt::mat4f & wm = sceneNode->getWorldMatrix3D();

			updateMatrix3D( wm );
		}

		return Allocator3D::getWorldMatrix3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::changePivot3D()
	{
		Allocator3D::changePivot3D();

		for( TContainerChildren::iterator
			it = m_children.begin(),
			it_end = m_children.end();
		it != it_end;
		++it)
		{
			SceneNode3D * sceneNode = dynamic_cast<SceneNode3D*>(*it);

			sceneNode->changePivot3D();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loader( XmlElement * _xml )
	{
		Node::loader( _xml );
		Allocator3D::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::yaw( float _degrees )
	{
		mt::rotate_axis_m4( m_localMatrix3D, m_fixedUp, _degrees);

		changePivot3D();

		m_yaw += _degrees;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::pitch( float _degrees )
	{
		const mt::vec3f & axis = getLocalStrafe3D();

		mt::vec3f saxis = mt::norm_safe_v3( axis );
		mt::rotate_axis_m4( m_localMatrix3D, saxis, _degrees);

		changePivot3D();

		m_pitch += _degrees;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::yawTime( float _degrees, float _time )
	{
		m_yawTime.start( 0.0f, _degrees, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::pitchTime( float _degrees, float _time )
	{
		m_pitchTime.start( 0.0f, _degrees, _time, ::fabsf );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_update( float _timing )
	{
		float v;
		if( m_yawTime.isStarted() )
		{
			if( m_yawTime.update( _timing, &v ) )
			{
				this->callEvent( "YAWTIME_END", "(O)", this->getEmbedding() );
			}
			yaw( m_yawTime.getDelta() );
		}

		if( m_pitchTime.isStarted() )
		{
			if( m_pitchTime.update( _timing, &v ) )
			{
				this->callEvent( "PITCHTIME_END", "(O)", this->getEmbedding() );
			}
			pitch( m_pitchTime.getDelta() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SceneNode3D::getYaw()
	{
		return m_yaw;
	}
	//////////////////////////////////////////////////////////////////////////
	float SceneNode3D::getPitch()
	{
		return m_pitch;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_setListener()
	{
		registerEvent( "YAWTIME_END", "onYawTimeEnd", m_listener );
		registerEvent( "PITCHTIME_END", "onPitchTimeEnd", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
}