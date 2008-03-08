#	pragma once

#	include "math/quat.h"
#	include <string>

#	include "SceneNode3D.h"

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
		: public SceneNode3D
	{
		OBJECT_DECLARE(DiscreteEntity);

	public:
		//DiscreteEntity( const std::string & _name );
		DiscreteEntity();
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
			return m_entInterface;
		}
		////
		void createRenderToTexture( const std::string& _renderCamera );

	public:
		virtual void loader( XmlElement * _xml );
		void update( float _timing ) override;

	protected:
		virtual bool _activate();
		virtual void _deactivate();
		virtual bool _compile();
		virtual void _release();

	public:
		SceneNode3D * getParentNode() const;
		void setParentNode( SceneNode3D * _node );

	private:

		EntityInterface * m_entInterface;

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