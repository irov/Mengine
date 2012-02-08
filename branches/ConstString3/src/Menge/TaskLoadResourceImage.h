#	pragma once
#	include "Core/String.h"

#	include "Task.h"

#	include "pybind/types.hpp"

namespace Menge
{
	class ResourceReference;
	class ResourceImageDefault;
	class Texture;
	class ImageDecoderInterface;
	class FileInputStreamInterface;

	
	class ResourceManager;
	class RenderEngine;
	class CodecEngine;
	class FileEngine;

	class TaskLoadResourceImage
		: public Task
	{
	public:
		TaskLoadResourceImage( const ConstString & _category, const ConstString & _resourceFile, PyObject* _progressCallback );
		~TaskLoadResourceImage();

	public:
		void _onRun() override;
		void _onMain() override;
		void _onComplete() override;
		void _onJoin() override;
	protected:
		ConstString m_category;
		
		PyObject * m_progressCallback;

		float m_oldProgress;
		float m_progress;
		
		float m_progressStep;
		bool m_lockDone;
		bool m_decodeDone;
		
		ResourceImageDefault * m_resource;
		ConstString m_resourceName;

		struct TextureJob
		{
			ConstString filename;
			FileInputStreamInterface* file;
			ImageDecoderInterface * decoder;
			Texture* texture;
			unsigned char* textureBuffer;
			int textureBufferPitch;
			int state;

			TextureJob()
				: state( 0 )
			{
			}
		};
		
		typedef std::vector<TextureJob> TTextureJobVector;
		TTextureJobVector m_textureJobs;

		ResourceManager * m_resourceMgr;
		RenderEngine * m_renderEngine;
		FileEngine * m_fileEngine;
		CodecEngine * m_codecEngine;
	};
}	// namespace Menge
