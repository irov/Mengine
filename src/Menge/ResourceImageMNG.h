#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	class ResourceImageMNG
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageMNG )

	private:

		struct Image
		{
			mt::vec2f offset;
			mt::vec2f size;
			RenderImageInterface * renderImage;
		};

	public:
		ResourceImageMNG( const std::string & _name );

	public:
		size_t getCount() override;

		const mt::vec2f & getMaxSize( size_t _frame ) const override;
		const mt::vec2f & getSize( size_t _frame ) const override;
		const mt::vec2f & getOffset( size_t _frame ) const override;
		const mt::vec4f & getUV( size_t _frame ) const override;
	 
		const RenderImageInterface * getImage( size_t _frame ) const override;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		std::string m_fileMNG;

		mt::vec2f m_size;
		mt::vec4f m_uv;

		std::vector<Image>	m_images;
	};
}