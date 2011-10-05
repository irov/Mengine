#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include "ResourceManager.h"

#	include <map>

namespace Menge
{
	class Scene;
	
	class SceneManager
		: public Holder<SceneManager>
	{
	public:
		SceneManager();
		~SceneManager();

	public:
		void registerScene( const ConstString & _name, const ResourceDesc & _desc );
		bool hasScene( const ConstString & _name );

	public:		
		Scene * createScene( const ConstString & _name );

	protected:
		typedef std::map<ConstString, ResourceDesc> TMapDescriptionScenes;
		TMapDescriptionScenes m_descriptions;
	};
}