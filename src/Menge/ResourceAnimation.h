#	pragma once

#	include "ResourceReference.h"

#	include <vector>

namespace Menge
{
	//! ResourceAnimation - ресурс-файл который содержит список пар (индекс, задержка).

    /** xml - файл имеет следующую структуру:
	 *<Resource Name = "имя_ресурса" Type = "ResourceAnimation" >
	 *	<Sequences>
	 *		<Sequence Index = "индекс_изображения0" Delay = "задержка0" />
	 *			...
	 *		<Sequence Index = "индекс_изображенияN" Delay = "задержкаN" />
	 * 	</Sequences>
	 *</Resource>
	*/

	class ResourceAnimation
		: public ResourceReference
	{
		 RESOURCE_DECLARE( ResourceAnimation )

	public:
		//! Конструктор.
		/*!
		\param _name имя ресурса.
		*/
		ResourceAnimation( const ResourceFactoryParam & _params );

	public:

		//! Возвращает количество изображений
		/*!
		\return количество изображений
		*/
		virtual unsigned int getSequenceCount() const;

		//! Возвращает задержку для _sequence кадра
		/*!
		\param _sequence индекс кадра
		\return задержка
		*/
		virtual float getSequenceDelay( unsigned int _sequence ) const;

		
		//! Возвращает индекс изображения
		/*!
		\param _sequence индекс кадра
		\return индекс изображения
		*/
		virtual unsigned int getSequenceIndex( unsigned int _sequence ) const;

	public:
		void loader( XmlElement * _xml );
		void loaderSequences_( XmlElement * _xml );

	private:
		struct Sequence
		{
			float delay;
			unsigned int index;
		};

		typedef std::vector<Sequence> TVectorSequence;
		TVectorSequence m_vectorSequence;
	};
}