#	pragma once

#	include "NodeSinglethon.h"
#	include "Color.h"

class LightInterface;

namespace Menge
{
	class Scene;

	class Light
		: public NodeSinglethon
	{
		OBJECT_DECLARE( Light )

	public:
		Light();
		~Light();
	public:
		void loader( XmlElement * _xml ) override;
	
	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		LightInterface * m_interface;
		Color m_diffuseColor;
		mt::vec3f	m_position;

		void render(){}
		bool isRenderable(){return false;}
	};
}