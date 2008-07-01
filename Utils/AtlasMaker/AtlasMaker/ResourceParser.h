#	pragma once
#	include <assert.h>
#	include <algorithm>
#	include "pugixml.hpp"

#	include "MengeTexture.h"
#	include "AtlasTexture.h"
#	include "AtlasTextureContainer.h"
#	include "Utils.h"
#	include <d3dx9.h>

class ResourceParser
{
public:
	typedef void (*ResourceType)(ResourceParser *, pugi::xml_node &);

	ResourceParser();
	~ResourceParser();
public:
	void setStandartAtlasWidth(int _width);
	void setStandartAtlasHeight(int _height);

	void setGameDirectory(const std::string & _gamedir);
	const std::string & getGameDirectory() const;

	void setOutputName(const std::string & _filename);

	MengeTexture2D * getTexture(const std::string & _texturename);

	void loadTexturesFromResource(const std::string & _filename);
	void createAtlases();

	void addResourceConverter(const std::string & _from, ResourceType _resourceType)
	{
		std::map<std::string, ResourceType>::iterator it = m_resources.find(_from);

		if(it != m_resources.end())
		{
			return;
		}

		m_resources.insert(std::make_pair(_from, _resourceType));
	}

private:

	std::map< std::string, ResourceType> m_resources;

	bool isResourceAllowed(const std::string & _resource);

	std::map<std::string, MengeTexture2D * > m_textures;
	std::string m_gamedir;
	std::string m_inputName;
	std::string m_outputName;
	int m_widthAtlas;
	int m_heightAtlas;
};