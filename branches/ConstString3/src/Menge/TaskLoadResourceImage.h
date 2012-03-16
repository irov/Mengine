#	pragma once

#	include "Task.h"

#	include "Core/String.h"

#	include <pybind/types.hpp>

namespace Menge
{
	class ResourceImage;
	class TextureInterface;
	class ImageDecoderInterface;
	class FileInputStreamInterface;
		
	class TaskLoadResourceImage
		: public Task
	{
	public:
		TaskLoadResourceImage( const ConstString & _category, const ConstString & _resourceName, PyObject* _progressCallback );
		~TaskLoadResourceImage();

	protected:
		bool _onRun() override;
		bool _onMain() override;
		void _onComplete() override;
		void _onJoin() override;
		void _onCancel() override;
	protected:
		ConstString m_category;
		
		PyObject * m_callbackComplete;
	
		ResourceImage * m_resource;
		ConstString m_resourceName;

		struct TextureJob
		{
			ConstString filename;
			FileInputStreamInterface* file;
			ImageDecoderInterface * decoder;
			TextureInterface* texture;
			unsigned char* textureBuffer;
			int textureBufferPitch;
		};
		
		typedef std::vector<TextureJob> TTextureJobVector;
		TTextureJobVector m_textureJobs;
	};
}	// namespace Menge
