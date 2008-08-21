#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class RenderImageInterface;

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
		//! Возвращает количество изображений
		/*!
		\return количество изображений
		*/

		virtual std::size_t getCount() const = 0;

		//! Возвращает максимальный размер frame изображения
		/*!
		\param _frame индекс изображения
		\return максимальный размер
		*/
		virtual const mt::vec2f & getMaxSize( std::size_t _frame ) const = 0;

		//! Возвращает размер frame изображения
		/*!
		\param _frame индекс изображения
		\return размер
		*/
		virtual const mt::vec2f & getSize( std::size_t _frame ) const = 0;

		//! Возвращает смещение frame изображения
		/*!
		\param _frame индекс изображения
		\return смещение
		*/
		virtual const mt::vec2f & getOffset( std::size_t _frame ) const = 0;

		//! Возвращает текстурные координаты frame изображения
		/*!
		\param _frame индекс изображения
		\return текстурные координаты
		*/
		virtual const mt::vec4f & getUV( std::size_t _frame ) const = 0;		

		virtual bool isAlpha( std::size_t _frame ) const = 0;

		//! Возвращает frame изображение
		/*!
		\param _frame индекс изображения
		\return изображение
		*/
		virtual const RenderImageInterface * getImage( std::size_t _frame ) const = 0;

	public:
		void loader( XmlElement * _xml ) override;

	protected:

		//! Возвращает изображение
		/*!
		\param _filename имя изображения
		\param _buff буффер данных с изображением
		\return изображение
		*/
		ImageFrame loadImageFrame( const String& _filename );
		void releaseImageFrame( const ImageFrame & _frame );

	protected:
		unsigned int m_filter;
	};
}