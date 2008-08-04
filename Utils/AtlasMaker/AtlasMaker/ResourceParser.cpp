#	include "ResourceParser.h"

ResourceParser::ResourceParser()
	: m_log(0)
	, m_outputName("atlas")
{
	m_log = fopen("am_log.txt","w");
}

ResourceParser::~ResourceParser()
{
	for(MapTextures::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		delete (*it).second;
	}

	fclose(m_log);
}

void ResourceParser::setStandartAtlasWidth(int _width)
{
	m_widthAtlas = _width;
}

void ResourceParser::setStandartAtlasHeight(int _height)
{
	m_heightAtlas = _height;
}

void ResourceParser::setGameDirectory(const std::string & _gamedir)
{
	m_gamedir = _gamedir;
}

const std::string & ResourceParser::getGameDirectory() const
{
	return m_gamedir;
}

Texture2D * ResourceParser::getTexture(const std::string & _texturename)
{
	MapTextures::iterator it = m_textures.find(_texturename);

	if(it == m_textures.end())
	{
		return NULL;
	}

	return it->second;
}

bool ResourceParser::_isResourceAllowed(const std::string & _resource)
{
	return m_resourceCallbacks.find(_resource) != m_resourceCallbacks.end();
}

void ResourceParser::addResourceCallback(const std::string & _from, ResourceType _resourceType)
{
	MapCallbacks::iterator it = m_resourceCallbacks.find(_from);

	if(it != m_resourceCallbacks.end())
	{
		return;
	}

	m_resourceCallbacks.insert(std::make_pair(_from, _resourceType));
}

void ResourceParser::execute( const std::string & _input )
{
	FreeImage_Initialise(false);

	fprintf(m_log, "Open input file: %s \n", _input.c_str());

	_loadResourceLocations(_input);

	for(VectorResources::iterator it = m_resources.begin(); it != m_resources.end(); ++it)
	{
		_loadTexturesFromResource(*it);
	}

	AtlasContainerCompiler atlasContainerCompiler( m_log, m_widthAtlas, m_heightAtlas );

	std::vector<std::string> output_atlasnames = atlasContainerCompiler.compileAtlas(m_textures, m_outputName);

	for(VectorResources::iterator it = m_resources.begin(); it != m_resources.end(); ++it)
	{
		_saveModifiedResource(*it);
	}

	_saveResultsToFile(output_atlasnames);

	FreeImage_DeInitialise();
}

void ResourceParser::_saveResultsToFile(const std::vector<std::string> & _names)
{
	fprintf(m_log, "Process: Save results to output.xml\n");

	pugi::xml_document result_file;

	result_file.append_child().set_name("datapack");
	pugi::xml_node datapack = result_file.child("datapack");

	datapack.append_child().set_name("atlases");
	pugi::xml_node atlas = datapack.child("atlases");

	for(VectorResources::const_iterator it = _names.begin(); it != _names.end(); ++it)
	{
		pugi::xml_node res = atlas.append_child();
		res.set_name("atlas");
		res.append_attribute("value") = (*it).c_str();
	}

	datapack.append_child().set_name("resources");
	pugi::xml_node resource = datapack.child("resources");

	for(VectorResources::const_iterator it = m_outputResourceNames.begin(); it != m_outputResourceNames.end(); ++it)
	{
		pugi::xml_node res = resource.append_child();
		res.set_name("resource");
		res.append_attribute("value") = (*it).c_str();
	}

	result_file.save_file("output.xml");
}

void ResourceParser::_saveModifiedResource( const std::string & _input )
{
	pugi::xml_document resource_file;

	resource_file.load_file(_input.c_str());

	pugi::xml_node data_block = resource_file.child("DataBlock");

	for (pugi::xml_node resource = data_block.child("Resource"); resource; resource = resource.next_sibling("Resource"))
	{
		std::string resourceType = resource.attribute("Type").value();

		if(_isResourceAllowed(resourceType))
		{
			MapCallbacks::iterator it = m_resourceCallbacks.find(resourceType);
			it->second(this, resource);
		}
	}

	std::string outputname = _input.substr(_input.find_last_of("\\") + 1, _input.size() - 1);

	m_outputResourceNames.push_back(outputname);
		
	resource_file.save_file(outputname.c_str());
}

void ResourceParser::_loadResourceLocations( const std::string & _input )
{
	pugi::xml_document doc;

	doc.load_file(_input.c_str());

	pugi::xml_node data_block = doc.child("resources");

	for (pugi::xml_node resource = data_block.child("resource"); resource; resource = resource.next_sibling("resource"))
	{
		std::string resourcename = resource.attribute("value").value();
		m_resources.push_back(resourcename);
	}
}

int compare_area(const Texture2D* s1, const Texture2D * s2)
{
	return s1->getHeight() * s1->getWidth() < s2->getHeight() * s2->getWidth();
}

int compare_height(const Texture2D* s1, const Texture2D * s2)
{
	if (   (s1->getHeight() > s2->getHeight())
             && (s1->getHeight()*s1->getWidth() == s2->getHeight()*s2->getWidth()))
	{
        return true;
	}

	return false;
}

void ResourceParser::_loadTexturesFromResource(const std::string & _filename)
{
	fprintf(m_log, "Process: Parsing %s \n",_filename.c_str());

	m_inputName = _filename;

	pugi::xml_document doc;

	doc.load_file(_filename.c_str());

	pugi::xml_node data_block = doc.child("DataBlock");

	static std::vector<std::string>	values(100);
	values.clear();

	for (pugi::xml_node resource = data_block.child("Resource"); resource; resource = resource.next_sibling("Resource"))
	{
		if(_isResourceAllowed(resource.attribute("Type").value()))
		{
			for (pugi::xml_node file = resource.child("File"); file; file = file.next_sibling("File"))
			{
				//std::string fullname = getTextureLocation(file.attribute("Path").value());
				values.push_back(file.attribute("Path").value());

			/*	if( getTexture(fullname) != NULL ) continue; 
				
				Texture2D * texture = new Texture2D();

				if (texture->loadTexture(fullname) == false)
				{
					printf("Warning: Texture %s not found \n", texture->getFilename().c_str());
					delete texture;
				}
				else
				{
					m_textures.insert( std::make_pair( texture->getFilename(), texture) );
				}*/
			}
		}
	}

	static std::vector<std::string>	unique_textures(100);
	unique_textures.clear();

	std::unique_copy(values.begin(),values.end(),std::back_inserter(unique_textures));
	values.swap(unique_textures);

	std::vector<Texture2D *> textures;

	for (int i = 0; i < values.size(); ++i)
	{
		std::string val = getTextureLocation(values[i]);
		Texture2D * tex = new Texture2D();

		if (tex->loadTexture(val) == false)
		{
			printf("texture %s not found \n",tex->getFilename().c_str());
			delete tex;
		}
		else
		{
			textures.push_back(tex);
		}
	}
	//sort by area.
	std::sort(textures.begin(),textures.end(),compare_area);
	//sort by height.
	std::sort(textures.begin(),textures.end(),compare_height);

	for (int i = 0; i < textures.size(); ++i)
	{
		Texture2D & texture = *textures[i];

		m_textures.insert(
			std::make_pair(texture.getFilename(),&texture)
			);
	}

	textures.clear();
}

std::string ResourceParser::getTextureLocation(const std::string & _name)
{
	return m_gamedir + "/" + _name;
}