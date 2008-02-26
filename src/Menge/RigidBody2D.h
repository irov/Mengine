#	pragma once

#	include "SceneNode2D.h"
#	include "Interface/PhysicSystem2DInterface.h"

#	include "Math/polygon.h"

class PhysicBody2DInterface;

namespace Menge
{

	class RigidBody2D
		: public SceneNode2D
		, public PhysicBody2DCollisionListener
	{
		OBJECT_DECLARE( RigidBody2D )

	public:
		RigidBody2D();
		virtual ~RigidBody2D();

		mt::vec2f getPosition() const;
		void setPosition( const mt::vec2f& _pos );

		void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) override;

	public:
		void render() override;
		bool isRenderable() override;
		void loader( XmlElement * _xml ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		PhysicBody2DInterface* m_interface;
		mt::polygon m_shape;
		float m_density;
		float m_friction;
		float m_restitution;

		void _loaderShape( XmlElement * _xml );
	};

}	// namespace Menge