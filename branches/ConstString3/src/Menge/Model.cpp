#	include "Model.h"

#	include "ResourceManager.h"

#	include "RenderEngine.h"
#	include "Texture.h"

#	include "Consts.h"
#	include "BinParser.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Model::Model()
		: m_resourceModel(0)
		, m_timing(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Model::~Model()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::setModelResource( const ConstString & _resourceName )
	{
		m_resourceName = _resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Model::getModelResource() const
	{
		return m_resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model::_play()
	{
		//Empty

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::_stop( std::size_t _enumerator )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::_end( std::size_t _enumerator )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Model_Name, &Model::setModelResource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model::_compile()
	{
		m_resourceModel = ResourceManager::get()
			->getResourceT<ResourceModel>(m_resourceName);

		if( m_resourceModel == 0 )
		{
			return false;
		}

		const TVectorAnimationMesh & animationMesh = m_resourceModel->getAnimationMeshes();

		TVectorAnimationMesh::size_type am_size = animationMesh.size();
		m_renderModelMesh.reserve( am_size );

		for( TVectorAnimationMesh::const_iterator
			it = animationMesh.begin(),
			it_end = animationMesh.end();
		it != it_end;
		++it )
		{
			const AnimationMesh & am = *it;

			RenderModelMesh rmm;

			rmm.active = false;
			rmm.ibHandle = am.ibHandle;
			rmm.indices_size = am.indices.size();
			rmm.texture = am.texture;

			TVectorVertex2D::size_type verticesSize = am.verticesSize;
			rmm.frame.vertices.resize( verticesSize );

			m_renderModelMesh.push_back( rmm );
		}

		this->play();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::_release()
	{
		m_renderModelMesh.clear();

		ResourceManager::get()
			->releaseResource( m_resourceModel );

		m_resourceModel = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::_update( float _timing )
	{
		if( this->isPlay() == false )
		{
			return;
		}

		const TVectorAnimationMesh & animationMesh = m_resourceModel->getAnimationMeshes();
		
		float lastTiming = m_timing;
		m_timing += _timing;

		for( TVectorAnimationMesh::size_type
			it = 0,
			it_end = animationMesh.size();
		it != it_end;
		++it )
		{
			const AnimationMesh & am = animationMesh[it];

			RenderModelMesh & rmm = m_renderModelMesh[it];

			if( am.in > m_timing )
			{
				continue;
			}

			if( am.in >= lastTiming && am.in < m_timing )
			{
				rmm.active = true;
				//this->activateLayer_( layer.index );
			}

			if( am.out < lastTiming )
			{
				continue;
			}

			if( am.out >= lastTiming && am.out < m_timing )
			{
				if( m_loop == true )
				{
					this->play();
					return;
				}

				if( m_resourceModel->getFrameMeshLast( am, rmm.frame ) == false )
				{
					MENGE_LOG_ERROR("Model: '%s' frame last incorect '%s'"
						, m_name.c_str()
						, am.name.c_str()
						);

					continue;
				}
			}
			else
			{
				if( m_resourceModel->getFrameMesh( am, m_timing, rmm.frame ) == false )
				{
					continue;
				}
			}
		}

		//if( m_out >= lastTiming && m_out < m_timing )
		//{
		//	this->complete();
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::_render( Camera2D * _camera )
	{
		Node::_render( _camera );

		const Material * material = m_resourceModel->getMaterial();

		for( TVectorRenderModelMesh::iterator 
			it = m_renderModelMesh.begin(),
			it_end = m_renderModelMesh.end();
		it != it_end;
		++it )
		{
			RenderModelMesh & mm = *it;

			if( mm.active == false )
			{
				continue;
			}

			RenderEngine::get()->renderObject2D( 
				material, 
				&mm.texture, 
				0, 
				1, 
				&mm.frame.vertices[0], 
				mm.frame.vertices.size(), 
				LPT_MESH, 
				mm.indices_size, 
				mm.ibHandle );
		}
	}
}