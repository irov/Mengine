#	pragma once
#	include <assert.h>
#	include <algorithm>
#	include "pugixml.hpp"

#	include "Texture2D.h"
#	include "AtlasTexture.h"
#	include "AtlasContainerCompiler.h"
#	include "Utils.h"
#	include <d3dx9.h>

class ResourceParser
{
public:

	typedef void (*ResourceType)(ResourceParser *, pugi::xml_node &);

	ResourceParser();
	~ResourceParser();

public:

	void execute( const std::string & _input );

public:

	void setStandartAtlasWidth(int _width);
	void setStandartAtlasHeight(int _height);
	void setGameDirectory(const std::string & _gamedir);
	const std::string & getGameDirectory() const;

	Texture2D * getTexture(const std::string & _texturename);

	void addResourceCallback(const std::string & _from, ResourceType _resourceType);
	std::string getTextureLocation( const std::string & _name );

private:

	void	_loadResourceLocations( const std::string & _input );
	void	_loadTexturesFromResource( const std::string & _filename );
	void	_saveModifiedResource( const std::string & _input );
	void	_saveResultsToFile( const std::vector<std::string> & _names );
	bool	_isResourceAllowed( const std::string & _resource );

	typedef std::vector<std::string> VectorResources;
	typedef std::map<std::string, ResourceType> MapCallbacks;
	typedef std::map<std::string, Texture2D*> MapTextures;

	VectorResources m_resources;
	VectorResources m_outputResourceNames;

	MapCallbacks m_resourceCallbacks;
	MapTextures m_textures;

	std::string m_gamedir;
	std::string m_inputName;
	std::string m_outputName;
	int m_widthAtlas;
	int m_heightAtlas;

	FILE * m_log;
};