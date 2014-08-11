#	include "Cal3dCoreModel.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Cal3dCoreModel::Cal3dCoreModel()
		: m_calCoreModel(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Cal3dCoreModel::~Cal3dCoreModel()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dCoreModel::setResourceSkeleton( ResourceCal3dSkeleton * _resource )
	{
		m_resourceSkeleton = _resource;
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dCoreModel::addResourceMesh( const ConstString & _name, ResourceCal3dMesh * _resource )
	{
		Cal3dMeshDesc desc;
		desc.resource = _resource;
		desc.meshId = -1;

		m_resourceMeshes.insert( std::make_pair(_name, desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dCoreModel::addResourceAnimation( const ConstString & _name, ResourceCal3dAnimation * _resource )
	{
		Cal3dAnimationDesc desc;
		desc.resource = _resource;
		desc.animationId = -1;

		m_resourceAnimations.insert( std::make_pair(_name, desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	int Cal3dCoreModel::getMeshId( const ConstString & _name ) const
	{
		TMapResourceHolderMesh::const_iterator it_found = m_resourceMeshes.find( _name );

		const Cal3dMeshDesc & desc = it_found->second;

		return desc.meshId;
	}
	//////////////////////////////////////////////////////////////////////////
	int Cal3dCoreModel::getAnimationId( const ConstString & _name ) const
	{
		TMapResourceHolderAnimations::const_iterator it_found = m_resourceAnimations.find( _name );

		const Cal3dAnimationDesc & desc = it_found->second;

		return desc.animationId;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Cal3dCoreModel::compile()
	{
		if( m_resourceSkeleton.compile() == false )
		{
			return false;
		}

		for( TMapResourceHolderMesh::iterator
			it = m_resourceMeshes.begin(),
			it_end = m_resourceMeshes.end();
		it != it_end;
		++it )
		{
			Cal3dMeshDesc & desc = it->second;

			if( desc.resource.compile() == false )
			{
				return false;
			}
		}

		for( TMapResourceHolderAnimations::iterator
			it = m_resourceAnimations.begin(),
			it_end = m_resourceAnimations.end();
		it != it_end;
		++it )
		{
			Cal3dAnimationDesc & desc = it->second;

			if( desc.resource.compile() == false )
			{
				return false;
			}
		}

		m_calCoreModel = new CalCoreModel("dummy");
		m_calCoreModel->setUserData( this );

		const CalCoreSkeletonPtr & skeleton = m_resourceSkeleton->getSkeleton();

		m_calCoreModel->setCoreSkeleton( skeleton.get() );

		for( TMapResourceHolderMesh::iterator
			it = m_resourceMeshes.begin(),
			it_end = m_resourceMeshes.end();
		it != it_end;
		++it )
		{
			Cal3dMeshDesc & desc = it->second;

			const CalCoreMeshPtr & mesh = desc.resource->getMesh();

			desc.meshId = m_calCoreModel->addCoreMesh( mesh.get() );
		}

		for( TMapResourceHolderAnimations::iterator
			it = m_resourceAnimations.begin(),
			it_end = m_resourceAnimations.end();
		it != it_end;
		++it )
		{
			Cal3dAnimationDesc & desc = it->second;

			const CalCoreAnimationPtr & animation = desc.resource->getAnimation();

			desc.animationId = m_calCoreModel->addCoreAnimation( animation.get() );
		}

		// load all textures and store the opengl texture id in the corresponding map in the material
		int materialCount = m_calCoreModel->getCoreMaterialCount();

		for( int materialId = 0; materialId != materialCount; ++materialId )
		{
			// get the core material
			CalCoreMaterial * pCoreMaterial = m_calCoreModel->getCoreMaterial( materialId );

			// loop through all maps of the core material
			int mapCount = pCoreMaterial->getMapCount();

			for( int mapId = 0; mapId != mapCount; ++mapId )
			{
				// get the filename of the texture
				const std::string & strFilename = pCoreMaterial->getMapFilename( mapId );

				printf("%s", strFilename.c_str());
				// load the texture from the file
				//GLuint textureId;
				//textureId = loadTexture(strPath + strFilename);

				//// store the opengl texture id in the user data of the map
				//pCoreMaterial->setMapUserData(mapId, (Cal::UserData)textureId);
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dCoreModel::release()
	{
		m_resourceSkeleton.release();

		for( TMapResourceHolderMesh::iterator
			it = m_resourceMeshes.begin(),
			it_end = m_resourceMeshes.end();
		it != it_end;
		++it )
		{
			Cal3dMeshDesc & desc = it->second;

			desc.resource.release();
		}

		for( TMapResourceHolderAnimations::iterator
			it = m_resourceAnimations.begin(),
			it_end = m_resourceAnimations.end();
		it != it_end;
		++it )
		{
			Cal3dAnimationDesc & desc = it->second;

			desc.resource.release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CalModel * Cal3dCoreModel::createModel()
	{
		CalModel * model = new CalModel(m_calCoreModel);

		return model;
	}
	//////////////////////////////////////////////////////////////////////////
	void Cal3dCoreModel::destroyModel( CalModel * _model )
	{
		delete _model;
	}
}