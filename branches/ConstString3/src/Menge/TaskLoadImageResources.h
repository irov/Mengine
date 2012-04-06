#	pragma once

#	include "Task.h"

#	include "Core/String.h"

#	include <pybind/types.hpp>

namespace Menge
{
	class ResourceImage;
	class RenderTextureInterface;
	class ImageDecoderInterface;
	class FileInputStreamInterface;

	class TaskLoadImageResources
		: public Task
	{
	public:
		TaskLoadImageResources( const ConstString & _category, TVectorConstString _resources, PyObject* _progressCallback );
		~TaskLoadImageResources();

	protected:
		bool _onRun() override;
		bool _onMain() override;
		void _onComplete() override;
		void _onCancel() override;
		void _onInterrupt() override;
	protected:
		ConstString m_category;

		PyObject * m_callbackComplete;
		typedef std::vector<ResourceImage *> TVectorResourceImages;
		TVectorResourceImages m_resources;
		
		TVectorConstString m_resourceNames;
		
		struct TextureJob
		{
			ConstString name;
			WString filename;
			FileInputStreamInterface* file;
			ImageDecoderInterface * decoder;
			RenderTextureInterface* texture;
			unsigned char* textureBuffer;
			int textureBufferPitch;
		};

		typedef std::vector<TextureJob> TTextureJobVector;
		TTextureJobVector m_textureJobs;
	};
}	// namespace Menge
