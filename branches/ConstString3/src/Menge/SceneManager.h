#	pragma once

#	include "Config/Typedef.h"

#	include "Core/Holder.h"
#	include "Core/ConstString.h"

#	include <map>

namespace Menge
{
	class Scene;

	struct SceneDesc
	{
		ConstString pak;
		ConstString path;
	};

	class SceneManager
		: public Holder<SceneManager>
	{
	public:
		SceneManager();
		~SceneManager();

	public:
		void registerScene( const ConstString & _name, const SceneDesc & _desc );

	public:
		Scene * createScene( const ConstString & _name );

	protected:
		typedef std::map<ConstString, SceneDesc> TMapDescriptionScenes;
		TMapDescriptionScenes m_descriptions;
	};
}