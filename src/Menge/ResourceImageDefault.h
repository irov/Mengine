#	pragma once

#	include "ResourceImage.h"

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
		void addFrameFile( const std::string & _path );
		void addFrameAtlasUV( const mt::vec4f & _v );
		void addFrameAtlasOffset( const mt::vec2f & _v );
		void addFrameAtlasMaxSize( const mt::vec2f & _v );

	public:
		unsigned int getCount() const override;

		const mt::vec2f & getMaxSize( unsigned int _frame ) const override;
		const mt::vec2f & getSize( unsigned int _frame ) const override;
		const mt::vec2f & getOffset( unsigned int _frame ) const override;
		const mt::vec4f & getUV( unsigned int _frame ) const override;		

		const RenderImageInterface * getImage( unsigned int _frame ) const override;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		unsigned int m_filter;

		typedef std::vector<std::string> TVectorFileNames;
		TVectorFileNames m_vectorFileNames;

		typedef std::vector<ImageFrame> TVectorImageFrame;
		TVectorImageFrame m_vectorImageFrames;

		std::vector<mt::vec4f> m_vectorAtlasUVs;
		std::vector<mt::vec2f> m_vectorAtlasOffsets;
		std::vector<mt::vec2f> m_vectorAtlasMaxSizes;
	};
}