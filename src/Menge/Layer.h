#	pragma once

#	include "Config/Typedef.h"

#	include "Node.h"
#	include "NodeRenderable.h"

#	include "Math/vec2.h"
#	include "Math/vec4.h"

namespace Menge
{
	class Scene;

	class Layer
		: public virtual NodeRenderable
	{
	public:
		Layer();

	public:
		void setMain( bool value );
		bool isMain() const;

		void setScene( Scene * _scene );
		Scene * getScene();

		void setSize( const mt::vec2f & _size );
		const mt::vec2f & getSize() const;

		void setRenderArea( const mt::vec4f& _renderArea );
		const mt::vec4f& getRenderArea() const;

	public:
		virtual void setOffsetPosition( const mt::vec2f & _offset );
		virtual void setRenderTarget( const std::string& _cameraName );
		virtual bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _renderBegin() override;

	protected:
		bool m_main;
		mt::vec2f m_size;		

		mt::vec4f m_renderArea;
		String m_renderTarget;

		Scene * m_scene;
	};
}