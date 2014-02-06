#	pragma once

#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include "Kernel/ThreadJob.h"
#	include "Core/MemoryInput.h"

namespace Menge
{
	class ThreadWorkerImageResourceCompile
		: public ThreadWorkerInterface
	{
	public:
		ThreadWorkerImageResourceCompile();
		~ThreadWorkerImageResourceCompile();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _file, const ConstString & _codecType );
		void finalize();

	protected:
		bool onWork( size_t _id ) override;
		void onDone( size_t _id ) override;

	protected:
		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ImageDecoderInterfacePtr m_imageDecoder;
		RenderTextureInterfacePtr m_texture;

		unsigned char * m_textureBuffer;
		int m_texturePitch;

		bool m_invalid;
	};
}