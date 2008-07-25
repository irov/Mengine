#	include "AtlasContainerCompiler.h"
#	include "Utils.h"
//////////////////////////////////////////////////////////////////////////
AtlasContainerCompiler::AtlasContainerCompiler(FILE * _log, int _width, int _height)
: m_width(_width)
, m_height(_height)
, m_log(_log)
{}
//////////////////////////////////////////////////////////////////////////
AtlasContainerCompiler::~AtlasContainerCompiler()
{
	for(std::vector<AtlasContainer*>::iterator it = m_atlasContainers.begin();
		it != m_atlasContainers.end();
		it++
		)
	{
		delete *it;
	}

	m_atlasContainers.clear();
}
//////////////////////////////////////////////////////////////////////////
const std::vector<std::string> & AtlasContainerCompiler::compileAtlas(std::map<std::string, Texture2D*> & textures, const std::string& _outputFilename )
{
	fprintf(m_log, "Process: Compiling atlases. \n");

	TNewFormatMap   formatMap;

	for(std::map<std::string, Texture2D*>::iterator it = textures.begin();
		it != textures.end(); ++it)
	{
		int bpp = it->second->getBPP();
		formatMap[bpp].push_back( it->second );
	}

	for (TNewFormatMap::iterator fmIter = formatMap.begin(); fmIter != formatMap.end(); ++fmIter)
	{
		int bpp = fmIter->first;

		AtlasContainer * atlasContainer
			= new AtlasContainer(m_log, bpp, m_width, m_height);

		atlasContainer->insertTextures(fmIter->second);

		m_atlasContainers.push_back(atlasContainer);
	}

	static std::vector<std::string>	output_names;
	output_names.clear();

	for(std::vector<AtlasContainer*>::iterator it = m_atlasContainers.begin();
		it != m_atlasContainers.end();
		it++
		)
	{
		AtlasContainer * atlasContainer = *it;
		
		static char buffer[65];
		int bpp = atlasContainer->getBPP();
		itoa(bpp, buffer, 10);

		std::string name = buffer + _outputFilename;
		atlasContainer->saveToFile(name);

		const std::vector<std::string> & names = atlasContainer->getAtlasesNames();
		std::copy(names.begin(),names.end(),std::back_inserter(output_names));
	}

	return output_names;
}