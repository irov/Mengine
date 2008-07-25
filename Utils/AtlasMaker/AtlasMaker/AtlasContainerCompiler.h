#	pragma once
#	include <vector>
#	include <map>
#	include <list>
#	include "RectangleAreaPacker.h"
#	include "Texture2D.h"
#	include "AtlasTexture.h"
#	include "AtlasContainer.h"

typedef std::vector<Texture2D*> TTextureVector;
typedef std::map <int, TTextureVector> TNewFormatMap;
typedef std::list<AtlasTexture> TAtlasList;

class AtlasContainerCompiler
{
public:
	AtlasContainerCompiler(FILE * _log, int _width, int _height);
	~AtlasContainerCompiler();
public:
	const std::vector<std::string>& compileAtlas(
		std::map<std::string, Texture2D*> & _textures, 
		const std::string& _outputFilename );

private:

	std::vector<AtlasContainer*> m_atlasContainers;

	FILE * m_log;
	int m_width;
	int m_height;
};