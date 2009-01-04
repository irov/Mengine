#	pragma once

#	include "ResourceImage.h"

#	include "Config/Typedef.h"

#	include <vector>

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
		ResourceImageDefault( const ResourceFactoryParam & _params );

	public:
		std::size_t getCount() const override;

		const mt::vec2f & getMaxSize( std::size_t _frame ) const override;
		const mt::vec2f & getSize( std::size_t _frame ) const override;
		const mt::vec2f & getOffset( std::size_t _frame ) const override;
		const mt::vec4f & getUV( std::size_t _frame ) const override;		
		bool isAlpha( std::size_t _frame ) const override;

		const RenderImageInterface * getImage( std::size_t _frame ) const override;
		void addImagePath( const String& _imagePath );

		const String & getFilename( std::size_t _frame ) const override;
		std::size_t getFilenameCount() const override;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		//unsigned int m_filter;

		typedef std::vector<ImageFrame> TVectorImageFrame;
		TVectorImageFrame m_vectorImageFrames;
	private:
		struct ImageDesc
		{
			String fileName;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
			mt::vec2f size;
			bool isAlpha;
		};

		typedef std::vector<ImageDesc> TVectorImageDesc;
		TVectorImageDesc m_vectorImageDescs;
	};
}