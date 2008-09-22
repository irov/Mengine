#	include "ResourceParser.h"

void ConvertImageDefaultToAtlas(ResourceParser * resourceParser, pugi::xml_node & resource)
{
	pugi::xml_attribute atr = resource.attribute("Restricted");

	if(atr.as_bool() == true)
		return;

	bool result = true;

	for (pugi::xml_node file = resource.child("File"); 
		file; file = file.next_sibling("File"))
	{
		std::string value = resourceParser->getTextureLocation(file.attribute("Path").value());

		Texture2D * texture = resourceParser->getTexture(value);

		if(texture != NULL)
		{
			AtlasTexture * atlas = texture->getAtlas();

			if(atlas == NULL)
			{
			//	result = false;
			}

			if(atlas != NULL)
			{
				std::string filename = texture->getAtlas()->getFilename();
				file.attribute("Path").set_value(filename.c_str());

			//fprintf(resourceParser->getLog(), "Process: value = %s \n", value.c_str());
			
				const TextureAtlasDesc & desc = atlas->getTextureDesc(value);

				pugi::xml_attribute uv = file.append_attribute("UV");

				std::string uv_value;

				//Texture2D::TextureDesc desc = texture->getDesc();

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
		}
		else
		{
			printf("Warning: Texture %s not found for attribute Path \n", value.c_str());
		}
	}

	if(result == true)
	{
		resource.attribute("Type").set_value("ResourceImageAtlas");
	}

}

int main( int argc, char **argv )
{
	if(argc == 1)
	{
		return 0;
	}

	std::string infilename = argv[1];
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	std::string gamedir = argv[4];
	Utility::JPEGQuality = atoi(argv[5]);
	Utility::AlphaMin = atoi(argv[6]);

	ResourceParser resourceParser;

	resourceParser.setStandartAtlasWidth(width);
	resourceParser.setStandartAtlasHeight(height);
	resourceParser.setGameDirectory(gamedir);
	resourceParser.addResourceCallback("ResourceImageDefault",ConvertImageDefaultToAtlas);

	resourceParser.execute(infilename);
	return 0;
}