#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/PlayerInterface.h"

#	include "Core/Params.h"

namespace Menge
{
	struct ResourcePackDesc
	{
		ResourcePackDesc()
			: dev(false)
			, preload(true)
		{
		}

		ConstString name;
		ConstString type;

		ConstString locale;
		ConstString platform;

		ConstString path;
		ConstString descriptionPath;

		bool dev;
		bool preload;
	};

	typedef stdex::vector<ResourcePackDesc> TVectorResourcePackDesc;
    
    class GameServiceInterface
        : public ServiceInterface
        , public InputSystemHandler
    {
        SERVICE_DECLARE("GameService")

    public:
        virtual bool initialize( const FilePath & _accountPath, uint32_t _projectVersion, const TMapParams & _params, const String & _scriptInitParams ) = 0;
        virtual void finalize() = 0;

	public:
		virtual void run() = 0;

    public:
        virtual void setDevelopmentMode( bool _developmentMode ) = 0;

    public:
        virtual bool beginUpdate() = 0;
        virtual void tick( float _timing ) = 0;
		virtual void endUpdate() = 0;

	public:
        virtual void render() = 0;

    public:
        virtual PlayerServiceInterface * getPlayer() const = 0;

    public:
        virtual void setCursorMode( bool _mode ) = 0;

    public:
        virtual float getTimingFactor() const = 0;
        virtual void setTimingFactor( float _timingFactor ) = 0;

    public:
        virtual const WString & getParam( const ConstString & _paramName ) const = 0;
        virtual bool hasParam( const ConstString & _paramName ) const = 0;

    public:
		virtual void addResourcePak( const ResourcePackDesc & _desc ) = 0;
		virtual void addLanguagePak( const ResourcePackDesc & _desc ) = 0;

	public:
		virtual bool loadResourcePak( const ResourcePackDesc & _desc ) = 0;

	public:
        virtual void setLanguagePack( const ConstString& _packName ) = 0;
        virtual const ConstString & getLanguagePack() const = 0;

    public:
        virtual bool applyConfigPaks() = 0;

    public:
        virtual bool loadPersonality( const ConstString & _module ) = 0;

    public:
        virtual void initializeRenderResources() = 0;
        virtual void finalizeRenderResources() = 0;

        virtual void mouseLeave() = 0;
        virtual void mouseEnter( const mt::vec2f & _point ) = 0;
		virtual void mousePosition( const mt::vec2f & _point ) = 0;

        virtual void setFocus( bool _focus ) = 0;
        virtual void setFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void setFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
		virtual void setGameViewport( const Viewport & _viewport, float _aspect ) = 0;

        virtual bool close() = 0;

        virtual void userEvent( const ConstString & _event, const TMapParams & _params ) = 0;

        virtual void turnSound( bool _turn ) = 0;
    };

#   define GAME_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::GameServiceInterface)
}