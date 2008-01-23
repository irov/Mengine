#	pragma once

#	include "SceneNode3D.h"

class Entity3dInterface;

namespace Menge
{
	class Scene;

	class RenderMesh3D
		: public SceneNode3D
	{
		OBJECT_DECLARE( RenderMesh3D )

	public:
		RenderMesh3D();
		~RenderMesh3D();
	public:
		void loader( XmlElement * _xml ) override;
	
	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		std::string m_meshName;
		Entity3dInterface * m_interface;
	};
}