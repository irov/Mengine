#	pragma once

#	include "Allocator2D.h"

namespace Menge
{
	class Scene;

	class Entity
		: public Allocator2D
	{
		OBJECT_DECLARE( Entity )

	public:
		void setScene( Scene * _scene );

	public:
		void setLayer( const std::string & _name );

	protected:
		bool _activate();

	private:
		Scene * m_scene;
	};
}