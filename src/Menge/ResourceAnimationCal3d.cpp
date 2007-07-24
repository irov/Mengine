#	include "ResourceAnimationCal3d.h"

#	include "ResourceImplement.h"

#	include "FileEngine.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include <iostream>
#	include <fstream>

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceAnimationCal3d )
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimationCal3d::ResourceAnimationCal3d( const std::string & _name )
		: ResourceImpl( _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimationCal3d::loader( TiXmlElement * _xml )
	{
		AnimInfo sq;
		std::string hardPointName;

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Config", m_filename );
				
			XML_CHECK_NODE_FOR_EACH( "Sequences" )
			{
				XML_CHECK_NODE( "AnimInfo" )
				{
					XML_VALUE_ATTRIBUTE( "Index", sq.index );
					XML_VALUE_ATTRIBUTE( "Delay", sq.delay );
					XML_VALUE_ATTRIBUTE( "Blend", sq.blend );

					m_animInfos.push_back( sq );
				}
			}	

			XML_CHECK_NODE_FOR_EACH( "HardPoints" )
			{
				XML_CHECK_NODE( "HardPoint" )
				{
					XML_VALUE_ATTRIBUTE( "Name", hardPointName );
			
					m_hardPoints.push_back( hardPointName );
				}
			}	
		}

		ResourceImpl::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	AnimInfo * ResourceAnimationCal3d::getAnimationInfo(const std::string& _name)
	{
		int id = m_calCoreModel->getCoreAnimationId(_name);
		AnimInfo * seq = &m_animInfos[id];
		return seq;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimationCal3d::_compile()
	{
		std::ifstream file;

		file.open(m_filename.c_str(), std::ios::in | std::ios::binary);

		if(!file)
		{
			assert(!"Failed to open model configuration file '");
			return false;
		}

		m_calCoreModel = new CalCoreModel("dummy");

		for(int line = 1; ; line++)
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

			std::string strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
			pos += strKey.size();

			pos = strBuffer.find_first_not_of(" \t", pos);

			if((pos == std::string::npos) || (strBuffer[pos] != '='))
			{
				std::cout << m_filename << "(" << line << "): Invalid syntax." << std::endl;
				return false;
			}

			pos = strBuffer.find_first_not_of(" \t", pos + 1);

			std::string strData;
			strData = strBuffer.substr(pos, strBuffer.find_first_of("\n\r", pos) - pos);

			if(strKey == "path")
			{
				 m_folder = strData;
			}
			if(strKey == "scale")
			{
				m_scale = (float)atof(strData.c_str());
			}
			else if(strKey == "skeleton")
			{
				const std::string& info = m_folder + strData;
				if(!m_calCoreModel->loadCoreSkeleton(info))
				{
					return false;
				}
			}
			else if(strKey == "animation")
			{
				const std::string& info = m_folder + strData;
				if(m_calCoreModel->loadCoreAnimation(info) == -1)
				{
					return false;
				}
				static int i = 0;
				m_calCoreModel->addAnimationName(strData,i++);
			}
			else if(strKey == "mesh")
			{
				const std::string& info = m_folder + strData;
				if(m_calCoreModel->loadCoreMesh(info) == -1)
				{
					return false;
				}
			}
			else if(strKey == "material")
			{
				const std::string& info = m_folder + strData;
				if(m_calCoreModel->loadCoreMaterial(info) == -1)
				{
					return false;
				}
			}
		}
		file.close();

		m_calCoreModel->scale(m_scale);
		m_calCoreModel->getCoreSkeleton()->calculateBoundingBoxes(m_calCoreModel);

		createMaterials();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimationCal3d::_release()
	{
		freeMaterials();
		delete m_calCoreModel;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimationCal3d::getAnimationCount() const
	{
		return m_calCoreModel->getCoreAnimationCount();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimationCal3d::getMeshCount() const
	{
		return m_calCoreModel->getCoreMeshCount();
	}
	//////////////////////////////////////////////////////////////////////////
	int	ResourceAnimationCal3d::getAnimationId(const std::string& _name) const
	{
		return m_calCoreModel->getCoreAnimationId(_name);
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceAnimationCal3d::getBoneIndex(const std::string& _bonename) const
	{
		return m_calCoreModel->getCoreSkeleton()->getCoreBoneId(_bonename);
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimationCal3d::createMaterials()
	{
		for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
		{
			m_calCoreModel->createCoreMaterialThread(materialId);
			m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
		}

		for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
		{
			CalCoreMaterial * pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

			for(int mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
			{
				std::string strFilename = pCoreMaterial->getMapFilename(mapId);

				FileDataInterface * imageData = Holder<FileEngine>::hostage()->openFile(m_folder + strFilename);

				TextureDesc td;
				
				td.buffer = (void*)imageData->getBuffer();
				td.size = imageData->size();
				td.filter = 1;

				Texture * texture = Holder<RenderEngine>::hostage()->createTextureInMemory(td);

				pCoreMaterial->setMapUserData(mapId, (Cal::UserData)texture);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimationCal3d::freeMaterials()
	{
		for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
		{
			CalCoreMaterial * pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

			for(int mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
			{
				Texture* tex = (Texture*)pCoreMaterial->getMapUserData(mapId);
				Holder<RenderEngine>::hostage()->releaseTexture(tex);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CalModel * ResourceAnimationCal3d::createInstance()
	{
		CalModel * model = new CalModel(m_calCoreModel);
		return model;
	}
	//////////////////////////////////////////////////////////////////////////
	CalCoreAnimation * ResourceAnimationCal3d::getCoreAnimation(int _index)
	{
		CalCoreAnimation * calCoreAnimation = m_calCoreModel->getCoreAnimation(_index);
		return calCoreAnimation;
	}
	//////////////////////////////////////////////////////////////////////////
	float	ResourceAnimationCal3d::getScale() const
	{
		return m_scale;
	}
	//////////////////////////////////////////////////////////////////////////
	const	TVecHardPoints & ResourceAnimationCal3d::getHardPoints() const
	{
		return m_hardPoints;
	}
}