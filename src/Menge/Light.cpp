#	include "Light.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Light )
	//////////////////////////////////////////////////////////////////////////
	Light::Light()
	: m_diffuseColor( 1.0f, 1.0f, 1.0f, 1.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Light::~Light()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::_update( float _timing )
	{}
	//////////////////////////////////////////////////////////////////////////
	void Light::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Color", "Value", m_diffuseColor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light::_activate()
	{
		bool result = SceneNode3D::_activate();
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::_deactivate()
	{
		SceneNode3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light::_compile()
	{
		if( SceneNode3D::_compile() == false )
		{
			return false;
		}

		const std::string& name = this->getName();
		m_interface = Holder<RenderEngine>::hostage()->createLight(name);

		const mt::vec3f & pos = this->getLocalPosition();
		m_interface->setPosition(pos.x,pos.y,pos.z);
		m_interface->setDiffuseColor(m_diffuseColor.r, m_diffuseColor.g, m_diffuseColor.b);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::_release()
	{
		SceneNode3D::_release();
		//
	}
}
