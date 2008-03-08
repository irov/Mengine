#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "math/quat.h"

class SceneNodeInterface;
class EntityInterface;

namespace Menge
{
	class DiscreteEntity;
	class Light;
	class Camera3D;

	class SceneNode3D
		: public NodeCore
		, public NodeChildren<SceneNode3D>
	{
		OBJECT_DECLARE(SceneNode3D);
	public:
		SceneNode3D();
		virtual ~SceneNode3D();

	public:
		virtual const mt::quatf & getWorldOrient();
		virtual const mt::vec3f & getWorldPosition();

		const mt::quatf & getLocalOrient();
		const mt::vec3f & getLocalPosition();

		void setLocalPosition( const mt::vec3f & _position );
		void setLocalOrient( const mt::quatf & _quat );

		void translate( const mt::vec3f & _delta );
		void setScale( const mt::vec3f & _scale );
		
		void yaw( float _angle );
		void pitch( float _angle );
		void roll( float _angle );

	public:
		void addChild(SceneNode3D * _node);
		void attachToRootNode();

	public:
		virtual void loader( XmlElement * _xml ) override;

		void loaderTransformation_( XmlElement * _xml );

	protected:
		bool _activate() override;
		//void _deactivate() override;
		void _release() override;
		void _addChildren( SceneNode3D * _node ) override;

	protected:
		SceneNodeInterface * m_interface;

	private:
		Camera3D * m_camera;

		void render();
		bool isRenderable();
	};
}
