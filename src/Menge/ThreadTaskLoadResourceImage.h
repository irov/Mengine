//#	pragma once
//
//#	include "ThreadTask.h"
//
//#	include "Core/String.h"
//
//#	include <pybind/types.hpp>
//
//namespace Menge
//{
//	class ResourceImage;
//	class RenderTextureInterface;
//	class ImageDecoderInterface;
//	class FileInputStreamInterface;
//		
//	class ThreadTaskLoadResourceImage
//		: public ThreadTask
//	{
//	public:
//		ThreadTaskLoadResourceImage( const ConstString & _category, const ConstString & _resourceName, PyObject* _progressCallback );
//		~ThreadTaskLoadResourceImage();
//
//	protected:
//		bool _onRun() override;
//		bool _onMain() override;
//		void _onComplete() override;
//		void _onInterrupt() override;
//		void _onCancel() override;
//
//	protected:
//		ConstString m_category;
//		
//		PyObject * m_callbackComplete;
//	
//		ResourceImage * m_resource;
//		ConstString m_resourceName;
//
//		struct TextureJob
//		{
//			ConstString name;
//			WString filename;
//			FileInputStreamInterface* file;
//			ImageDecoderInterface * decoder;
//			RenderTextureInterface* texture;
//			unsigned char* textureBuffer;
//			int textureBufferPitch;
//		};
//		
//		typedef std::vector<TextureJob> TVectorTextureJob;
//		TVectorTextureJob m_textureJobs;
//	};
//}	// namespace Menge
