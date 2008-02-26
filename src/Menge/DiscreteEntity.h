#	pragma once

#	include "math/quat.h"
#	include <string>

class EntityInterface;
class SkeletonInterface;

class XmlElement;

namespace Menge
{
	class ResourceMesh;
	class ResourceSkeleton;

	class Skeleton;
	class SceneNode3D;

	class DiscreteEntity
	{
	public:
		DiscreteEntity( const std::string & _name );
		~DiscreteEntity();
	public:
		Skeleton * getSkeleton() const;
		void setCastsShadow( bool _castsShadow );
		void setVisible( bool _visible );
		void setMaterial( const std::string & _material );
		void setSubEntityMaterial( const std::string & _subEntity, const std::string & _material );

		//hack ??
		EntityInterface * get()
		{
			return m_interface;
		}

	public:
		void loader( XmlElement * _xml );
		bool activate();
		void deactivate();
		void release();


	public:
		SceneNode3D * getParentNode() const;
		void setParentNode( SceneNode3D * _node );

	private:

		std::string m_name;

		EntityInterface * m_interface;

		Skeleton * m_skeleton;

		std::string m_resourcenameMesh;
		ResourceMesh * m_resourceMesh;

		std::string m_resourcenameSkeleton;
		ResourceSkeleton * m_resourceSkeleton;

		bool m_castShadows;
		bool m_receiveShadows;

		SceneNode3D * m_parent;
	};
}