
#	include "ThreadTaskLoadResourceImage.h"
#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "ResourceImageDefault.h"
#	include "RenderEngine.h"

#	include "Interface/ImageCodecInterface.h"
#	include "LogEngine.h"
#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "Consts.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskLoadResourceImage::ThreadTaskLoadResourceImage( const ConstString & _category, const ConstString& _resourceFile, PyObject* _progressCallback )
		: m_category(_category)
		, m_resourceName(_resourceFile)
		, m_callbackComplete(_progressCallback)
	{
		pybind::incref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskLoadResourceImage::~ThreadTaskLoadResourceImage()
	{
		pybind::decref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskLoadResourceImage::_onRun()
	{
		m_resource = dynamic_cast<ResourceImage*>( ResourceManager::get()->getResourceReference(m_resourceName) );

		if( m_resource == 0 )
		{
			//error
			MENGE_LOG_ERROR( "TaskLoadResourceImage:  Resource Image '%s' was not found"
				, m_resourceName.c_str()
				);

			return false;
		}
					
		TVectorTextureJob::iterator it_jobs = m_textureJobs.begin();
		
		const WString & filename = m_resource->getFileName();
		
		if( RenderEngine::get()->hasTexture( filename ) == true )
		{
			return true;
		}

		const ConstString & name = m_resource->getName();
			
		TextureJob job;
		job.name = name;
		job.filename = filename;
		job.file = FileEngine::get()->createInputFile(m_category);
			
		m_textureJobs.push_back(job);
		
		ResourceManager::get()->lockResource( m_resourceName );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskLoadResourceImage::_onMain()
	{
		for( TVectorTextureJob::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			TextureJob & job = (*it);
			
			if( job.file->open( job.filename ) == false )
			{
				MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
					, job.filename.c_str()
					);

				return false;
			}

			job.decoder = CodecEngine::get()->createDecoderT<ImageDecoderInterface>( ConstString("pngImage"), job.file );

			if( job.decoder == NULL )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file '%s' was not found"
					, job.filename.c_str() 
					);

				job.file->close();

				return false;
			}

			const ImageCodecDataInfo* dataInfo = job.decoder->getCodecDataInfo();

			if( dataInfo->format == PF_UNKNOWN )
			{
				MENGE_LOG_ERROR( "Error: Invalid image format '%s'"
					,job.filename.c_str() 
					);

				job.decoder->destroy();
				job.file->close();
				return false;
			}
			//const mt::vec2f & texture_size = m_resource->getMaxSize( i );
			job.texture = RenderEngine::get()
				->createTexture( dataInfo->width, dataInfo->height, dataInfo->format );

			if( job.texture == NULL )
			{
				job.file->close();
				return false;
			}

			RenderEngine::get()
				->cacheFileTexture( job.filename, job.texture );


			Rect rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = dataInfo->width;
			rect.bottom = dataInfo->height;

			job.textureBuffer = job.texture->lock( &(job.textureBufferPitch), rect, false );

			//job.texture->loadImageData( job.textureBuffer, job.textureBufferPitch, job.decoder );

			ImageCodecOptions options;

			PixelFormat hwPixelFormat = job.texture->getHWPixelFormat();

			if( dataInfo->format == PF_R8G8B8
				&& hwPixelFormat == PF_X8R8G8B8 )
			{
				options.flags |= DF_COUNT_ALPHA;
			}

			size_t width = job.texture->getWidth();

			if( job.textureBufferPitch != dataInfo->width )
			{
				options.pitch = job.textureBufferPitch;

				options.flags |= DF_CUSTOM_PITCH;
			}

			job.decoder->setOptions( &options );

			RenderEngine::get()
				->loadBufferImageData( job.textureBuffer, job.textureBufferPitch, hwPixelFormat, job.decoder );

			job.decoder->destroy();
			job.file->close();
			job.texture->unlock();
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskLoadResourceImage::_onComplete()
	{		
		ResourceManager::get()->unlockResource( m_resourceName );
		//size_t count2 = m_resource->countReference(); 
		m_resource->incrementReference();
		/*size_t count = m_resource->countReference();
		if( count <= 0 )
		{
			int x = 1;
		}*/
		
		/*for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			TextureJob & job = (*it);
			RenderEngine::get()->releaseTexture( job.texture );
		}
		*/
		if( m_callbackComplete != NULL )
		{
			pybind::call( m_callbackComplete, "(O)", pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskLoadResourceImage::_onInterrupt()
	{
		pybind::call( m_callbackComplete, "(O)", pybind::get_bool(false) );
	}	
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskLoadResourceImage::_onCancel()
	{
		ResourceManager::get()->unlockResource( m_resourceName );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
