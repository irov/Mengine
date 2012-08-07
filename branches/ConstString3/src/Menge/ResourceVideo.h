#	pragma once

#	include "ResourceReference.h"

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
		void loader( const Metabuf::Metadata * _parser ) override;

	public:
		const WString & getFilePath() const;
		const ConstString& getCodecType() const;

	public:
		const mt::vec2f& getFrameSize() const;
		bool isAlpha() const;

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		WString m_filePath;
		bool m_alpha;
		ConstString m_codecType;
	};
}
