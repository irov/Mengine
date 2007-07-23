#	include "AnimationObject.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "ResourceManager.h"

#	include "ResourceAnimationCal3d.h"

#	include "AnimationCallbacks.h"

#	include "AnimationBone.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(AnimationObject);
	//////////////////////////////////////////////////////////////////////////
	AnimationObject::AnimationObject()
	: m_calModel(0)
	, m_vertexDecl(0)
	, m_primitiveData(0)
	, m_vertexSize(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	AnimationObject::~AnimationObject()
	{}
	///////////////////////////////////////////////////////////////////////////
	bool AnimationObject::isVisible( const Camera3D * _camera )
	{
	//	CalBoundingBox & box = m_calModel->getBoundingBox();

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
		
		//need for calc. size. 
		struct Vertex
		{
			float pos[3];
			float normal[3];
			float t;
			float s;
		};

		m_vertexSize = sizeof(Vertex);

		vertexData->createVertexBuffer(30000,m_vertexSize);
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
		//Taking instance from cal3d core model.
		m_calModel = m_cal3dRes->createInstance();

		for(int meshId = 0; meshId < m_cal3dRes->getMeshCount(); meshId++)
		{
			m_calModel->attachMesh(meshId);
		}

		m_calModel->setMaterialSet(0);

		const TVecHardPoints & hp = m_cal3dRes->getHardPoints();

		for each( std::string hardPoint in hp )
		{
			this->addChildren(new AnimationBone(this, hardPoint));
		}

		clearCycles();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::_deactivate()
	{
		delete m_calModel;

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

		int	VBCursor = 0;
		int	IBCursor = 0;

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

					vertexData->lock(vertices, VBCursor, pCalRenderer->getVertexCount(), m_vertexSize); 
					int vertexCount = pCalRenderer->getVerticesNormalsAndTexCoords( vertices );
					vertexData->unlock();

					CalIndex * indecies = 0;

					IndexData * indexData = m_primitiveData->getIndexData();

					indexData->lock( indecies, IBCursor, pCalRenderer->getFaceCount() );
					int faceCount = pCalRenderer->getFaces( indecies );
					indexData->unlock();
							
					renderEng->setTexture((Texture*)pCalRenderer->getMapUserData(0));
					renderEng->drawPrimitive(m_primitiveData);

					VBCursor += vertexCount;
					IBCursor += faceCount;
				}
			}
		}

		pCalRenderer->endRendering();
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::_debugRender()
	{}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::setCallback(const std::string & _name, float _updateTime, UpdateCallback _updateCallback, CompleteCallback _completeCallback, void * _userData)
	{
		int index = m_cal3dRes->getAnimationId(_name);

		if(index != -1)
		{
			TMapCallbacks::iterator it = m_callbacks.find(index);

			if(it != m_callbacks.end())
			{
				m_removeCallbacks.push_back(std::make_pair(index, it->second));
				m_callbacks.erase(it);
			}

			CalCoreAnimation * calCoreAnimation = m_cal3dRes->getCoreAnimation(index);
			
			AnimationCallback * callback = new AnimationCallback(this, _name, _updateCallback, _completeCallback, _userData);

			calCoreAnimation->registerCallback(callback, _updateTime);

			m_callbacks.insert(std::make_pair(index, callback));
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::clearCallback(const std::string & _name)
	{
		int index = m_cal3dRes->getAnimationId(_name);

		if(index != -1)
		{
			TMapCallbacks::iterator it = m_callbacks.find(index);

			if(it != m_callbacks.end())
			{
				m_removeCallbacks.push_back(std::make_pair(index, it->second));
				m_callbacks.erase(it);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::clearRemovedCallback()
	{
		for(TListRemoveCallbacks::iterator it = m_removeCallbacks.begin(); 
			it!= m_removeCallbacks.end(); ++it)
		{
			CalCoreAnimation * calCoreAnimation = m_cal3dRes->getCoreAnimation((*it).first);
			calCoreAnimation->removeCallback((*it).second);
			delete (*it).second;
		}

		m_removeCallbacks.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::clearCycles()
	{
		size_t animCount = m_cal3dRes->getAnimationCount();

		for(int i = 0; i < animCount; ++i)
		{
			m_calModel->getMixer()->clearCycle(i,0.0f);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::play(const std::string& _name)
	{
		size_t animCount = m_cal3dRes->getAnimationCount();

		AnimInfo * seq = m_cal3dRes->getAnimationInfo(_name);

		m_calModel->getMixer()->setTimeFactor(seq->delay);

		for(int i = 0; i < animCount; ++i)
		{
			if(seq->index != i)
			{
				m_calModel->getMixer()->clearCycle(i, 0);
			}
			else
			{
				m_calModel->getMixer()->blendCycle(seq->index, 1.0f, seq->blend);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::play2Blend(const std::string& _name1,	float _weight1,
				const std::string& _name2,	float _weight2
			)
	{
		AnimInfo * seq1 = m_cal3dRes->getAnimationInfo(_name1);
		AnimInfo * seq2 = m_cal3dRes->getAnimationInfo(_name2);

		m_calModel->getMixer()->setTimeFactor(seq1->delay);

		float blend	= seq1->blend;

		size_t animCount = m_cal3dRes->getAnimationCount();

		for(int i = 0; i < animCount; i++)
		{
			if(i == seq1->index)
			{
				m_calModel->getMixer()->blendCycle(i, _weight1, blend);
			}
			else if(i == seq2->index)
			{
				m_calModel->getMixer()->blendCycle(i, _weight2, blend);
			}
			else
			{
				m_calModel->getMixer()->clearCycle(i, blend);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	int AnimationObject::getBoneIndex(const std::string& _bonename)
	{
		return m_cal3dRes->getBoneIndex(_bonename);
	}
	//////////////////////////////////////////////////////////////////////////
}