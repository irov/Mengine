#	pragma once

#	include "ResourceManager.h"

#	include "InputHandler.h"
#	include "Eventable.h"
#	include "math/vec3.h"

#	include <list>
#	include <map>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

class XmlElement;

namespace Menge
{
	class Player;

	class Scene;
	class Arrow;

	class Amplifier;

	class Game
		: public InputHandler
		, public Eventable
		, public ResourceManagerListener
	{
	public:
		Game();
		~Game();

	public:
		void update( float _timing );
		void render( bool _enableDebug = false );

	public:
		bool init();
		void release();
		void debugRender();

	public:
		void setParamString( const std::string& _params );

	public:
		std::string getPathEntities( const std::string& _entity ) const;
//		const std::string & getPathScenes() const;
//		const std::string & getPathArrows() const;

	public:
		Arrow * getArrow( const std::string & _name );
		bool loadArrow( const std::string & _name, const std::string & _path );
		void removeArrow( const std::string & _name );

		Arrow * getDefaultArrow();

	public:
		Scene * getScene(const std::string & _name );
		void destroyScene( const std::string & _name );

	public:
		void loader( XmlElement * _xml );
		void loaderGame_( XmlElement * _xml );
		void loaderScenes_( XmlElement * _xml );
		void loaderArrows_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );
		void loaderResources_( XmlElement * _xml );
		void loaderDefault_( XmlElement * _xml );
		void loaderPersonality_( XmlElement * _xml );

		void readResourceFile( const std::string& _file );
		void loaderResourceFile( XmlElement * _xml );
		void loaderResourceFile_( XmlElement * _xml );
		
	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		void handleMouseLeave();
		void handleMouseEnter();

		void onResourceLoaded( const std::string& _name ) override;
		void onResourceUnLoaded() override;
		
	public:
		const mt::vec2f & getResourceResolution() const;
		const std::string& getTitle() const;
		bool isContentResolutionFixed() const;
		float getWidth() const;
		float getHeight() const;
		int getBits() const;
		bool getFullscreen() const;
		bool getVSync() const;
		const std::string& getRenderDriverName() const;
		const std::string& getPhysicSystemName() const;
		bool getTextureFiltering() const;
		int getFSAAType() const;
		int getFSAAQuality() const;

	protected:

		Player* m_player;
		std::string m_title;
		mt::vec2f	m_resourceResolution;
		bool m_fixedContentResolution;

		std::string m_defaultArrowName;

		std::string m_personality;

		std::string m_eventInit;
		std::string m_eventUpdate;
		std::string m_eventFini;

		PyObject * m_pyPersonality;
		Arrow * m_defaultArrow;

		typedef std::map<std::string, Arrow*> TMapArrow;
		TMapArrow m_mapArrow;

		typedef std::map<std::string, Scene*> TMapScene;
		TMapScene m_mapScene;

		typedef std::pair<std::string, std::string> TPairDeclaration;
		typedef std::list< TPairDeclaration > TListDeclaration;
		typedef std::map<std::string, TPairDeclaration > TMapDeclaration;
		TMapDeclaration m_mapEntitiesDeclaration;
		TMapDeclaration m_mapArrowsDeclaration;
		TMapDeclaration m_mapScenesDeclaration;
		TMapDeclaration m_mapResourceDeclaration;
		TListDeclaration m_listResourceLocation;
		
		//std::string m_pathResource;
		//std::string m_pathScripts;
		//std::string m_pathEntities;
		//std::string m_pathScenes;
		std::string m_currentResourcePath;
		TListDeclaration m_pathResource;
		TListDeclaration m_pathScripts;
		TListDeclaration m_pathEntities;
		TListDeclaration m_pathScenes;
		TListDeclaration m_pathArrows;
		//std::string m_pathArrows;
		float m_width;
		float m_height;
		int m_bits;
		bool m_fullScreen;
		bool m_vsync;
		bool m_textureFiltering;
		int	m_FSAAType;
		int m_FSAAQuality;
		std::string m_renderDriver;
		std::string m_physicSystemName;

	};	
}
