#	include "SurfaceTrackMatte.h"

#	include "Interface/StringizeInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	SurfaceTrackMatte::SurfaceTrackMatte()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SurfaceTrackMatte::~SurfaceTrackMatte()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::setResourceImage( const ResourceImagePtr & _resourceImage )
	{
		if( m_resourceImage == _resourceImage )
		{
			return;
		}

		m_resourceImage = _resourceImage;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & SurfaceTrackMatte::getResourceImage() const
	{
		return m_resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::setResourceTrackMatteImage( const ResourceImagePtr & _resourceTrackMatteImage )
	{
		if( m_resourceTrackMatteImage == _resourceTrackMatteImage )
		{
			return;
		}

		m_resourceTrackMatteImage = _resourceTrackMatteImage;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & SurfaceTrackMatte::getResourceTrackMatteImage() const
	{
		return m_resourceTrackMatteImage;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceTrackMatte::_compile()
	{
		if( m_resourceImage == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("SurfaceTrackMatte::_compile: '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

		if( m_resourceImage.compile() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("SurfaceTrackMatte::_compile: '%s' resource '%s' is not compile"
				, m_name.c_str()
				, m_resourceImage->getName().c_str()
				);

			return false;
		}

		if( m_resourceTrackMatteImage == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("SurfaceTrackMatte::_compile: '%s' resource is null"
				, m_name.c_str()
				);

			return false;
		}

		if( m_resourceTrackMatteImage.compile() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("SurfaceTrackMatte::_compile: '%s' resource '%s' is not compile"
				, m_name.c_str()
				, m_resourceTrackMatteImage->getName().c_str()
				);

			return false;
		}

		this->invalidateMaterial();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::_release()
	{
		m_resourceImage.release();
		m_resourceTrackMatteImage.release();
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::_update( float _current, float _timing )
	{
		(void)_current;
		(void)_timing;
		//const RenderMaterialInterfacePtr & material = this->getMaterial();

		//const RenderMaterialStage * stage = material->getStage();

		//const RenderFragmentShaderInterfacePtr & fragmentShader = stage->program->getFragmentShader();

		//fragmentShader->g
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceTrackMatte::_play( float _time )
	{
		(void)_time;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceTrackMatte::_restart( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::_resume( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::_stop( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void SurfaceTrackMatte::_end( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SurfaceTrackMatte::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr SurfaceTrackMatte::_updateMaterial() const
	{
		RenderTextureInterfacePtr textures[2];

		textures[0] = m_resourceImage->getTexture();
		textures[1] = m_resourceTrackMatteImage->getTexture();

		RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE( m_serviceProvider )
			->getMaterial( STRINGIZE_STRING_LOCAL( m_serviceProvider, "TrackMatte_Blend" ), PT_TRIANGLELIST, 2, textures );

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
}
