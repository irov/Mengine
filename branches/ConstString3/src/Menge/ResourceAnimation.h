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
		ResourceAnimation();

	public:

		//! Возвращает количество изображений
		/*!
		\return количество изображений
		*/
		virtual std::size_t getSequenceCount() const;

		//! Возвращает задержку для _sequence кадра
		/*!
		\param _sequence индекс кадра
		\return задержка
		*/
		virtual float getSequenceDelay( std::size_t _sequence ) const;

		
		//! Возвращает индекс изображения
		/*!
		\param _sequence индекс кадра
		\return индекс изображения
		*/
		virtual std::size_t getSequenceIndex( std::size_t _sequence ) const;

	public:
		void loader( BinParser * _parser ) override;
		void loaderSequences_( BinParser * _parser );

	protected:
		bool _compile() override;

	private:
		struct Sequence
		{
			float delay;
			std::size_t index;
		};

		typedef std::vector<Sequence> TVectorSequence;
		TVectorSequence m_vectorSequence;
	};
}
