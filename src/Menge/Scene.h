#	pragma once

#	include "NodeImpl.h"

namespace Menge
{
	class Layer;
	class Camera;

	class Scene
		: public NodeImpl
	{
		OBJECT_DECLARE(Scene);

	public:
		Scene();

	public:
		void setRenderCamera( Camera * _camera);
		Camera * getRenderCamera();

	public:
		void loader(TiXmlElement *_xml) override;

	protected:
		bool _compile() override;

	private:
		std::string m_scriptFile;

		Camera * m_renderCamera;
	};
}