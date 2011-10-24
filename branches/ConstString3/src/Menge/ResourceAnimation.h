#	pragma once

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	class ResourceImage;
	class ResourceSequence;

	class ResourceAnimation
		: public ResourceReference
	{
		 RESOURCE_DECLARE( ResourceAnimation )

	public:
		ResourceAnimation();

	public:
		void setResourceImageName( const ConstString & _name );
		const ConstString & getResourceImageName() const;

		void setResourceSequenceName( const ConstString & _name );
		const ConstString & getResourceSequenceName() const;

		ResourceImage * getResourceImage() const;
		ResourceSequence * getResourceSequence() const;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		ConstString m_resourceImageName;
		ConstString m_resourceSequenceName;

		ResourceImage * m_resourceImage;
		ResourceSequence * m_resourceSequence;
	};
}
