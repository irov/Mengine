#	pragma once

#	include "SceneNode3D.h"

#	include "math/quat.h"
#	include <string>


class EntityInterface;
class MeshInterface;

namespace Menge
{
	class ResourceMesh;
	class ResourceSkeleton;

	class RenderMesh3D
		: public SceneNode3D
	{
		OBJECT_DECLARE( RenderMesh3D )

	public:
		RenderMesh3D();
		~RenderMesh3D();
	public:
		void loader( XmlElement * _xml ) override;

	public:

		const mt::quatf & getBoneWorldOrient( const std::string& _name );
		const mt::vec3f & getBoneWorldPosition( const std::string& _name );

	public:

		void play( const std::string& _name );
		void play( const std::string& _name1, float _weight1,
			const std::string& _name2, float _weight2 );

		void stop();
		void setLooped( bool _looped );

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		std::string m_meshName;
		EntityInterface * m_interface;

		std::string m_resourcenameMesh;
		ResourceMesh * m_resourceMesh;

		std::string m_resourcenameSkeleton;
		ResourceSkeleton * m_resourceSkeleton;

		const MeshInterface * m_interfaceMesh; 

		mt::quatf	m_worldBoneOrient;
		mt::vec3f	m_worldBonePos;
	};
}