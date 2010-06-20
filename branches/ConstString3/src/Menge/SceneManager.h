#	pragma once

#	include "Manager/ConstManager.h"

#	include <map>

namespace Menge
{
	class Scene;

	struct SceneDesc
	{
		ConstString pak;
		String path;
	};

	class SceneManager
		: public Holder<SceneManager>
	{
	public:
		void registerScene( const ConstString & _name, const SceneDesc & _desc );

	public:
		Scene * getScene( const ConstString & _name );

	protected:
		Scene * createScene_( const ConstString & _name );

	protected:
		typedef std::map<ConstString, Scene *> TMapScenes;
		TMapScenes m_scenes;

		typedef std::map<ConstString, SceneDesc> TMapDescriptionScenes;
		TMapDescriptionScenes m_descriptions;
	};
}