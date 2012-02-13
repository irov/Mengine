
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

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	TaskLoadResourceImage::TaskLoadResourceImage( const ConstString & _category, const ConstString& _resourceFile, PyObject* _progressCallback )
		: m_category(_category)
		, m_resourceName(_resourceFile)
		, m_callbackComplete(_progressCallback)
	{
		pybind::incref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	TaskLoadResourceImage::~TaskLoadResourceImage()
	{
		pybind::decref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskLoadResourceImage::_onRun()
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
		
		size_t countFiles = m_resource->getFilenameCount();
			
		TTextureJobVector::iterator it_jobs = m_textureJobs.begin();

		for( size_t i = 0; i < countFiles; ++i )
		{
			const ConstString & filename = m_resource->getFileName( i );

			if( filename == Consts::get()->c_CreateTexture || 
				filename == Consts::get()->c_CreateTarget || 
				RenderEngine::get()->hasTexture( filename ) == true )
			{
				continue;
			}
			
			TextureJob job;
			job.filename = filename;
			job.file = FileEngine::get()->createInputFile(m_category);
			
			m_textureJobs.push_back(job);
		}

		ResourceManager::get()->lockResource( m_resourceName );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskLoadResourceImage::_onMain()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			TextureJob & job = (*it);
			
			if( job.file->open( Helper::to_str(job.filename) ) == false )
			{
				MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
					, job.filename.c_str()
					);

				return false;
			}

			job.decoder = CodecEngine::get()->createDecoderT<ImageDecoderInterface>( "pngImage", job.file );

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
			job.texture = RenderEngine::get()->createTexture( job.filename, dataInfo->width, dataInfo->height, dataInfo->format );
			if( job.texture == NULL )
			{
				job.file->close();
				return false;
			}

			job.textureBuffer = job.texture->lock( &(job.textureBufferPitch), false );

			job.texture->loadImageData( job.textureBuffer, job.textureBufferPitch, job.decoder );
			job.decoder->destroy();
			job.file->close();
			job.texture->unlock();
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadResourceImage::_onComplete()
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
	void TaskLoadResourceImage::_onJoin()
	{
		pybind::call( m_callbackComplete, "(O)", pybind::get_bool(false) );
	}	
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadResourceImage::_onCancel()
	{
		ResourceManager::get()->unlockResource( m_resourceName );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
