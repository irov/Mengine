#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageDefault - ������-����, ������� �������� ������ �����������. 

    /*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceImageDefault" >
	*	<File Path = "���_�����0"/>
	*    ...
	*   <File Path = "���_�����N"/>
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

		struct ImageDesc
		{
			std::string fileName;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
		};

		typedef std::vector<ImageDesc> TVectorImageDesc;
		TVectorImageDesc m_vectorImageDescs;

		typedef std::vector<ImageFrame> TVectorImageFrame;
		TVectorImageFrame m_vectorImageFrames;
	};
}