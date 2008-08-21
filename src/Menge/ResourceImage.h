#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class RenderImageInterface;

	//! ResourceImage - ��������� ��� ��������, ������� �������� � ������������.
	/*! 
	*
	*/

	class ResourceImage
		: public ResourceReference
	{
	public:
		//! �����������.
		/*!
		\param _name ��� �������.
		*/
		ResourceImage( const ResourceFactoryParam & _params );

		struct ImageFrame
		{
			mt::vec2f maxSize;
			mt::vec2f size;
			mt::vec2f offset;
			mt::vec4f uv;
			bool isAlpha;
			RenderImageInterface * image;
		};

	public:
		//! ���������� ���������� �����������
		/*!
		\return ���������� �����������
		*/

		virtual std::size_t getCount() const = 0;

		//! ���������� ������������ ������ frame �����������
		/*!
		\param _frame ������ �����������
		\return ������������ ������
		*/
		virtual const mt::vec2f & getMaxSize( std::size_t _frame ) const = 0;

		//! ���������� ������ frame �����������
		/*!
		\param _frame ������ �����������
		\return ������
		*/
		virtual const mt::vec2f & getSize( std::size_t _frame ) const = 0;

		//! ���������� �������� frame �����������
		/*!
		\param _frame ������ �����������
		\return ��������
		*/
		virtual const mt::vec2f & getOffset( std::size_t _frame ) const = 0;

		//! ���������� ���������� ���������� frame �����������
		/*!
		\param _frame ������ �����������
		\return ���������� ����������
		*/
		virtual const mt::vec4f & getUV( std::size_t _frame ) const = 0;		

		virtual bool isAlpha( std::size_t _frame ) const = 0;

		//! ���������� frame �����������
		/*!
		\param _frame ������ �����������
		\return �����������
		*/
		virtual const RenderImageInterface * getImage( std::size_t _frame ) const = 0;

	public:
		void loader( XmlElement * _xml ) override;

	protected:

		//! ���������� �����������
		/*!
		\param _filename ��� �����������
		\param _buff ������ ������ � ������������
		\return �����������
		*/
		ImageFrame loadImageFrame( const String& _filename );
		void releaseImageFrame( const ImageFrame & _frame );

	protected:
		unsigned int m_filter;
	};
}