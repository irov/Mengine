#	include <vector>
#	include <map>
#	include <list>
#	include "RectangleAreaPacker.h"
#	include "Texture2D.h"
#	include "AtlasTexture.h"

class AtlasContainer
{
public:
	AtlasContainer(FILE * _log, int _bpp, int _width, int _height);
	~AtlasContainer();

public:
	void insertTextures(const std::vector<Texture2D*> & _textures);
	void saveToFile(const std::string & _outputFilename);
	const std::vector<std::string> & getAtlasesNames() const;
	int getBPP() const;

private:
	std::list<AtlasTexture>	m_atlasTextures;
	std::vector<std::string> m_atlasesNames;

	int m_bpp;
	int m_width;
	int m_height;
	FILE * m_log;
};