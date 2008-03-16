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
		DiscreteEntity();
		~DiscreteEntity();
	public:
		Skeleton * getSkeleton() const;//kill
		void	setCastsShadow( bool _castsShadow );
		void	setVisible( bool _visible );
		void	setMaterial( const std::string & _material );
		void	setSubEntityMaterial( const std::string & _subEntity, const std::string & _material );
		void	createRenderToTexture( const std::string& _renderCamera, int _width, int _height );

	public:
		void	loader( XmlElement * _xml );

	protected:
		bool	_activate() override;
		void	_deactivate() override;
		bool	_compile() override;
		void	_release() override;
		void	_update( float _timing ) override;

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