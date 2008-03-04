#	include "SceneNode3D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "DiscreteEntity.h"

#	include "Light.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ObjectImplement.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(SceneNode3D);
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::SceneNode3D()
		: m_interface(0)
		//, m_entity(0)
		//, m_light(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D::~SceneNode3D()
	{
		Holder<RenderEngine>::hostage()->releaseSceneNode( m_interface );
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
	void SceneNode3D::addChild( SceneNode3D * _node )
	{
		m_interface->addChild( _node->m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::setScale( const mt::vec3f & _scale )
	{
		m_interface->setScale( (float*)_scale.m);
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::attachToRootNode()
	{
		const std::string & name = getName();
		m_interface = Holder<RenderEngine>::hostage()->createSceneNode( name );
	}
	//////////////////////////////////////////////////////////////////////////
	/*void SceneNode3D::attachEntity( DiscreteEntity * _entity )
	{
		m_interface->attachEntity(_entity->get());
	}*/
	//////////////////////////////////////////////////////////////////////////
	/*void SceneNode3D::attachLight( Light * _entity )
	{
		m_interface->attachLight( _entity->get() );
	}*/
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_addChildren( SceneNode3D * _node )
	{
		const std::string & name = _node->getName();
		_node->m_interface = m_interface->createChildSceneNode( name.c_str() );
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loaderTransformation_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Position")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER("Value", &SceneNode3D::setLocalPosition);
				}
			}

			XML_CASE_NODE("Scale")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER("Value", &SceneNode3D::setScale);
				}
			}

			XML_CASE_NODE("Rotation")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
				//	XML_CASE_ATTRIBUTE_MEMBER("Value", &SceneNode3D::setLocalOrient);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	/*void SceneNode3D::loaderEntities_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Entity")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
				}

				m_entity = new DiscreteEntity( name );
				m_entity->setParentNode( this );

			//	m_movables.push_back( entity );
				XML_PARSE_ELEMENT( m_entity, &DiscreteEntity::loader );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loaderLights_( XmlElement * _xml )
	{
		std::string name;

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Light")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", name );
				}

				m_light = new Light( name );
				m_light->setParentNode( this );

				//	m_movables.push_back( entity );
				XML_PARSE_ELEMENT( m_light, &Light::loader );
			}
		}
	}*/
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::loader( XmlElement * _xml )
	{
		NodeCore::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Transformation")
			{
				XML_PARSE_ELEMENT( this, &SceneNode3D::loaderTransformation_ );
			}

			/*XML_CASE_NODE("Entities")
			{
				XML_PARSE_ELEMENT( this, &SceneNode3D::loaderEntities_ );
			}

			XML_CASE_NODE("Lights")
			{
				XML_PARSE_ELEMENT( this, &SceneNode3D::loaderLights_ );
			}*/

		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode3D::_activate()
	{
	/*	for (std::list<Movable*>::iterator it = m_movables.begin(); it != m_movables.end(); ++it)
		{
			(*it)->activate();
		}
*/
		/*if(m_entity!=NULL)
		{
			m_entity->activate();
		}

		if(m_light!=NULL)
		{
			m_light->activate();
		}*/

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::render()
	{
	};
	//////////////////////////////////////////////////////////////////////////
	bool SceneNode3D::isRenderable()
	{
		return false;
	};
	//////////////////////////////////////////////////////////////////////////
	void SceneNode3D::_release()
	{
	}
}