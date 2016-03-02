#	pragma once

#	include "Kernel/ResourceReference.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceMusic
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceMusic();

	public:
		const FilePath & getPath() const;
		const ConstString & getCodec() const;

		float getVolume() const;
		bool isExternal() const;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _isValid() const override;

	private:
		FilePath m_path;
		ConstString m_codec;
		float m_volume;
		bool m_external;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceMusic> ResourceMusicPtr;
}
