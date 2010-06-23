#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class Texture;

	//! ResourceImage - интерфейс для ресурсов, которые работают с избражениями.
	/*! 
	*
	*/

	class ResourceImage
		: public ResourceReference
	{
	public:
		//! Конструктор.
		/*!
		\param _name имя ресурса.
		*/
		ResourceImage();

		struct ImageFrame
		{
			mt::vec2f maxSize;
			mt::vec2f size;
			mt::vec2f offset;
			mt::vec4f uv;
			bool isAlpha;
			Texture* texture;
			bool wrapX;
			bool wrapY;
		};

	public:
		virtual std::size_t getCount() const = 0;
		virtual const mt::vec2f & getMaxSize( std::size_t _frame ) const = 0;
		virtual const mt::vec2f & getSize( std::size_t _frame ) const = 0;
		virtual const mt::vec2f & getOffset( std::size_t _frame ) const = 0;
		virtual const mt::vec4f & getUV( std::size_t _frame ) const = 0;		

		virtual bool isAlpha( std::size_t _frame ) const = 0;

		virtual Texture* getTexture( std::size_t _frame ) = 0;
		virtual const ConstString & getFilename( std::size_t _frame ) const = 0;
		virtual const ConstString & getCodecType( std::size_t _frame ) const = 0;
		virtual std::size_t getFilenameCount() const = 0;

		virtual bool getWrapX( std::size_t _frame ) const;
		virtual bool getWrapY( std::size_t _frame ) const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:

		//! Возвращает изображение
		/*!
		\param _filename имя изображения
		\param _buff буффер данных с изображением
		\return изображение
		*/
		ImageFrame loadImageFrame_( const ConstString& _pakName, const ConstString& _filename ) const;
		ImageFrame createImageFrame_( const ConstString& _name, const mt::vec2f& _size ) const;
		ImageFrame createRenderTargetFrame_( const ConstString& _name, const mt::vec2f& _size ) const;
		void releaseImageFrame_( const ImageFrame & _frame ) const;

		ConstString getImageType_( const String & _filename ) const;

	protected:
		std::size_t m_filter;
	};
}
