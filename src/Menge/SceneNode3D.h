#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "math/quat.h"

class SceneNodeInterface;

namespace Menge
{
	class SceneNode3D
		: public NodeCore
		, public NodeChildren<SceneNode3D>
	{
	public:
		SceneNode3D();
		~SceneNode3D();

	public:
		virtual const mt::quatf & getWorldOrient();
		virtual const mt::vec3f & getWorldPosition();

		const mt::quatf & getLocalOrient();
		const mt::vec3f & getLocalPosition();

		void setLocalPosition( const mt::vec3f & _position );
		void setLocalOrient( const mt::quatf & _quat );

		void translate( const mt::vec3f & _delta );
		void setScale( float _scale );
		
		void yaw( float _angle );
		void pitch( float _angle );
		void roll( float _angle );

	public:
		void loader( XmlElement * _xml ) override;

		
		void render(){};
		bool isRenderable() {return false;};

	protected:
		SceneNodeInterface * m_interface;
	};
}