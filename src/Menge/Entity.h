#	pragma once

#	include "SceneNode2D.h"

//extern "C" 
//{ 
//	struct _typeobject; 
//	typedef _typeobject PyTypeObject;
//}

namespace Menge
{
	class Scene;

	class Entity
		: public SceneNode2D
	{
		OBJECT_DECLARE( Entity )

	public:
		Entity();
		~Entity();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _activate() override;
	};
}