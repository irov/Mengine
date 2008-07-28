#	pragma once

#	include "Node.h"
#	include "math/quat.h"

class ControllerInterface;

namespace Menge
{
	class SceneNode3D;

	class ResourceCapsuleController;

	class CapsuleController
		: public Node
	{
		OBJECT_DECLARE( CapsuleController )

	public:
		CapsuleController();
		~CapsuleController();
	public:
		void move( const mt::vec3f & _displacement );
		void setPosition( const mt::vec3f & _pos );
		const mt::vec3f & getFilteredPosition();

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
		mt::vec3f m_startPosition;

		ControllerInterface * m_interface;

		std::string m_resourcename;
		ResourceCapsuleController * m_resource;

		void render( bool _enableDebug ){}
		bool isRenderable(){return false;}
	};
}