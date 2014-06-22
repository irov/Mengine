#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/ApplicationInterface.h"
#   include "Interface/PlayerInterface.h"

namespace Menge
{
    class Node;
    
    class GameServiceInterface
        : public ServiceInterface
        , public InputSystemHandler
    {
        SERVICE_DECLARE("GameService")

    public:
        virtual bool initialize( const FilePath & _accountPath, size_t _projectVersion, bool _projectVersionCheck, const TMapParams & _params ) = 0;
        virtual void finalize() = 0;

    public:
        virtual void setBaseDir( const FilePath & _baseDir ) = 0;
        virtual void setDevelopmentMode( bool _developmentMode ) = 0;
        virtual void setPlatformName( const ConstString & _platformName ) = 0;

    public:
        virtual bool run( const String& _scriptInitParams ) = 0;

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
		virtual bool loadResourcePak( const FilePath & _baseDir, const ResourcePackDesc & _desc ) = 0;

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
        virtual bool close() = 0;

        virtual void userEvent( const ConstString & _event, const TMapParams & _params ) = 0;

        virtual void turnSound( bool _turn ) = 0;
    };

#   define GAME_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::GameServiceInterface)
}