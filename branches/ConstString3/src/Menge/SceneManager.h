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
        SceneManager();

    public:
        bool initialize();
        void finalize();

	public:		
		Scene * createScene( const ConstString & _name );

    protected:
        typedef std::map<ConstString, Scene *> TMapScenes;
        TMapScenes m_scenes;
	};
}