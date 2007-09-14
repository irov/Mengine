#	include "AnimationObject.h" 

#	include "ObjectImplement.h"

#	include "Camera3D.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "ResourceManager.h"

#	include "ResourceAnimationCal3d.h"

#	include "AnimationCallbacks.h"

#	include "AnimationBone.h"

#	include "math/box3.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(AnimationObject);
	//////////////////////////////////////////////////////////////////////////
	AnimationObject::AnimationObject()
	: m_cal3dRes(0)
	, m_calModel(0)
	, m_vertexDecl(0)
	, m_primitiveData(0)
	, m_vertexSize(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	AnimationObject::~AnimationObject()
	{}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::loader(TiXmlElement * _xml)
	{
		SceneNode3D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "ImageMap", "Name", m_resourceName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool AnimationObject::_activate()
	{
		if( SceneNode3D::_activate() == false )
		{
			return false;
		}

		m_cal3dRes = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceAnimationCal3d>( m_resourceName );

		if( m_cal3dRes == 0 )
		{
			return false;
		}

		/*	Attach hard points to SceneNode3d	*/

		const TVecHardPoints & hardsPoints = m_cal3dRes->getHardPoints();

		for each( std::string hardPoint in hardsPoints )
		{
			int index = m_cal3dRes->getBoneIndex( hardPoint );

			assert( index != -1 && "no bone with such index!" );

			AnimationBone * bone = new AnimationBone( this, index );

			bone->setName( hardPoint );
			this->addChildren( bone );
		}

		/*	Get instance of cal3d model			*/

		m_calModel = m_cal3dRes->getNewInstance();

		size_t count = m_cal3dRes->getMeshCount();

		for( int i = 0; i < count; ++i )
		{
			m_calModel->attachMesh( i );
		}

		m_calModel->setMaterialSet(0);

		/*	Clear all blends					*/

		_clearCycles( 0.0f );

		/*	Prepare vertex and index buffers	*/

		_initGeometry();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::_deactivate()
	{
		delete m_vertexDecl;
		delete m_primitiveData;

		delete m_calModel;

		Holder<ResourceManager>::hostage()
			->releaseResource( m_cal3dRes );

		SceneNode3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::_update( float _timing )
	{
		m_calModel->update( _timing );

		if( _timing != 0 && !m_removeCallbacks.empty() )
		{
			_clearRemovedCallback();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::_render( const mt::mat4f & _rwm, const Camera3D * _camera )
	{	
		SceneNode3D::_render(_rwm,_camera);
		
		CalSkeleton * pCalSkeleton = m_calModel->getSkeleton();
		m_calModel->getSkeleton()->calculateBoundingBoxes();

		RenderEngine * renderEng = Holder<RenderEngine>::hostage();

		renderEng->setWorldMatrix( _rwm );
/*
		float lines[1024][2][3];
		int nrLines = m_calModel->getSkeleton()->getBoneLines(&lines[0][0][0]);

		for ( int i = 0; i < nrLines; ++i )
		{
			mt::vec3f p1(lines[i][0][0], lines[i][0][1], lines[i][0][2]);
			mt::vec3f p2(lines[i][1][0], lines[i][1][1], lines[i][1][2]);

			renderEng->drawLine3D(p1, p2, 0xFFFFFFAA);
		}
	
	
		float points[1024][3];

		int nrPoints = m_calModel->getSkeleton()->getBonePoints(&points[0][0]);

		for ( int i = 0; i < nrPoints; ++i )
		{
			mt::vec3f p(points[i][0], points[i][1], points[i][2]);
			mt::Box box;
			mt::SetBoxFromCenterAndExtent(box, p, mt::vec3f(5.0f, 5.0f, 5.0f));

			renderEng->drawBox(box.MinEdge,box.MaxEdge, 0xaaff00ff);
		}
*/
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
							
					Cal::UserData material = pCalRenderer->getMapUserData(0);
					renderEng->setTexture(static_cast<RenderImageInterface*>(material));
					renderEng->drawPrimitive(m_primitiveData);

					VBCursor += vertexCount;
					IBCursor += faceCount;
				}
			}
		}

		pCalRenderer->endRendering();

		static mt::mat4f ident(
			mt::vec4f(1.0f, 0.0f, 0.0f, 0.0f),
			mt::vec4f(0.0f, 1.0f, 0.0f, 0.0f),
			mt::vec4f(0.0f, 0.0f, 1.0f, 0.0f),
			mt::vec4f(0.0f, 0.0f, 0.0f, 1.0f));

		renderEng->setWorldMatrix( ident );
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::_debugRender()
	{}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::clearCallback( const std::string & _name )
	{
		int index = m_cal3dRes->getAnimationId( _name );

		assert( index != -1 && "no animation with such index!" );

		TMapCallbacks::iterator it = m_callbacks.find( index );

		if( it != m_callbacks.end() )
		{
			m_removeCallbacks.push_back( std::make_pair( index, it->second ) );
			m_callbacks.erase( it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::setCallback( const std::string & _name, float _updateTime,
		UpdateCallback _updateCallback, CompleteCallback _completeCallback, 
		void * _userData )
	{
		clearCallback( _name );

		int index = m_cal3dRes->getAnimationId( _name );

		AnimationCallback * callback = new AnimationCallback( this, _name, _updateCallback, _completeCallback, _userData );

		CalCoreAnimation * calCoreAnimation = m_cal3dRes->getCoreAnimation( index );
		calCoreAnimation->registerCallback( callback, _updateTime );

		m_callbacks.insert( std::make_pair( index, callback ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::play( const std::string& _name )
	{
		size_t count = m_cal3dRes->getAnimationCount();

		const AnimInfo& seq = m_cal3dRes->getAnimationInfo( _name );

		m_calModel->getMixer()->setTimeFactor( seq.delay );

		for( int i = 0; i < count; ++i )
		{
			if( seq.index != i )
			{
				m_calModel->getMixer()->clearCycle( i, 0 );
			}
			else
			{
				m_calModel->getMixer()->blendCycle( seq.index, 1.0f, seq.blend );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::play( const std::string& _name1,	float _weight1,
				const std::string& _name2,	float _weight2
			)
	{
		const AnimInfo& seq1 = m_cal3dRes->getAnimationInfo( _name1 );
		const AnimInfo& seq2 = m_cal3dRes->getAnimationInfo( _name2 );
 
		m_calModel->getMixer()->setTimeFactor( seq1.delay );

		size_t count = m_cal3dRes->getAnimationCount();

		for( int i = 0; i < count; ++i )
		{
			if( i == seq1.index )
			{
				m_calModel->getMixer()->blendCycle( i, _weight1, seq1.blend );
			}
			else if( i == seq2.index )
			{
				m_calModel->getMixer()->blendCycle( i, _weight2, seq1.blend );
			}
			else
			{
				m_calModel->getMixer()->clearCycle( i, seq1.blend );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	AnimationObject::execute( const std::string & _name, float _timeIn,
			float _timeOut, float _width, bool _autoLock
		)
	{
		_clearCycles( _timeIn );

		const AnimInfo& inf = m_cal3dRes->getAnimationInfo( _name );

		m_calModel->getMixer()->setTimeFactor( inf.delay );
		m_calModel->getMixer()->executeAction( inf.index, _timeIn, _timeOut, _width, _autoLock );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::mat4f	AnimationObject::getBoneWorldMatrix( int _index )
	{
		CalBone * bone = m_calModel->getSkeleton()->getBone( _index );

		const CalVector & position = bone->getTranslationAbsolute();
		const CalQuaternion & orient = bone->getRotationAbsolute();

		mt::vec3f pos(position.x,position.y,position.z);
		mt::quatf q(orient.w,orient.x,orient.y,orient.z);

		mt::mat4f worldMatrix = mt::qpos_to_rot_m4(q,pos);

		return worldMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	bool	AnimationObject::isSimilarModel( const CalModel * _calModel )
	{
		return m_calModel == _calModel;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationObject::_clearRemovedCallback()
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
	void AnimationObject::_clearCycles( float _blendTime )
	{
		size_t count = m_cal3dRes->getAnimationCount();

		for( int i = 0; i < count; ++i )
		{
			m_calModel->getMixer()->clearCycle( i, _blendTime );
		}
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
	///////////////////////////////////////////////////////////////////////////
	bool AnimationObject::isVisible( const Camera3D * _camera )
	{
		return true;
	}
	///////////////////////////////////////////////////////////////////////////
}