#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include <vector>

class RenderImageInterface;

namespace Menge
{
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
		ResourceImage( const std::string & _name );

		struct ImageFrame
		{
			mt::vec2f size;
			RenderImageInterface * image;
		};

	public:
		//! ���������� ���������� �����������
		/*!
		\return ���������� �����������
		*/

		virtual size_t getCount() = 0;

		//! ���������� ������������ ������ frame �����������
		/*!
		\param _frame ������ �����������
		\return ������������ ������
		*/
		virtual const mt::vec2f & getMaxSize( size_t _frame ) const = 0;

		//! ���������� ������ frame �����������
		/*!
		\param _frame ������ �����������
		\return ������
		*/
		virtual const mt::vec2f & getSize( size_t _frame ) const = 0;

		//! ���������� �������� frame �����������
		/*!
		\param _frame ������ �����������
		\return ��������
		*/
		virtual const mt::vec2f & getOffset( size_t _frame ) const = 0;

		//! ���������� ���������� ���������� frame �����������
		/*!
		\param _frame ������ �����������
		\return ���������� ����������
		*/
		virtual const mt::vec4f & getUV( size_t _frame ) const = 0;		

		//! ���������� frame �����������
		/*!
		\param _frame ������ �����������
		\return �����������
		*/
		virtual const RenderImageInterface * getImage( size_t _frame ) const = 0;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:

		//! ���������� �����������
		/*!
		\param _filename ��� �����������
		\param _buff ������ ������ � ������������
		\return �����������
		*/
		ImageFrame loadImageFrame( const std::string & _filename, std::vector<char> & _buff );
		void releaseImageFrame( const ImageFrame & _frame );

	protected:
		size_t m_filter;
	};
}