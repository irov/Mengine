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
	const mt::quatf & SceneNode3D::getLocalOrient() const
	{
		return *(mt::quatf*)m_interface->getLocalOrient();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & SceneNode3D::getLocalPosition() const
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
	void SceneNode3D::_addChildren( SceneNode3D * _node )
	{
		const std::string & name = _node->getName();

		_node->m_interface 
			= m_interface->createChildSceneNode( name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
}