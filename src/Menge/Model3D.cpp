#	include "Model3D.h" 

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/NodeInterface.h"

#	include "Kernel/RenderCameraProjection.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/utils.h"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Model3D::Model3D()
		: m_solid(false)
		, m_frame(nullptr)
		, m_invalidateVerticesLocal(true)
		, m_invalidateVerticesWM(true)
		, m_invalidateVerticesColor(true)
		, m_vertexCount(0)
		, m_indicesCount(0)
		, m_frameTiming(0)
		, m_currentFrame(0)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Model3D::~Model3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model3D::_compile()
	{
		if( this->compileResource_() == false )
		{
			return false;
		}

		this->invalidateMaterial();
		this->invalidateBoundingBox();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model3D::compileResource_()
	{
		if( m_resourceModel == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Model3D::compileResource_ '%s' image resource null"
				, m_name.c_str() 
				);

			return false;
		}

		if( m_resourceModel.compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Model3D::compileResource_ '%s' model resource %s not compile"
				, m_name.c_str() 
				, m_resourceModel->getName().c_str()
				);

			return false;
		}

		const Model3DInterfacePtr & model = m_resourceModel->getModel();

		m_camera = NODE_SERVICE(m_serviceProvider)
			->createNodeT<RenderCameraProjection>( CONST_STRING( m_serviceProvider, RenderCameraProjection ) );

		const ConstString & name = this->getName();
		m_camera->setName( name );

		float cameraFOV = model->getCameraFOV();
		float cameraAspect = model->getCameraAspect();
		float cameraRightSign = model->getCameraRightSign();

		m_camera->setCameraFOV( cameraFOV );
		m_camera->setCameraAspect( cameraAspect );
		m_camera->setCameraRightSign( cameraRightSign );

		this->addChild( m_camera );
		this->setRenderCamera( m_camera );

		m_vertexCount = model->getVertexCount();
		m_indicesCount = model->getIndicesCount();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_release()
	{
		Node::_release();

		m_resourceModel.release();

		if( m_camera != nullptr )
		{
			m_camera->destroy();
			m_camera = nullptr;
		}
				
		m_frame = nullptr;

		this->releaseMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::setResourceModel3D( const ResourceModel3DPtr & _resourceModel )
	{
		if( m_resourceModel == _resourceModel )
		{
			return;
		}

		m_resourceModel = _resourceModel;

		this->recompile();

		this->invalidateVertices();
		this->invalidateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceModel3DPtr & Model3D::getResourceModel3D() const
	{
		return m_resourceModel;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Model3D::_updateMaterial() const
	{
		const ResourceImagePtr & resourceImage = m_resourceModel->getResourceImage();

		if( resourceImage == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Mesh::updateMaterial_ %s resourceImage is NULL"
				, this->getName().c_str()
				);

			return nullptr;
		}

		RenderMaterialInterfacePtr material = this->makeImageMaterial( m_serviceProvider, resourceImage, m_solid );

		if( material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Mesh::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);

			return nullptr;
		}
		
		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_render( const RenderObjectState * _state )
	{
		Node::_render( _state );

		if( m_frame == nullptr )
		{
			return;
		}

		const RenderVertex2D * vertices = this->getVerticesWM();
		const RenderMaterialInterfacePtr & material = this->getMaterial();

		const mt::box2f & bb = this->getBoundingBox();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderObject( _state, material, vertices, m_vertexCount, m_frame->indecies, m_indicesCount, &bb, false );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model3D::_activate()
	{
		this->setTiming( 0.f );

		if( Node::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_deactivate()
	{
		this->stop();

		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		const RenderVertex2D * vertices = this->getVerticesWM();

		mt::reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

		for( uint32_t i = 1; i != m_vertexCount; ++i )
		{
			mt::add_internal_point( _boundingBox, vertices[i].position.x, vertices[i].position.y );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::invalidateVertices()
	{
		m_invalidateVerticesLocal = true;

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::invalidateVerticesWM()
	{
		m_invalidateVerticesWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::updateVertices() const
	{
		m_invalidateVerticesLocal = false;

		const ResourceImagePtr & resourceImage =  m_resourceModel->getResourceImage();

		const mt::uv4f & uv_image = resourceImage->getUVImage();
		const mt::uv4f & uv_alpha = resourceImage->getUVAlpha();

		for( uint32_t i = 0; i != m_vertexCount; ++i )
		{
			const mt::vec2f & uv = m_frame->uv[i];

			multiply_tetragon_uv4_v2( m_verticesWM[i].uv[0], uv_image, uv );
			multiply_tetragon_uv4_v2( m_verticesWM[i].uv[1], uv_alpha, uv );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_invalidateColor()
	{
		Node::_invalidateColor();

		this->invalidateVerticesColor();

		bool solid = this->isSolidColor();

		if( m_solid != solid )
		{
			m_solid = solid;

			this->invalidateMaterial();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::invalidateVerticesColor()
	{
		m_invalidateVerticesColor = true;                
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::updateVerticesColor() const
	{
		m_invalidateVerticesColor = false;

		ColourValue color;
		this->calcTotalColor(color);

		uint32_t argb = color.getAsARGB();

		for( RenderVertex2D
			*it = m_verticesWM,
			*it_end = m_verticesWM + m_vertexCount;
		it != it_end;
		++it )
		{
			RenderVertex2D & vtx = *it;

			vtx.color = argb;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::updateVerticesWM() const
	{
		m_invalidateVerticesWM = false;

		if( m_invalidateVerticesLocal == true )
		{
			this->updateVertices();
		}

		const mt::mat4f & wm = this->getWorldMatrix();

		for( uint32_t i = 0; i != m_vertexCount; ++i )
		{
			const mt::vec3f & position = m_frame->position[i];

			mt::vec3f & wm_pos = m_verticesWM[i].position;
			mt::mul_v3_v3_m4( wm_pos, position, wm );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_update( float _current, float _timing )
	{
		if( this->isPlay() == false )
		{
			return; 
		}

		if( m_playTime > _current )
		{
			float deltha = m_playTime - _current;
			_timing -= deltha;
		}

		const Model3DInterfacePtr & model = m_resourceModel->getModel();

		uint32_t frameCount = model->getFrameCount();
		float frameDelay = model->getFrameDelay();
		
		float speedFactor = this->getAnimationSpeedFactor();
		float scretch = this->getStretch();
		m_frameTiming += _timing * speedFactor / scretch;
		
		uint32_t lastFrame = m_currentFrame;

		if( m_currentFrame != frameCount )
		{
			while( m_frameTiming >= frameDelay )
			{
				m_frameTiming -= frameDelay;

				++m_currentFrame;

				if( m_currentFrame == frameCount )
				{
					if( this->getLoop() == true )
					{
						this->setTiming( m_frameTiming );                                   
					}
					else
					{
						if( --m_playIterator == 0 )
						{
							m_currentFrame = frameCount - 1;
							this->updateCurrentFrame_();

							m_frameTiming = 0.f;

							lastFrame = m_currentFrame;

							this->end();

							break;
						}
						else
						{
							this->setTiming( m_frameTiming );
						}					
					}

					lastFrame = m_currentFrame;
				}           
			}
		}

		if( lastFrame != m_currentFrame )
		{
			this->updateCurrentFrame_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::updateCurrentFrame_()
	{
		const Model3DInterfacePtr & model = m_resourceModel->getModel();

		m_frame = model->getFrame( m_currentFrame );

		m_camera->setCameraPosition( m_frame->cameraPosition );
		m_camera->setCameraDirection( m_frame->cameraDirection );
		m_camera->setCameraUp( m_frame->cameraUp );

		this->invalidateVertices();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model3D::_play( float _time )
	{
		(void)_time;

		if( this->isActivate() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Model3D: '%s' play not activate"
				, getName().c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model3D::_restart( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;

		if( this->isActivate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_resume( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_stop( uint32_t _enumerator )
	{
		(void)_enumerator;

		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation: '%s' stop not activate"
				, getName().c_str()
				);

			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_end( uint32_t _enumerator )
	{
		(void)_enumerator;

		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation: '%s' end not activate"
				, getName().c_str()
				);

			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Model3D::getFrame_( float _timing, float & _delthaTiming ) const
	{
		if( _timing <= 0.f )
		{
			_delthaTiming = _timing;

			return 0;
		}

		const Model3DInterfacePtr & model = m_resourceModel->getModel();

		uint32_t frameCount = model->getFrameCount();
		float frameDelay = model->getFrameDelay();

		float duration = frameCount * frameDelay;

		if( _timing >= duration )
		{
			_timing -= floorf( _timing / duration ) * duration;

			if( fabsf(_timing) < 0.0001f )
			{
				_delthaTiming = 0.f;

				return 0;
			}
		}
		
		for( uint32_t frame = 0; frame != frameCount; ++frame )
		{
			_timing -= frameDelay;

			if( _timing < 0.f )
			{
				_delthaTiming = _timing + frameDelay;

				return frame;
			}
		}

		return frameCount - 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_setFirstFrame()
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation._setFirstFrame: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		uint32_t firstFrame = 0;

		this->setCurrentFrame_( firstFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::setCurrentFrame_( uint32_t _frame )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Model3D.setCurrentFrame: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		const Model3DInterfacePtr & model = m_resourceModel->getModel();

		uint32_t frameCount = model->getFrameCount();
				
		if( _frame >= frameCount )	
		{
			LOGGER_ERROR(m_serviceProvider)("Model3D.setCurrentFrame: '%s' _frame(%d) >= frameCount(%d)"
				, m_name.c_str()
				, _frame
				, frameCount
				);

			return;
		}

		m_currentFrame = _frame;
		m_frameTiming = 0.f;

		this->updateCurrentFrame_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_setLastFrame()
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation._setLastFrame: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		const Model3DInterfacePtr & model = m_resourceModel->getModel();

		uint32_t frameCount = model->getFrameCount();

		uint32_t lastFrame = frameCount - 1;

		this->setCurrentFrame_( lastFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	void Model3D::_setTiming( float _timing )
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation._setTiming: '%s' not activate"
				, m_name.c_str()
				);

			return;
		}

		m_currentFrame = this->getFrame_( _timing, m_frameTiming );

		this->updateCurrentFrame_();
	}
	//////////////////////////////////////////////////////////////////////////
	float Model3D::_getTiming() const
	{
		if( this->isCompile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Animation._getTiming: '%s' not activate"
				, m_name.c_str()
				);

			return 0.f;
		}

		const Model3DInterfacePtr & model = m_resourceModel->getModel();

		float frameDelay = model->getFrameDelay();

		float timing = m_currentFrame * frameDelay + m_frameTiming;

		return timing; 
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model3D::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}
