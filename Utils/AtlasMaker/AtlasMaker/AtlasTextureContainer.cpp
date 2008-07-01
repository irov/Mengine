#	include "AtlasTextureContainer.h"
#	include "Utils.h"

AtlasTextureContainer::AtlasTextureContainer(int _width, int _height)
: m_atlases(0)
, m_width(_width)
, m_height(_height)
{}

AtlasTextureContainer::~AtlasTextureContainer()
{
	delete[] m_atlases;
}

const std::vector<std::string>& AtlasTextureContainer::compileAtlas(std::map<std::string, MengeTexture2D*> & textures, const std::string& _outputFilename )
{
	TNewFormatMap   formatMap;

	//разбиваем текстуры по форматам
	for(std::map<std::string, MengeTexture2D*>::iterator it = textures.begin();
		it != textures.end(); ++it)
	{
		long fmt = (*it).second->getBPP();
		formatMap[fmt].push_back( (*it).second );
	}

	// каждый атлас различен по формату
	m_atlases = new TAtlasList[formatMap.size()];

	//расставляем текстуры по атласам
	int i = 0;
	for (TNewFormatMap::iterator fmIter = formatMap.begin(); fmIter != formatMap.end(); ++fmIter)
	{
		insertAtlas(i++, fmIter->first, fmIter->second);
	}

	static std::vector<std::string>	output_names;
	output_names.clear();

	int k = 0;
	for(int i = 0; i < formatMap.size(); i++)
	{
		for(std::list<AtlasTexture>::iterator it = m_atlases[i].begin(); it != m_atlases[i].end(); ++it)
		{
			AtlasTexture & Atlas = *it;

			static char buffer[65];
			itoa(k++, buffer, 10);

			std::string name = _outputFilename + buffer + ".mne";

			Atlas.writeToDisc(name);

			output_names.push_back(name);
		}
	}

	return output_names;
}

void AtlasTextureContainer::insertAtlas(int index, long bpp, const TTextureVector& textures)
{
	for (int i = 0; i < textures.size(); ++i)
	{
		MengeTexture2D & texture = *textures[i];

		std::list<AtlasTexture>::iterator it;

		for(it = m_atlases[index].begin(); it != m_atlases[index].end(); ++it)
		{
			AtlasTexture & Atlas = *it;

			if(Atlas.isFitting(texture) == false)
			{
				printf("Warning: Atlas width/height smaller than texture %s width/height \n", texture.getFilename());
				break;
			}

			bool result = Atlas.insertTexture(texture);

			if(result == true)
			{
				break;
			}
		}

		if(it == m_atlases[index].end())
		{
			printf("Warning: Can't insert texture into atlas, creating another atlas...\n");
			m_atlases[index].push_back( AtlasTexture(m_width, m_height, bpp) );

			AtlasTexture & Atlas = m_atlases[index].back();

			bool result = Atlas.insertTexture(texture);

			if(result == false)
			{
				printf("Error: Inserting texture into atlas failed! \n");
			}
		}
	}
}