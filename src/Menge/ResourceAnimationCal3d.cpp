#	include "ResourceAnimationCal3d.h"

#	include "ResourceImplement.h"

#	include "FileEngine.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include <iostream>
#	include <fstream>

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceAnimationCal3d )
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimationCal3d::ResourceAnimationCal3d( const std::string & _name )
		: ResourceImpl( _name )
	{}
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
	const AnimInfo& ResourceAnimationCal3d::getAnimationInfo(const std::string& _name) const
	{
		int id = m_calCoreModel->getCoreAnimationId(_name);
		return m_animInfos[id];
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimationCal3d::_compile()
	{
		FileDataInterface * cal3dfile = 
			Holder<FileEngine>::hostage()->openFile(m_filename);
	
		m_calCoreModel = new CalCoreModel("dummy");

		std::string strBuffer;
		strBuffer.reserve(100);
		char c = 0;

		float scale = 1.0f;

		for(int line = 0; ;++line)
		{
			strBuffer.clear();

			while(!cal3dfile->eof())
			{
				cal3dfile->read_chars(&c,1);	
				if(c == '\n')
				{
					break;
				}
				strBuffer+=c;
			}

			std::string::size_type pos = strBuffer.find_first_not_of(" \t");

			if((pos == std::string::npos) || (strBuffer[pos] == '\n') || (strBuffer[pos] == '\r') || (strBuffer[pos] == 0)) continue;

			if(strBuffer[pos] == '#') continue;

			std::string strKey = strBuffer.substr(pos, strBuffer.find_first_of(" =\t\n\r", pos) - pos);
			pos += strKey.size();

			pos = strBuffer.find_first_not_of(" \t", pos);

			if((pos == std::string::npos) || (strBuffer[pos] != '='))
			{
				printf("%s (%d): Invalid syntax.", m_filename.c_str(),line);
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
				scale = (float)atof(strData.c_str());
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

			if(cal3dfile->eof()) 
			{
				break;
			}
		}

		Holder<FileEngine>::hostage()->closeFile(cal3dfile);

		m_calCoreModel->scale(scale);
		m_calCoreModel->getCoreSkeleton()->calculateBoundingBoxes(m_calCoreModel);

		_createMaterials();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimationCal3d::_release()
	{
		_freeMaterials();
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
	void ResourceAnimationCal3d::_createMaterials()
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

				RenderImageInterface * texture = Holder<RenderEngine>::hostage()->loadImage(td);

				pCoreMaterial->setMapUserData(mapId, (Cal::UserData)texture);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimationCal3d::_freeMaterials()
	{
		for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
		{
			CalCoreMaterial * pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

			for(int mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
			{
				Cal::UserData material = pCoreMaterial->getMapUserData(mapId);
				RenderImageInterface * tex = static_cast<RenderImageInterface*>(material);
				Holder<RenderEngine>::hostage()->releaseRenderImage(tex);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CalModel * ResourceAnimationCal3d::getNewInstance()
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
	const	TVecHardPoints & ResourceAnimationCal3d::getHardPoints() const
	{
		return m_hardPoints;
	}
	//////////////////////////////////////////////////////////////////////////
	const	CalCoreBone * ResourceAnimationCal3d::getCoreBone(int _index) const
	{
		return m_calCoreModel->getCoreSkeleton()->getCoreBone(_index);
	}
}