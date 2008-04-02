#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

class RenderImageInterface;

namespace Menge
{
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
			mt::vec2f size;
			RenderImageInterface * image;
		};

	public:
		//! Возвращает количество изображений
		/*!
		\return количество изображений
		*/

		virtual unsigned int getCount() const = 0;

		//! Возвращает максимальный размер frame изображения
		/*!
		\param _frame индекс изображения
		\return максимальный размер
		*/
		virtual const mt::vec2f & getMaxSize( unsigned int _frame ) const = 0;

		//! Возвращает размер frame изображения
		/*!
		\param _frame индекс изображения
		\return размер
		*/
		virtual const mt::vec2f & getSize( unsigned int _frame ) const = 0;

		//! Возвращает смещение frame изображения
		/*!
		\param _frame индекс изображения
		\return смещение
		*/
		virtual const mt::vec2f & getOffset( unsigned int _frame ) const = 0;

		//! Возвращает текстурные координаты frame изображения
		/*!
		\param _frame индекс изображения
		\return текстурные координаты
		*/
		virtual const mt::vec4f & getUV( unsigned int _frame ) const = 0;		

		//! Возвращает frame изображение
		/*!
		\param _frame индекс изображения
		\return изображение
		*/
		virtual const RenderImageInterface * getImage( unsigned int _frame ) const = 0;

	public:
		void loader( XmlElement * _xml ) override;

	protected:

		//! Возвращает изображение
		/*!
		\param _filename имя изображения
		\param _buff буффер данных с изображением
		\return изображение
		*/
		ImageFrame loadImageFrame( const std::string & _filename );
		void releaseImageFrame( const ImageFrame & _frame );

	protected:
		unsigned int m_filter;
	};
}