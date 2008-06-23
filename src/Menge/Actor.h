#	pragma once

#	include "SceneNode3D.h"

#	include "math/quat.h"

#	include <vector>

class ControllerInterface;
class EntityInterface;
class SkeletonInterface;

namespace Menge
{
	class ResourceMesh;
	class RigidBody3D;

	class Actor
		: public SceneNode3D
	{
		OBJECT_DECLARE( Actor )

	public:
		Actor();
		~Actor();

	public:
		void	setRigidBody( RigidBody3D * _rb );

	public:
		void	loader( XmlElement * _xml ) override;
	public:

		void	setAnimationEnabled( const std::string& _animName, bool _enable );
		void	playAnimation( const std::string& _animName );	
		void	pauseAnimation( const std::string& _animName );
		void	stopAnimation( const std::string& _animName );	
		void	stopAllAnimations();

		void	setPhysicControllerEnabled( bool _enabled );
		void	setMoveSpeed( const mt::vec3f& _speed );
		void	setYawSpeed( float _speed );
		void	setPitchSpeed( float _speed );
		void	setRollSpeed( float _speed );

		float	getBoundingRadius() const;

		void	attachEntity( const std::string& _boneName, SceneNode3D* _node );
		EntityInterface*	getEntity();
			
	protected:
		bool	_activate() override;
		void	_deactivate() override;
		bool	_compile() override;
		void	_release() override;
		void	_update( float _timing ) override;

		void	_loaderPhysicController( XmlElement * _xml );

	private:

		void	_updateMovement( float _timing );
		void	_updateAnimation( float _timing );

		RigidBody3D * m_rigidBody;
		bool m_physicController;
		mt::vec3f m_physCapsuleCenter;
		float m_physCapsuleRadius;
		float m_physCapsuleHeight;

		ControllerInterface * m_controller;
		EntityInterface * m_entity;
		SkeletonInterface * m_skeleton;

		std::string m_resourcename;
		ResourceMesh * m_resource;

		mt::vec3f m_moveSpeed;
		float m_yawSpeed;
		float m_pitchSpeed;
		float m_rollSpeed;

		typedef std::vector<std::string> TStringVector;
		TStringVector m_playingAnimations;

		void render( bool _debugRender ){}
		bool isRenderable(){return false;}
	};
}