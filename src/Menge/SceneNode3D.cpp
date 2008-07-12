#	include "SceneNode3D.h"
#	include "ObjectImplement.h"
#	include "LogEngine.h"

namespace Menge
{
/*	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( SceneNode3D );
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::SceneNode3D()
		: m_yaw( 0.0f )
		, m_pitch( 0.0f )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & SceneNode3D::getWorldMatrix3D()
	{
		if( m_parent == 0 )
		{
			return getLocalMatrix3D();
		}

		updateMatrix3D( m_parent );

		return Allocator3D::getWorldMatrix3D();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::changePivot3D()
	{
		Allocator3D::changePivot3D();

		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			(*it)->changePivot3D();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loader( XmlElement * _xml )
	{
		Allocator3D::loader( _xml );
		Renderable3D::loader( _xml );

		Node::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::yaw( float _degrees )
	{
		mt::rotate_axis_m4( m_localMatrix, m_fixedUp, _degrees);

		changePivot3D();

		m_yaw += _degrees;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::pitch( float _degrees )
	{
		const mt::vec3f & axis = getLocalStrafe3D();

		mt::rotate_axis_m4( m_localMatrix, axis, _degrees);

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
	*/
}