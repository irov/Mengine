#	pragma once

#	include "NodeSinglethon.h"

#	include "math/quat.h"
#	include <string>

class EntityInterface;

namespace Menge
{
	class ResourceMesh;
	class ResourceSkeleton;

	class Entity3d
		: public NodeSinglethon
	{
		OBJECT_DECLARE( Entity3d )

	public:
		Entity3d();
		~Entity3d();
	public:
		void loader( XmlElement * _xml ) override;

	public:

		const mt::quatf & getBoneWorldOrient( const std::string& _name );
		const mt::vec3f & getBoneWorldPosition( const std::string& _name );

	public:

		void play( const std::string& _name );
		void play2( const std::string& _name1, float _weight1,
			const std::string& _name2, float _weight2 );

		void stop();
		void setLooped( bool _looped );


		EntityInterface * get()
		{
			return m_entityInterface;
		}

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		std::string m_meshName;

		std::string m_resourcenameMesh;
		ResourceMesh * m_resourceMesh;

		std::string m_resourcenameSkeleton;
		ResourceSkeleton * m_resourceSkeleton;

		EntityInterface * m_entityInterface;

		bool	m_isGrounded;

		float m_scale;

		void render(){}
		bool isRenderable(){return false;}
	};
}