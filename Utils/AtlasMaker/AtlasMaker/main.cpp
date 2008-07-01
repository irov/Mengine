#	include <windows.h>
#	include <commctrl.h>
#	include <commdlg.h>
#	include <basetsd.h>
#	include <stdio.h>
#	include <d3dx9.h>
#	include <algorithm>

#	include "pugixml.hpp"

#	include <fstream>
#	include <iostream>
#	include <list>

#	include <vector>
#	include <map>
#	include "RectangleAreaPacker.h"
#	include "MengeTexture.h"
#	include "AtlasTexture.h"
#	include "AtlasTextureContainer.h"
#	include "Utils.h"
#	include "ResourceParser.h"
#	include "FreeImage.h"

void ConvertImageDefaultToAtlas(ResourceParser * resourceParser, pugi::xml_node & resource)
{
	resource.attribute("Type").set_value("ResourceImageAtlas");

	for (pugi::xml_node file = resource.child("File"); 
		file; file = file.next_sibling("File"))
	{
		std::string value = resourceParser->getGameDirectory() + "/" + file.attribute("Path").value();

		MengeTexture2D * texture = resourceParser->getTexture(value);

		if(texture != NULL)
		{
			if(texture->getAtlas() != NULL)
			{
				std::string filename = texture->getAtlas()->getFilename();
				file.attribute("Path").set_value(filename.c_str());
			}

			pugi::xml_attribute uv = file.append_attribute("UV");

			std::string uv_value;

			MengeTexture2D::TextureDesc desc = texture->getDesc();

			uv_value+=Utility::convert_number_to_string(desc.u);
			uv_value+=";";
			uv_value+=Utility::convert_number_to_string(desc.v);
			uv_value+=";";
			uv_value+=Utility::convert_number_to_string(desc.w);
			uv_value+=";";
			uv_value+=Utility::convert_number_to_string(desc.z);

			uv.set_value(uv_value.c_str());

			pugi::xml_attribute offset = file.append_attribute("Offset");

			std::string offset_value;

			offset_value+=Utility::convert_number_to_string(desc.offsetX);
			offset_value+=";";
			offset_value+=Utility::convert_number_to_string(desc.offsetY);

			offset.set_value(offset_value.c_str());

			pugi::xml_attribute maxSize = file.append_attribute("MaxSize");

			std::string maxSize_value;

			maxSize_value+=Utility::convert_number_to_string(desc.maxSizeX);
			maxSize_value+=";";
			maxSize_value+=Utility::convert_number_to_string(desc.maxSizeY);

			maxSize.set_value(maxSize_value.c_str());

			pugi::xml_attribute size = file.append_attribute("Size");

			std::string size_value;

			size_value+=Utility::convert_number_to_string(desc.sizeX);
			size_value+=";";
			size_value+=Utility::convert_number_to_string(desc.sizeY);

			size.set_value(size_value.c_str());

			pugi::xml_attribute alpha = file.append_attribute("Alpha");

			if(texture->isAlphaChannel())
			{
				alpha.set_value("1");
			}
			else
			{
				alpha.set_value("0");
			}
		}
		else
		{
			printf("Warning: Texture %s not found for attribute Path \n", value.c_str());
		}
	}
}

int main( int argc, char **argv )
{
	FreeImage_Initialise(false);

	if(argc == 1)
	{
		return 0;
	}

	std::string infilename = argv[1];
	std::string outfilename = argv[2];

	int width = atoi(argv[3]);
	int height = atoi(argv[4]);

	std::string gamedir = argv[5];

	Utility::useHalfTexel = atoi(argv[6]);

	Utility::JPEGQuality = atoi(argv[7]); 

	ResourceParser resourceParser;

	resourceParser.setStandartAtlasWidth(width);
	resourceParser.setStandartAtlasHeight(height);
	resourceParser.setOutputName(outfilename);
	resourceParser.setGameDirectory(gamedir);

	resourceParser.addResourceConverter("ResourceImageDefault",ConvertImageDefaultToAtlas);
	resourceParser.loadTexturesFromResource(infilename);

	resourceParser.createAtlases();

	FreeImage_DeInitialise();

	return 0;
}