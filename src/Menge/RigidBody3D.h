#	pragma once

#	include "SceneNode3D.h"

class RigidBodyInterface;

namespace Menge
{
	class ResourcePhysicGeometry;

	class RigidBody3D
		: public SceneNode3D
	{
		OBJECT_DECLARE( RigidBody3D )

	public:
		RigidBody3D();
		~RigidBody3D();
	public:
		void loader( XmlElement * _xml ) override;
	
	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		bool m_active;
		bool m_density;
		std::string m_resourcename;
		ResourcePhysicGeometry * m_resource;
		RigidBodyInterface * m_interface;
	};
}