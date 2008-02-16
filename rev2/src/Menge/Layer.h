#	pragma once

#	include "Node.h"
#	include "NodeCore.h"
#	include "InputHandler.h"

#	include "Math/vec2.h"

namespace Menge
{
	class Scene;

	class Layer
		: public /*virtual */NodeCore//NodeRenderable
		, public InputHandler
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

	public:
		virtual void setOffsetPosition( const mt::vec2f & _offset );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool m_main;
		mt::vec2f m_size;		

		Scene * m_scene;
	};
}