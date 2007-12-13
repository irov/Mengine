#	pragma once

#	include "ResourceImage.h"

namespace Menge
{
	//! ResourceImageDefault - ресурс-файл, который содержит список изображений. 

    /*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceImageDefault" >
	*	<File Path = "имя_файла0"/>
	*    ...
	*   <File Path = "имя_файлаN"/>
	* </Resource>
	*/

	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault( const std::string & _name );

	public:
		size_t getCount() const override;

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
		mt::vec2f m_offset;
		mt::vec4f m_uv;

		size_t m_filter;

		typedef std::vector<std::string> TVectorFileNames;
		TVectorFileNames m_vectorFileNames;

		typedef std::vector<ImageFrame> TVectorImageFrame;
		TVectorImageFrame m_vectorImageFrames;
	};
}