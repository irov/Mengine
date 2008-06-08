#	pragma once

#	include "Node.h"

#	include "Math/vec2.h"

namespace Menge
{
	class Scene;

	class Layer
		: public Node
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
		virtual void setRenderTarget( const std::string& _cameraName );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool m_main;
		mt::vec2f m_size;		

		Scene * m_scene;
	};
}