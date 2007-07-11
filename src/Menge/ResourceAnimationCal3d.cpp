#	include "ResourceAnimationCal3d.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "XmlParser.h"

#	include <iostream>
#	include <fstream>

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceAnimationCal3d )

	ResourceAnimationCal3d::ResourceAnimationCal3d( const std::string & _name )
		: ResourceImpl( _name )
	{
	}

	void ResourceAnimationCal3d::loader( TiXmlElement * _xml )
	{
		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_fileCAL3D );
		}

		ResourceImpl::loader( _xml );
	}

	bool ResourceAnimationCal3d::_compile()
	{
		std::ifstream file;

		file.open(m_fileCAL3D.c_str(), std::ios::in | std::ios::binary);
		if(!file)
		{
			assert(!"Failed to open model configuration file '");
			return false;
		}

		int line;

		for(line = 1; ; line++)
		{
			std::string strBuffer;
			std::getline(file, strBuffer);

			if(file.eof()) break;

			if(!file)
			{
				assert(0);
				return false;
			}

			std::string::size_type pos;
			pos = strBuffer.find_first_not_of(" \t");

			if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

			if(strBuffer[pos] == '#') continue;

			std::string strKey;
			strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
			pos += strKey.size();

			pos = strBuffer.find_first_not_of(" \t", pos);

			if((pos == std::string::npos) || (strBuffer[pos] != '='))
			{
				std::cout << m_fileCAL3D << "(" << line << "): Invalid syntax." << std::endl;
				return false;
			}

			pos = strBuffer.find_first_not_of(" \t", pos + 1);

			std::string strData;
			strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

			if(strKey == "scale")
			{
				m_scale = atof(strData.c_str());
			}
			else if(strKey == "skeleton")
			{
				m_sceletonInfo.push_back(strData);
			}
			else if(strKey == "animation")
			{
				m_coreAnimInfo.push_back(strData);
			}
			else if(strKey == "mesh")
			{
				m_coreMeshInfo.push_back(strData);
			}
			else if(strKey == "material")
			{
				m_coreMatInfo.push_back(strData);
			}
		}
		file.close();
		return true;
	}

	float ResourceAnimationCal3d::getScale()
	{
		return m_scale;
	}

	const std::vector<std::string>& ResourceAnimationCal3d::getSceletonInfo()
	{
		return m_sceletonInfo;
	}

	const std::vector<std::string>& ResourceAnimationCal3d::getCoreAnimInfo()
	{
		return m_coreAnimInfo;
	}

	const std::vector<std::string>& ResourceAnimationCal3d::getCoreMeshInfo()
	{
		return m_coreMeshInfo;
	}	

	const std::vector<std::string>& ResourceAnimationCal3d::getCoreMatInfo()
	{
		return m_coreMatInfo;
	}

	void ResourceAnimationCal3d::_release()
	{
	}
}