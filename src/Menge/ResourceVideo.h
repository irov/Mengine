#	pragma once

#   include "Interface/VideoCodecInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Math/vec2.h"

namespace Menge
{
	class ResourceVideo
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceVideo )
	public:
		ResourceVideo();
		~ResourceVideo();

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

    public:
        float getFrameRate() const;

	public:
		const FilePath & getFilePath() const;
		const ConstString& getCodecType() const;

    public:
        VideoDecoderInterfacePtr createVideoDecoder() const;

	public:
		const mt::vec2f& getFrameSize() const;
		bool isAlpha() const;
        bool isNoSkeep() const;

    protected:
        bool _isValid() const override;

	protected:
		bool _compile() override;
		void _release() override;
			
	protected:
		FilePath m_path;
        
        ConstString m_converter;
		ConstString m_codecType;

        float m_frameRate;

        bool m_alpha;
        bool m_noSeek;
	};
}
