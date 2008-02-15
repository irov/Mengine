#	include "Light.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Light )
	//////////////////////////////////////////////////////////////////////////
	Light::Light()
	: m_diffuseColor( 1.0f, 1.0f, 1.0f, 1.0f )
	, m_position( 0.f, 0.f, 0.f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Light::~Light()
	{
		Holder<RenderEngine>::hostage()->releaseLight( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::_update( float _timing )
	{}
	//////////////////////////////////////////////////////////////////////////
	void Light::loader( XmlElement * _xml )
	{
		NodeSinglethon::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Color", "Value", m_diffuseColor );
			XML_CASE_ATTRIBUTE_NODE( "Position", "Value", m_position );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light::_activate()
	{
		bool result = NodeSinglethon::_activate();
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::_deactivate()
	{
		NodeSinglethon::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Light::_compile()
	{
		if( NodeSinglethon::_compile() == false )
		{
			return false;
		}

		const std::string& name = this->getName();
		m_interface = Holder<RenderEngine>::hostage()->createLight( name );

		//m_interface->setPosition( m_position.x, m_position.y, m_position.z );
		//m_interface->setDiffuseColor( m_diffuseColor.r, m_diffuseColor.g, m_diffuseColor.b );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Light::_release()
	{
		NodeSinglethon::_release();
		//
	}
}
