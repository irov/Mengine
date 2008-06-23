#	include "SceneNode3D-649.h"
#	include "ObjectImplement.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( SceneNode3D_ );
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D_::SceneNode3D_()
		: m_yaw( 0.0f )
		, m_pitch( 0.0f )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & SceneNode3D_::getWorldMatrix()
	{
		if( m_parent == 0 )
		{
			return getLocalMatrix();
		}

		updateMatrix( m_parent );

		return Allocator3D_::getWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::changePivot()
	{
		Allocator3D_::changePivot();

		for( TListChildren::iterator
			it = m_listChildren.begin(),
			it_end = m_listChildren.end();
		it != it_end;
		++it)
		{
			(*it)->changePivot();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::loader( XmlElement * _xml )
	{
		Allocator3D_::loader( _xml );
		Renderable3D::loader( _xml );

		NodeCore::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::yaw( float _degrees )
	{
		mt::rotate_axis_m4( m_localMatrix, m_fixedUp, _degrees);

		changePivot();

		m_yaw += _degrees;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::pitch( float _degrees )
	{
		const mt::vec3f & axis = getLocalStrafe();

		mt::rotate_axis_m4( m_localMatrix, axis, _degrees);

		changePivot();

		m_pitch += _degrees;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::yawTime( float _degrees, float _time )
	{
		m_yawTime.start( 0.0f, _degrees, _time );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::pitchTime( float _degrees, float _time )
	{
		m_pitchTime.start( 0.0f, _degrees, _time );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::_update( float _timing )
	{
		float v;
		if( m_yawTime.isStarted() )
		{
			if( m_yawTime.update( _timing, &v ) )
			{
				this->callEvent( "YAWTIME_END", "(O)", this->getScript() );
			}
			yaw( m_yawTime.getDelta() );
		}

		if( m_pitchTime.isStarted() )
		{
			if( m_pitchTime.update( _timing, &v ) )
			{
				this->callEvent( "PITCHTIME_END", "(O)", this->getScript() );
			}
			pitch( m_pitchTime.getDelta() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SceneNode3D_::getYaw()
	{
		return m_yaw;
	}
	//////////////////////////////////////////////////////////////////////////
	float SceneNode3D_::getPitch()
	{
		return m_pitch;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D_::setListener( PyObject* _listener )
	{
		Eventable::setListener( _listener );
		registerEventListener( "YAWTIME_END", "onYawTimeEnd", _listener );
		registerEventListener( "PITCHTIME_END", "onPitchTimeEnd", _listener );
	}
	//////////////////////////////////////////////////////////////////////////
}