#	include "AnimationObject.h" 

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
OBJECT_IMPLEMENT(AnimationObject);
//////////////////////////////////////////////////////////////////////////
AnimationObject::AnimationObject()
: m_scale(1.0f)
, m_leftAnimationTime(0)
, m_blendTime(1.0f)
, m_currentAnimationId(0)
, m_calCoreModel(0)
, m_calModel(0)
{}
//////////////////////////////////////////////////////////////////////////
AnimationObject::~AnimationObject()
{}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::clearCycles()
{
	size_t animCount = m_calCoreModel->getCoreAnimationCount();

	for(int i = 0; i < animCount; ++i)
	{
		m_calModel->getMixer()->clearCycle(i,0.0f);
	}
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::play(const std::string& _name)
{
	size_t animCount = m_calCoreModel->getCoreAnimationCount();

	int action = m_calCoreModel->getCoreAnimationId(_name);

	for(int i = 0; i < animCount; ++i)
	{
		if(action != i)
		{
			m_calModel->getMixer()->clearCycle(i, 0);
		}
		else
		{
			m_calModel->getMixer()->blendCycle(action, 1.0f, m_blendTime);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::nextPlay()
{
	size_t size = m_calCoreModel->getCoreAnimationCount();
	m_currentAnimationId = (m_currentAnimationId + 1) % size;

	for(int i = 0; i < size; ++i)
	{
		if(m_currentAnimationId != i)
		{
			m_calModel->getMixer()->clearCycle(i, 0);
		}
		else
		{
			m_calModel->getMixer()->blendCycle(m_currentAnimationId, 1.0f, m_blendTime);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::playBlend(
			const std::vector<std::string>& _animNames,
			const std::vector<float>& _weights
		)
{
	size_t size = _weights.size();

	if(_animNames.size() != size)
	{
		assert(!"sizes of two vectors are different!");
	}

	std::list<int> animationIds;
	
	int id = 0;

	for each( const std::string & animationName in _animNames )
	{
		/*getCoreAnimationId работает только при условии добавления имени вначале загрузки меша */
		int animId = m_calCoreModel->getCoreAnimationId(animationName);

		if(animId == -1)
		{
			assert(!"no such name of animation!");
		}

		float w = _weights[id++];

		m_calModel->getMixer()->blendCycle(animId, w, m_blendTime);
		animationIds.push_back(animId);
	}

	size_t animCount = m_calCoreModel->getCoreAnimationCount();

	m_calModel->getMixer()->clearCycle(2, 0);

	//FIX!!!!!!!
	/*for(int i = 0; i < animCount; ++i)
	{
		for(std::list<int>::iterator it = animationIds.begin(); it != animationIds.end();++it)
		{
			if(i != *it)	m_calModel->getMixer()->clearCycle(i, 0);
		}
	}
	*/
}

void AnimationObject::getBonePosition(int _boneIndex, mt::vec3f& _position)
{
	CalBone * bone = m_calModel->getSkeleton()->getBone(_boneIndex);

	if(bone == NULL)
	{
		assert(!"no bone with this index");
		return;
	}

	const CalVector &pos = bone->getTranslationAbsolute();

	_position.x = pos.x;
	_position.y = pos.y;
	_position.z = pos.z;
}
//////////////////////////////////////////////////////////////////////////
int AnimationObject::getBoneIndex(const std::string& _bonename)
{
	return m_calCoreModel->getCoreSkeleton()->getCoreBoneId(_bonename);
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::getBoneRotation(int _boneIndex, mt::quatf & _q)
{
	CalBone * bone = m_calModel->getSkeleton()->getBone(_boneIndex);

	if(bone == NULL)
	{
		assert(!"Not bone");
		return;
	}
	
	const CalQuaternion & calQ = bone->getRotationAbsolute();

	_q.w = calQ.w;
	_q.x = calQ.x;
	_q.y = calQ.y;
	_q.z = calQ.z;
}
///////////////////////////////////////////////////////////////////////////
bool AnimationObject::isVisible( const Camera3D * _camera )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::_initGeometry()
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
bool AnimationObject::_activate()
{
	m_cal3dRes = 
		Holder<ResourceManager>::hostage()
		->getResourceT<ResourceAnimationCal3d>( m_resourceName );


	if( m_cal3dRes == 0 )
	{
		return false;
	}

	_initGeometry();

	m_calCoreModel = new CalCoreModel("dummy");
	m_scale = m_cal3dRes->getScale();

	const std::string& folder = m_cal3dRes->getFolderPath();

	for(size_t i = 0; i < m_cal3dRes->getSceletonInfo().size();++i)
	{

		const std::string& info = folder + m_cal3dRes->getSceletonInfo()[i];
		if(!m_calCoreModel->loadCoreSkeleton(info))
		{
			return false;
		}
	}

	size_t sizeAnimCount = m_cal3dRes->getCoreAnimInfo().size();

	for(size_t i = 0; i < sizeAnimCount;++i)
	{
		const std::string& info = folder + m_cal3dRes->getCoreAnimInfo()[i];
		if(m_calCoreModel->loadCoreAnimation(info) == -1)
		{
			return false;
		}
		m_calCoreModel->addAnimationName(info,i);
	}

	for(size_t i = 0; i < m_cal3dRes->getCoreMeshInfo().size();++i)
	{
		const std::string& info = folder + m_cal3dRes->getCoreMeshInfo()[i];
		if(m_calCoreModel->loadCoreMesh(info) == -1)
		{
			return false;
		}
	}

	for(size_t i = 0; i < m_cal3dRes->getCoreMatInfo().size();++i)
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

	m_leftAnimationTime = m_calCoreModel->getCoreAnimation(m_currentAnimationId)->getDuration() - m_blendTime;

	clearCycles();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::_deactivate()
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

//	m_calModel->destroy();

	delete m_calModel;
	delete m_calCoreModel;

	delete m_vertexDecl;
	delete m_primitiveData;

	Holder<ResourceManager>::hostage()
		->releaseResource( m_cal3dRes );
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::update( float _timing )
{
	m_calModel->update(_timing);
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::loader(TiXmlElement *xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_VALUE_NODE( "ImageMap", "Name", m_resourceName );
	}

	SceneNode3D::loader(xml);
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::_render( const mt::mat4f & _rwm, const Camera3D * _camera )
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
				RenderEngine * renderEng = Holder<RenderEngine>::hostage();

				unsigned char ambient[4];
				pCalRenderer->getAmbientColor(&ambient[0]);

				unsigned char diffuse[4];
				pCalRenderer->getAmbientColor(&diffuse[0]);

				unsigned char specular[4];
				pCalRenderer->getSpecularColor(&specular[0]);

				renderEng->setMaterialColor(ambient,diffuse,specular);

				float * vertices = 0;

				VertexData * vertexData = m_primitiveData->getVertexData();

				vertexData->lock(vertices,m_VBCursor,pCalRenderer->getVertexCount(),sizeof(Vertex)); 
				int vertexCount = pCalRenderer->getVerticesNormalsAndTexCoords( vertices );
				vertexData->unlock();

				CalIndex * indecies = 0;

				IndexData * indexData = m_primitiveData->getIndexData();

				indexData->lock( indecies, m_IBCursor, pCalRenderer->getFaceCount() );
				int faceCount = pCalRenderer->getFaces( indecies );
				indexData->unlock();
						
				renderEng->setTexture((Texture*)pCalRenderer->getMapUserData(0));
				renderEng->drawPrimitive(m_primitiveData);

				m_VBCursor+=vertexCount;
				m_IBCursor+=faceCount;
			}
		}
	}

	pCalRenderer->endRendering();
}
//////////////////////////////////////////////////////////////////////////
void AnimationObject::_debugRender()
{}
//////////////////////////////////////////////////////////////////////////