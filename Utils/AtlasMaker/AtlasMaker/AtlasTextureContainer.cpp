#	include "AtlasTextureContainer.h"
#	include "Utils.h"
//////////////////////////////////////////////////////////////////////////
AtlasTextureContainer::AtlasTextureContainer(FILE * _log, int _width, int _height)
: m_atlases(0)
, m_width(_width)
, m_height(_height)
, m_log(_log)
{}
//////////////////////////////////////////////////////////////////////////
AtlasTextureContainer::~AtlasTextureContainer()
{
	delete[] m_atlases;
}
//////////////////////////////////////////////////////////////////////////
const std::vector<std::string>& AtlasTextureContainer::compileAtlas(std::map<std::string, Texture2D*> & textures, const std::string& _outputFilename )
{
	fprintf(m_log, "Process: Compiling atlases. \n");
	TNewFormatMap   formatMap;

	//разбиваем текстуры по форматам
	for(std::map<std::string, Texture2D*>::iterator it = textures.begin();
		it != textures.end(); ++it)
	{
		int fmt = (*it).second->getBPP();
		formatMap[fmt].push_back( (*it).second );
	}

	// каждый атлас различен по формату
	m_atlases = new TAtlasList[formatMap.size()];

	//расставляем текстуры по атласам
	int i = 0;
	for (TNewFormatMap::iterator fmIter = formatMap.begin(); fmIter != formatMap.end(); ++fmIter)
	{
		fprintf(m_log, "Process: %d bpp found. \n", fmIter->first);
		_insertAtlas(i++, fmIter->first, fmIter->second);
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

			fprintf(m_log, "Process: Atlas %s written on a disc. \n", name.c_str());

			output_names.push_back(name);
		}
	}

	return output_names;
}
//////////////////////////////////////////////////////////////////////////
void AtlasTextureContainer::_insertAtlas(int index, int bpp, const TTextureVector& textures)
{
	for (int i = 0; i < textures.size(); ++i)
	{
		Texture2D & texture = *textures[i];

		std::list<AtlasTexture>::iterator it;

		for(it = m_atlases[index].begin(); it != m_atlases[index].end(); ++it)
		{
			AtlasTexture & Atlas = *it;

			if(Atlas.isFitting(texture) == false)
			{
				fprintf(m_log, "Warning: Atlas [%d;%d] smaller than texture %s [%d;%d] \n", 
					Atlas.getWidth(), Atlas.getHeight(), 
					texture.getFilename().c_str(), texture.getWidth(), texture.getHeight() );

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
			fprintf(m_log, "Warning: Can't insert texture into atlas, creating another atlas...\n");

			m_atlases[index].push_back( AtlasTexture( m_log, m_width, m_height, bpp ) );

			AtlasTexture & Atlas = m_atlases[index].back();

			Atlas.bake();

			bool result = Atlas.insertTexture(texture);

			if(result == false)
			{
				fprintf(m_log, "Error: Inserting texture into atlas failed! \n");
			}
			else
			{
				fprintf(m_log, "Process: Texture inserted. \n");
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////