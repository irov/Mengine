#	include "Mesh.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Viewport.h"

#	include "tinyxml/tinyxml.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/FileSystemInterface.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "ResourceAnimationCal3d.h"

#	include "math/bv.h"

#	include "FileEngine.h"

using namespace Menge;

struct Vertex
{
	float pos[3];
	float normal[3];
	float t;
	float s;
};

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Mesh);
//////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
: m_scale(1.0f)
, m_leftAnimationTime(0)
, m_blendTime(0.3f)
, m_currentAnimationId(0)
, m_calCoreModel(0)
, m_calModel(0)
{}
//////////////////////////////////////////////////////////////////////////
Mesh::~Mesh()
{}
///////////////////////////////////////////////////////////////////////////
bool Mesh::isVisible( const Frustum & _frustum )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
void Mesh::initGeometry()
{
	m_primitiveData = new PrimitiveData();

	m_vertexDecl = Holder<RenderEngine>::hostage()->createVertexDeclaration();

	IndexData * indexData = Holder<RenderEngine>::hostage()->createIndexData();
	VertexData * vertexData = Holder<RenderEngine>::hostage()->createVertexData();

	m_primitiveData->setIndexData(indexData);
	m_primitiveData->setVertexData(vertexData);

	m_vertexDecl->insert(0, 0, DECLFLOAT3, DECLPOS, 0);
	m_vertexDecl->insert(0, 12, DECLFLOAT3, DECLNRM, 0);
	m_vertexDecl->insert(0, 12+12, DECLFLOAT2, DECLTEX, 0);

	vertexData->setVertexDeclaration(m_vertexDecl);

	vertexData->createVertexBuffer(30000,sizeof(Vertex));
	indexData->createIndexBuffer(30000,0);
}
//////////////////////////////////////////////////////////////////////////
bool Mesh::_activate()
{
	m_cal3dRes = 
		Holder<ResourceManager>::hostage()
		->getResourceT<ResourceAnimationCal3d>( m_resourceName );


	if( m_cal3dRes == 0 )
	{
		return false;
	}

	initGeometry();

	m_calCoreModel = new CalCoreModel("dummy");
	m_scale = m_cal3dRes->getScale();

	const std::string& folder = m_cal3dRes->getFolderPath();

	for(int i = 0; i < m_cal3dRes->getSceletonInfo().size();++i)
	{

		const std::string& info = folder + m_cal3dRes->getSceletonInfo()[i];
		if(!m_calCoreModel->loadCoreSkeleton(info))
		{
			return false;
		}
	}

	for(int i = 0; i < m_cal3dRes->getCoreAnimInfo().size();++i)
	{
		const std::string& info = folder + m_cal3dRes->getCoreAnimInfo()[i];
		if(m_calCoreModel->loadCoreAnimation(info) == -1)
		{
			return false;
		}
	}

	for(int i = 0; i < m_cal3dRes->getCoreMeshInfo().size();++i)
	{
		const std::string& info = folder + m_cal3dRes->getCoreMeshInfo()[i];
		if(m_calCoreModel->loadCoreMesh(info) == -1)
		{
			return false;
		}
	}

	for(int i = 0; i < m_cal3dRes->getCoreMatInfo().size();++i)
	{
		const std::string& info = folder + m_cal3dRes->getCoreMatInfo()[i];
		if(m_calCoreModel->loadCoreMaterial(info) == -1)
		{
			return false;
		}
	}

	for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
	{
		m_calCoreModel->createCoreMaterialThread(materialId);
		m_calCoreModel->setCoreMaterialId(materialId, 0, materialId);
	}

	m_calModel = new CalModel(m_calCoreModel);

	for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
	{
		CalCoreMaterial * pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);

		for(int mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
		{
			std::string strFilename = pCoreMaterial->getMapFilename(mapId);

			FileDataInterface* imageData = Holder<FileEngine>::hostage()->openFile(folder + strFilename);

			TextureDesc td;
			
			td.buffer = (void*)imageData->getBuffer();
			td.size = imageData->size();
			td.filter = 1;

			Texture * tex = Holder<RenderEngine>::hostage()->createTextureInMemory(td);

			pCoreMaterial->setMapUserData(mapId, (Cal::UserData)tex);
		}
	}

	for(int meshId = 0; meshId < m_calCoreModel->getCoreMeshCount(); meshId++)
	{
		m_calModel->attachMesh(meshId);
	}

	m_calModel->setMaterialSet(0);

	m_currentAnimationId = 0;
	m_leftAnimationTime = m_calCoreModel->getCoreAnimation(m_currentAnimationId)->getDuration() - m_blendTime;

	if(m_calCoreModel->getCoreAnimationCount() > 1)
	{
		m_calModel->getMixer()->executeAction(m_currentAnimationId, 0.0f, m_blendTime);
	}
	else
	{
		m_calModel->getMixer()->blendCycle(m_currentAnimationId, 1.0f, 0.0f);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Mesh::_deactivate()
{
	for(int materialId = 0; materialId < m_calCoreModel->getCoreMaterialCount(); materialId++)
	{
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_calCoreModel->getCoreMaterial(materialId);
		for(int mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
		{
			Texture* tex = (Texture*)pCoreMaterial->getMapUserData(mapId);
			Holder<RenderEngine>::hostage()->releaseTexture(tex);
		}
	}

	delete m_calModel;
	delete m_calCoreModel;
	delete m_vertexDecl;
	delete m_primitiveData;

	Holder<ResourceManager>::hostage()
		->releaseResource( m_cal3dRes );
}
//////////////////////////////////////////////////////////////////////////
void Mesh::update( float _timing )
{
	if(m_calCoreModel->getCoreAnimationCount() > 1)
	{
		m_leftAnimationTime -= _timing;
		if(m_leftAnimationTime <= m_blendTime)
		{
			m_currentAnimationId = (m_currentAnimationId + 1) % m_calCoreModel->getCoreAnimationCount();
			m_calModel->getMixer()->executeAction(m_currentAnimationId, m_leftAnimationTime, m_blendTime);
			m_leftAnimationTime = m_calCoreModel->getCoreAnimation(m_currentAnimationId)->getDuration() - m_blendTime;
		}
	}

	m_calModel->update(_timing);
}
//////////////////////////////////////////////////////////////////////////
void Mesh::loader(TiXmlElement *xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_VALUE_NODE( "ImageMap", "Name", m_resourceName );
	}

	SceneNode3D::loader(xml);
}
//////////////////////////////////////////////////////////////////////////
void Mesh::_render( const mt::mat4f & _rwm, const Frustum & _frustum )
{
	CalRenderer * pCalRenderer = m_calModel->getRenderer();

	if(!pCalRenderer->beginRendering()) return;

	int meshCount = pCalRenderer->getMeshCount();

	int	m_VBCursor = 0;
	int	m_IBCursor = 0;

	for(int meshId = 0; meshId < meshCount; meshId++)
	{
		int submeshCount = pCalRenderer->getSubmeshCount(meshId);

		for(int submeshId = 0; submeshId < submeshCount; submeshId++)
		{
			if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
			{
				float * vertices = 0;

				VertexData * vertexData = m_primitiveData->getVertexData();

				vertexData->lock(vertices,m_VBCursor,pCalRenderer->getVertexCount(),sizeof(Vertex)); 
				int vertexCount = pCalRenderer->getVerticesNormalsAndTexCoords(&vertices[0]);
				vertexData->unlock();

				CalIndex * indecies = 0;

				IndexData * indexData = m_primitiveData->getIndexData();

				indexData->lock( indecies, m_IBCursor, pCalRenderer->getFaceCount() );
				int faceCount = pCalRenderer->getFaces(indecies);
				indexData->unlock();
						
				Holder<RenderEngine>::hostage()->setTexture((Texture*)pCalRenderer->getMapUserData(0));
				Holder<RenderEngine>::hostage()->drawPrimitive(m_primitiveData);

				m_VBCursor+=vertexCount;
				m_IBCursor+=faceCount;
			}
		}
	}

	pCalRenderer->endRendering();
}
//////////////////////////////////////////////////////////////////////////
void Mesh::_debugRender()
{}
//////////////////////////////////////////////////////////////////////////