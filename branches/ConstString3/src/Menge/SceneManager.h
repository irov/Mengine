#	pragma once

#	include "PrototypeManager.h"

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class Scene;
	
	class SceneManager
		: public PrototypeManager
		, public Holder<SceneManager>
	{
	public:		
		Scene * createScene( const ConstString & _name );
	};
}