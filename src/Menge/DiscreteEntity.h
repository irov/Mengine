#	pragma once

#	include "Movable.h"
#	include "math/quat.h"
#	include <string>

class EntityInterface;
class SkeletonInterface;

class XmlElement;

namespace Menge
{
	class ResourceMesh;
	class ResourceSkeleton;

	class DiscreteEntity : public Movable
	{
	public:
		DiscreteEntity( const std::string & _name );
		~DiscreteEntity();
	public:
		void loader( XmlElement * _xml );
		bool activate();
		void deactivate();

	public:
		SkeletonInterface * getSkeleton() const;
		void setCastsShadow( bool _castsShadow );
		void setVisible( bool _visible );
		void setMaterial( const std::string & _material );
		void setSubEntityMaterial( const std::string & _subEntity, const std::string & _material );

		//hmmmmmmmmm
		EntityInterface * get()
		{
			return m_interface;
		}

	private:

		std::string m_name;

		EntityInterface * m_interface;

		std::string m_resourcenameMesh;
		ResourceMesh * m_resourceMesh;

		std::string m_resourcenameSkeleton;
		ResourceSkeleton * m_resourceSkeleton;

		bool m_castShadows;
		bool m_receiveShadows;
	};
}