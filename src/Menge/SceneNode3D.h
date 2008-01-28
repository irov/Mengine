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
	protected:
		SceneNode3D();
		~SceneNode3D();

	public:
		virtual const mt::quatf & getWorldOrient();
		virtual const mt::vec3f & getWorldPosition();

		const mt::quatf & getLocalOrient() const;
		const mt::vec3f & getLocalPosition() const;

		void setLocalPosition( const mt::vec3f & _position );
		void setLocalOrient( const mt::quatf & _quat );

	public:
		void loader( XmlElement * _xml ) override;

	private:
		void _addChildren( SceneNode3D * _node ) override;

	protected:
		SceneNodeInterface * m_interface;
	};
}