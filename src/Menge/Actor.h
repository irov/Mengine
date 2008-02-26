#	pragma once

#	include "SceneNode3D.h"

#	include "math/quat.h"
#	include <string>

class ControllerInterface;

namespace Menge
{
	class ResourceMesh;
	class ResourceSkeleton;
	class DiscreteEntity;
	class CapsuleController;
	class RigidBody3D;

	class Actor
		: public SceneNode3D
	{
		OBJECT_DECLARE( Actor )

	public:
		Actor();
		~Actor();

	public:
		//
		void setController( CapsuleController * _controller );
		void setRigidBody( RigidBody3D * _rb );
		//
	public:
		void loader( XmlElement * _xml ) override;
	public:

		void	attachEntity( DiscreteEntity * _entity );

		void	step( const mt::vec3f & _dir );
		void	left();
		void	stop();
	public:
		void	setParentActor( Actor * _parent, const mt::vec3f & _pos );
		void	setParentRoot( const mt::vec3f & _pos, const mt::quatf & _q );

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		CapsuleController * m_controller;
		RigidBody3D * m_rigidBody;

		DiscreteEntity * m_entityInterface;
		mt::quatf m_initOrient;
		mt::vec3f m_dir;
		mt::vec3f m_contMovement;
		RigidBody3D * m_body;
		//ControllerInterface * m_controller;

		mt::vec3f m_charPos;

		void render(){}
		bool isRenderable(){return false;}
	};
}