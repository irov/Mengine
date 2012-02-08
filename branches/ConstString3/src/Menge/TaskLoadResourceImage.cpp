
#	include "TaskLoadResourceImage.h"
#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "ResourceImageDefault.h"
#	include "RenderEngine.h"

#	include "Interface/ImageCodecInterface.h"
#	include "LogEngine.h"
#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "Texture.h"
#	include "Consts.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

enum
{
	STATE_ERROR = 0,
	STATE_DECODED = 1,
	STATE_LOCKED = 2
};

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	TaskLoadResourceImage::TaskLoadResourceImage( const ConstString & _category, const ConstString& _resourceFile, PyObject* _progressCallback )
		: m_category(_category)
		, m_resourceName(_resourceFile)
		, m_progressCallback(_progressCallback)
		, m_oldProgress(0.f)
		, m_progress(0.f)
		, m_progressStep(0.f)
		, m_lockDone(false)
		, m_decodeDone(true)
	{
		pybind::incref( m_progressCallback );

		m_resourceMgr = ResourceManager::get();
		m_renderEngine = RenderEngine::get();
		m_codecEngine = CodecEngine::get();
		m_fileEngine = FileEngine::get();
	}
	//////////////////////////////////////////////////////////////////////////
	TaskLoadResourceImage::~TaskLoadResourceImage()
	{
		pybind::decref( m_progressCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadResourceImage::_onRun()
	{
		m_resource = dynamic_cast<ResourceImageDefault*>(m_resourceMgr->getResourceReference(m_resourceName));
		if( m_resource == 0 )
		{
			//error
			MENGE_LOG_ERROR( "TaskLoadResourceImage:  Resource Image '%s' was not found"
				, m_resourceName.c_str()
				);
			join();
			return;
		}
		
		size_t countFiles = m_resource->getFilenameCount();
			
		TTextureJobVector::iterator it_jobs = m_textureJobs.begin();
		
		for( size_t i = 0; i < countFiles; ++i )
		{
			const ConstString & filename = m_resource->getFilename( i ) ;
			if( filename == Consts::get()->c_CreateTexture || 
				filename == Consts::get()->c_CreateTarget || 
				m_renderEngine->hasTexture( filename ) == true )
			{
				continue;
			}
			
			TextureJob job;
			job.filename = filename;
			job.file = m_fileEngine->createInputFile(m_category);

			if( job.file->open( Helper::to_str(filename) ) == false )
			{
				MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
					, filename.c_str()
					);

				//job.state = STATE_ERROR;
				continue;
			}

			job.decoder = m_codecEngine->createDecoderT<ImageDecoderInterface>( Consts::get()->c_Image, job.file );

			if( job.decoder == NULL )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file '%s' was not found"
					, filename.c_str() 
					);

				job.file->close();
				//job.state = STATE_ERROR;
				continue;
			}
						
			const ImageCodecDataInfo* dataInfo = job.decoder->getCodecDataInfo();
			//MENGE_LOG_INFO( "Create and lock texture %s", name.c_str() );
			job.texture = m_renderEngine->createTexture( job.filename, dataInfo->width, dataInfo->height, dataInfo->format );
			if( job.texture == NULL )
			{
				//job.state = STATE_ERROR;
				job.file->close();
				job.decoder->destroy();
				continue;
			}

			job.textureBuffer = job.texture->lock( &(job.textureBufferPitch), false );
			job.state = STATE_LOCKED;

			m_textureJobs.push_back(job);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadResourceImage::_onMain()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
		++it )
		{
			TextureJob& job = (*it);
			if( job.state == STATE_LOCKED )
			{
				job.texture->loadImageData( job.textureBuffer, job.textureBufferPitch, job.decoder );
				job.state = STATE_DECODED;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadResourceImage::_onComplete()
	{
		
		m_resource->incrementReference();

		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
		++it )
		{
			TextureJob& job = (*it);
			if( job.state == STATE_DECODED 
				|| job.state == STATE_LOCKED  )
			{
				job.decoder->destroy();
				job.file->close();
				job.texture->unlock();
				m_renderEngine->releaseTexture( job.texture );
			}
		}
		
		if( m_progressCallback != NULL
			&& pybind::string_check(m_progressCallback) == false )
		{
			pybind::call( m_progressCallback, "(fO)", 1.0f, pybind::get_bool(true) );
		}
	}

	void TaskLoadResourceImage::_onJoin()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
		++it )
		{
			TextureJob& job = (*it);
			if( job.state == STATE_LOCKED 
				|| job.state == STATE_DECODED  )
			{
				job.decoder->destroy();
				job.file->close();
				job.texture->unlock();
				m_renderEngine->releaseTexture( job.texture );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
