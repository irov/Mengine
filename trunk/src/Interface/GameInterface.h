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
        virtual bool initialize( const TMapParams & _params ) = 0;
        virtual void finalize() = 0;

    public:
        virtual void setBaseDir( const FilePath & _baseDir ) = 0;
        virtual void setDevelopmentMode( bool _developmentMode ) = 0;
        virtual void setPlatformName( const ConstString & _platformName ) = 0;

    public:
        virtual bool run( const String& _scriptInitParams ) = 0;

    public:
        virtual bool update() = 0;
        virtual void tick( float _timing ) = 0;
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
        virtual void setLanguagePack( const ConstString& _packName ) = 0;
        virtual const ConstString & getLanguagePack() const = 0;

    public:
        virtual bool loadConfigPaks( const TVectorResourcePackDesc & _resourcePack, const TVectorResourcePackDesc & _languagePack ) = 0;
        virtual bool applyConfigPaks() = 0;

    public:
        virtual bool loadPersonality( const ConstString & _module ) = 0;

    public:
        virtual void initializeRenderResources() = 0;
        virtual void finalizeRenderResources() = 0;

        virtual void onAppMouseLeave() = 0;
        virtual void onAppMouseEnter( const mt::vec2f & _point ) = 0;

        virtual void onFocus( bool _focus ) = 0;
        virtual void onFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void onFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void onRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) = 0;
        virtual bool onClose() = 0;

        virtual void onUserEvent( const ConstString & _event, const TMapParams & _params ) = 0;

        virtual void onTurnSound( bool _turn ) = 0;
    };

#   define GAME_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::GameServiceInterface>(serviceProvider))
}