#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/PlayerInterface.h"
#   include "Interface/CacheInterface.h"

#	include "Core/Params.h"

#	include "Core/Magic.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct DataDesc
	{
		ConstString category;
		FilePath path;
	};
	//////////////////////////////////////////////////////////////////////////
	DECLARE_MAGIC_NUMBER( MAGIC_GAME_DATA, 'G', 'M', 'D', '1', 1 );
	//////////////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////////////////
	class PakInterface
		: public FactorablePtr
	{
	public:
		virtual const ConstString & getName() const = 0;

	public:
		virtual void setPreload( bool _value ) = 0;
		virtual bool isPreload() const = 0;

		virtual void setLocale( const ConstString & _locale ) = 0;
		virtual const ConstString & getLocale() const = 0;

		virtual void setPlatfrom( const ConstString & _platform ) = 0;
		virtual const ConstString & getPlatfrom() const = 0;

		virtual void setPath( const ConstString & _path ) = 0;
		virtual const ConstString & getPath() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<PakInterface> PakInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
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
        virtual void tick( float _timing ) = 0;

	public:
        virtual void render() = 0;

    public:
        virtual void setCursorMode( bool _mode ) = 0;

    public:
        virtual float getTimingFactor() const = 0;
        virtual void setTimingFactor( float _timingFactor ) = 0;

    public:
        virtual const WString & getParam( const ConstString & _paramName ) const = 0;
        virtual bool hasParam( const ConstString & _paramName ) const = 0;

	public:
		virtual bool addData( const ConstString & _name, const DataDesc & _desc ) = 0;
		virtual bool hasData( const ConstString & _name ) const = 0;

		virtual CacheBufferID loadData( const ConstString & _name, const void ** _data, size_t & _size ) const = 0;
		virtual bool writeData( const ConstString & _name, const void * _data, size_t _size ) const = 0;

    public:
		virtual void createResourcePak( const ResourcePackDesc & _desc ) = 0;
		virtual PakInterfacePtr getResourcePak( const ConstString & _name ) const = 0;

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

	public:
		virtual void mousePosition( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseEnter( const InputMousePositionEvent & _event ) = 0;
		virtual void mouseLeave( const InputMousePositionEvent & _event ) = 0;
		
	public:
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