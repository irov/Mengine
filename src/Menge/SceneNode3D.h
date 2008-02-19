#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "math/quat.h"

class SceneNodeInterface;

namespace Menge
{
	class Movable;
	class DiscreteEntity;

	class SceneNode3D
		: public NodeCore
		, public NodeChildren<SceneNode3D>
	{
		OBJECT_DECLARE(SceneNode3D);
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
		void setScale( const mt::vec3f & _scale );
		
		void yaw( float _angle );
		void pitch( float _angle );
		void roll( float _angle );

		void addChild( SceneNode3D * _node );

		void attachToRootNode();

	public:
		void loader( XmlElement * _xml ) override;
		void loaderTransformation_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );

	protected:


		bool _activate() override;
		void _addChildren( SceneNode3D * _node ) override;

	protected:
		SceneNodeInterface * m_interface;

	private:
		std::list<Movable*>	m_movables;

		DiscreteEntity * m_entity;

		// �� � ���� ������ ������??
		void render(){};
		bool isRenderable() {return false;};
	};
}