#	include "AtlasContainer.h"
//////////////////////////////////////////////////////////////////////////
AtlasContainer::AtlasContainer(FILE * _log, int _bpp, int _width, int _height)
: m_log(_log)
, m_bpp(_bpp)
, m_width(_width)
, m_height(_height)
{}
//////////////////////////////////////////////////////////////////////////
AtlasContainer::~AtlasContainer()
{
	m_atlasTextures.clear();
}
//////////////////////////////////////////////////////////////////////////
int AtlasContainer::getBPP() const
{
	return m_bpp;
}
//////////////////////////////////////////////////////////////////////////
void AtlasContainer::insertTextures(const std::vector<Texture2D*> & _textures)
{
	for (int i = 0; i < _textures.size(); ++i)
	{
		Texture2D & texture = *_textures[i];

		std::list<AtlasTexture>::iterator it;

		for(it = m_atlasTextures.begin(); it != m_atlasTextures.end(); ++it)
		{
			AtlasTexture & Atlas = *it;

			if( _isFitting(Atlas, texture) == false )
			{
				fprintf(m_log, "Warning: Atlas [%d;%d] smaller than texture %s [%d;%d] \n", 
					Atlas.getWidth(), Atlas.getHeight(), 
					texture.getFilename().c_str(), 
					texture.getWidth(), texture.getHeight() );

				break;
			}

			bool result = Atlas.insertTexture(texture);

			if(result == true)
			{
				break;
			}
		}

		if(it == m_atlasTextures.end())
		{
			fprintf(m_log, "Warning: Can't insert texture into atlas, creating another atlas...\n");

			m_atlasTextures.push_back( AtlasTexture( m_log, m_width, m_height, m_bpp ) );

			AtlasTexture & Atlas = m_atlasTextures.back();

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
void AtlasContainer::saveToFile( const std::string & _outputFilename )
{
	m_atlasesNames.clear();
	int k = 0;
	for(std::list<AtlasTexture>::iterator it = m_atlasTextures.begin(); it != m_atlasTextures.end(); ++it)
	{
		AtlasTexture & Atlas = *it;

		static char buffer[65];
		itoa(k++, buffer, 10);

		std::string name = _outputFilename + buffer + ".mne";
		Atlas.writeToDisc(name);
		m_atlasesNames.push_back(name);

		fprintf(m_log, "Process: Atlas %s written on a disc. \n", name.c_str());
	}
}
//////////////////////////////////////////////////////////////////////////
const std::vector<std::string> & AtlasContainer::getAtlasesNames() const
{
	return m_atlasesNames;
}
//////////////////////////////////////////////////////////////////////////
bool AtlasContainer::_isFitting(const AtlasTexture & _atlas, const Texture2D & _texture)
{
	return ( _texture.getWidth() + _texture.getBorder() <= _atlas.getWidth() ) && ( _texture.getHeight() + _texture.getBorder() <= _atlas.getHeight() );
}
//////////////////////////////////////////////////////////////////////////