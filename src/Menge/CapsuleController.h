#	pragma once

#	include "NodeSinglethon.h"
#	include "math/quat.h"

class ControllerInterface;

namespace Menge
{
	class SceneNode3D;

	class CapsuleController
		: public NodeSinglethon
	{
		OBJECT_DECLARE( CapsuleController )

	public:
		CapsuleController();
		~CapsuleController();
	public:
		void setPosition( const mt::vec3f & _position );
		const mt::vec3f & getPosition();
		const mt::quatf & getOrientation();

	public:
		void loader( XmlElement * _xml ) override;
	
	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		float m_radius;
		float m_height;
		ControllerInterface * m_interface;

		std::string m_resourcename;
		//ResourcePhysicGeometry * m_resource;

		void render(){}
		bool isRenderable(){return false;}
	};
}