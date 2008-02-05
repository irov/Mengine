#	include "SceneNode3D.h"

#	include "XmlEngine.h"

#	include "Interface/RenderSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::SceneNode3D()
		: m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::~SceneNode3D()
	{
		delete m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & SceneNode3D::getWorldOrient()
	{
		return *(mt::quatf*)m_interface->getWorldOrient();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & SceneNode3D::getWorldPosition()
	{
		return *(mt::vec3f*)m_interface->getWorldPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & SceneNode3D::getLocalOrient()
	{
		return *(mt::quatf*)m_interface->getLocalOrient();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & SceneNode3D::getLocalPosition()
	{
		return *(mt::vec3f*)m_interface->getLocalPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setLocalPosition( const mt::vec3f & _position )
	{
		m_interface->setLocalPosition( _position.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setLocalOrient( const mt::quatf & _quat )
	{
		m_interface->setLocalOrient( _quat.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::translate( const mt::vec3f & _delta )
	{
		m_interface->translate( _delta.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setScale( float _scale )
	{
		m_interface->setScale( _scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::yaw( float _angle )
	{
		m_interface->yaw( _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::pitch( float _angle )
	{
		m_interface->pitch( _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::roll( float _angle )
	{
		m_interface->roll( _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loader( XmlElement * _xml )
	{
		NodeCore::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Transformation")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER("Value", &SceneNode3D::setLocalPosition);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}