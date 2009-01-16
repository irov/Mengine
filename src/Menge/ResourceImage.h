#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class RenderImageInterface;

	//! ResourceImage - интерфейс дл€ ресурсов, которые работают с избражени€ми.
	/*! 
	*
	*/

	class ResourceImage
		: public ResourceReference
	{
	public:
		//!  онструктор.
		/*!
		\param _name им€ ресурса.
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
		//! ¬озвращает количество изображений
		/*!
		\return количество изображений
		*/

		virtual std::size_t getCount() const = 0;

		//! ¬озвращает максимальный размер frame изображени€
		/*!
		\param _frame индекс изображени€
		\return максимальный размер
		*/
		virtual const mt::vec2f & getMaxSize( std::size_t _frame ) const = 0;

		//! ¬озвращает размер frame изображени€
		/*!
		\param _frame индекс изображени€
		\return размер
		*/
		virtual const mt::vec2f & getSize( std::size_t _frame ) const = 0;

		//! ¬озвращает смещение frame изображени€
		/*!
		\param _frame индекс изображени€
		\return смещение
		*/
		virtual const mt::vec2f & getOffset( std::size_t _frame ) const = 0;

		//! ¬озвращает текстурные координаты frame изображени€
		/*!
		\param _frame индекс изображени€
		\return текстурные координаты
		*/
		virtual const mt::vec4f & getUV( std::size_t _frame ) const = 0;		

		virtual bool isAlpha( std::size_t _frame ) const = 0;

		//! ¬озвращает frame изображение
		/*!
		\param _frame индекс изображени€
		\return изображение
		*/
		virtual const RenderImageInterface * getImage( std::size_t _frame ) const = 0;

		//! ¬озвращает название файла изображени€
		/*!
		\param _frame индекс изображени€
		\return им€ файла изображени€
		*/
		virtual const String & getFilename( std::size_t _frame ) const = 0;

		virtual std::size_t getFilenameCount() const = 0;

	public:
		void loader( XmlElement * _xml ) override;

	protected:

		//! ¬озвращает изображение
		/*!
		\param _filename им€ изображени€
		\param _buff буффер данных с изображением
		\return изображение
		*/
		ImageFrame loadImageFrame( const String& _filename );
		ImageFrame createImageFrame( const String& _name, const mt::vec2f& _size );
		void releaseImageFrame( const ImageFrame & _frame );

	protected:
		std::size_t m_filter;
	};
}
