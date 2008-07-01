#	include "ResourceParser.h"

ResourceParser::ResourceParser()
{}

ResourceParser::~ResourceParser()
{
	for(std::map<std::string, MengeTexture2D*>::iterator it = m_textures.begin();
		it != m_textures.end(); ++it)
	{
		delete (*it).second;
	}
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

void ResourceParser::setOutputName(const std::string & _filename)
{
	m_outputName = _filename;
}

MengeTexture2D * ResourceParser::getTexture(const std::string & _texturename)
{
	std::map<std::string, MengeTexture2D * >::iterator it = m_textures.find(_texturename);
	if(it == m_textures.end())
	{
		return NULL;
	}

	return it->second;
}

bool ResourceParser::isResourceAllowed(const std::string & _resource)
{
	return m_resources.find(_resource) != m_resources.end();
}

void ResourceParser::loadTexturesFromResource(const std::string & _filename)
{
	printf("Process: Parsing %s \n",_filename.c_str());

	m_inputName = _filename;

	pugi::xml_document doc;

	doc.load_file(_filename.c_str());

	pugi::xml_node data_block = doc.child("DataBlock");

	for (pugi::xml_node resource = data_block.child("Resource"); resource; resource = resource.next_sibling("Resource"))
	{
		std::string resourceType = resource.attribute("Type").value();

		if(isResourceAllowed(resourceType))
		{
			for (pugi::xml_node file = resource.child("File"); file; file = file.next_sibling("File"))
			{
				std::string texturename = m_gamedir + "/" + file.attribute("Path").value();

				std::map<std::string, MengeTexture2D *>::iterator it = m_textures.find(texturename);

				if(it == m_textures.end())
				{
					MengeTexture2D * tex = new MengeTexture2D();

					if (tex->loadTexture(texturename) == false)
					{
						printf("Warning: Texture %s not found \n",tex->getFilename().c_str());
						delete tex;
					}
					else
					{
						m_textures.insert( std::make_pair( tex->getFilename(), tex) );
					}
				}
			}
		}
	}
}

void ResourceParser::createAtlases()
{
	AtlasTextureContainer atlasTextureContainer(m_widthAtlas, m_heightAtlas);
	std::vector<std::string> output_names = atlasTextureContainer.compileAtlas(m_textures, m_outputName);

	pugi::xml_document resource_file;

	resource_file.load_file(m_inputName.c_str());

	pugi::xml_node data_block = resource_file.child("DataBlock");

	for (pugi::xml_node resource = data_block.child("Resource"); resource; resource = resource.next_sibling("Resource"))
	{
		std::string resourceType = resource.attribute("Type").value();

		if(isResourceAllowed(resourceType))
		{
			std::map<std::string, ResourceType>::iterator it = m_resources.find(resourceType);
			it->second(this, resource);
		}
	}
		
	resource_file.save_file(m_outputName.c_str());

	pugi::xml_document result_file;

	result_file.append_child().set_name("resources");
	pugi::xml_node resource = result_file.child("resources");

	pugi::xml_node res = resource.append_child();
	res.set_name("resource");
	res.append_attribute("value") = m_outputName.c_str();

	for(std::vector<std::string>::iterator it = output_names.begin(); it != output_names.end(); ++it)
	{
		pugi::xml_node res = resource.append_child();
		res.set_name("resource");
		res.append_attribute("value") = (*it).c_str();
	}

	result_file.save_file("output.xml");
}