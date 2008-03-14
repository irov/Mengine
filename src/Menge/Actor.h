#	pragma once

#	include "SceneNode3D.h"

#	include "math/quat.h"
#	include <string>
#	include <vector>

class ControllerInterface;
class EntityInterface;
class SkeletonInterface;

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
		//void setController( CapsuleController * _controller );
		void setRigidBody( RigidBody3D * _rb );
		//
	public:
		void loader( XmlElement * _xml ) override;
	public:

		void	step( const mt::vec3f & _dir );
		void	left();
		void	stop();

		void	setAnimationEnabled( const std::string& _animName, bool _enable );
		void	playAnimation( const std::string& _animName );		// enables animation
		void	pauseAnimation( const std::string& _animName );		// stops playing animation but not disables it
		void	stopAnimation( const std::string& _animName );		// returns animation to first frame and disables it
		void	stopAllAnimations();

		void	setPhysicControllerEnabled( bool _enabled );
		void	setMoveSpeed( const mt::vec3f& _speed );
		void	setYawSpeed( float _speed );
		void	setPitchSpeed( float _speed );
		void	setRollSpeed( float _speed );
		//void	

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		//CapsuleController * m_controller;
		ControllerInterface* m_controller;
		RigidBody3D * m_rigidBody;
		
		std::string m_resourcenameMesh;
		EntityInterface* m_entity;
		SkeletonInterface* m_skeleton;

		mt::quatf m_initOrient;
		mt::vec3f m_dir;
		mt::vec3f m_contMovement;

		mt::vec3f m_moveSpeed;
		float m_yawSpeed;
		float m_pitchSpeed;
		float m_rollSpeed;

		typedef std::vector<std::string> TStringVector;
		TStringVector m_playingAnimations;
		//	public:
		//		void	setParentActor( Actor * _parent, const mt::vec3f & _pos );
		//		void	setParentRoot( const mt::vec3f & _pos, const mt::quatf & _q );
		//mt::vec3f m_charPos;

		void render(){}
		bool isRenderable(){return false;}
	};
}