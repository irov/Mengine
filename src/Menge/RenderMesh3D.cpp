#	include "RenderMesh3D.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RenderMesh3D )
	//////////////////////////////////////////////////////////////////////////
	RenderMesh3D::RenderMesh3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMesh3D::~RenderMesh3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::_update( float _timing )
	{
		const mt::vec3f& pos = this->getWorldPosition();

		m_interface->setPosition(pos.x,pos.y,pos.z);

		// не понятно как огр направление юзает, поэтому пока через кватернионы
		//const mt::vec3f& d = this->getWorldDirection();
		//m_interface->setDirection((float*)d.m);
		const mt::mat4f& M = this->getWorldMatrix();
		mt::quatf q;
		mt::q_from_rot_m4(q, M);
		m_interface->setDirection(q.m);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "Mesh", "Value", m_meshName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMesh3D::_activate()
	{
		bool result = SceneNode3D::_activate();
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::_deactivate()
	{
		SceneNode3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMesh3D::_compile()
	{
		bool result = SceneNode3D::_compile();

		const std::string & entityName = this->getName();
		m_interface = Holder<RenderEngine>::hostage()->create3dEntity( entityName, m_meshName );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh3D::_release()
	{
		SceneNode3D::_release();
		//
	}
}
