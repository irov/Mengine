#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class XmlExpatParser;
	class XmlElementListener;
	class XmlElementValueListener;

	class XmlElement
	{
	public:
		XmlElement( XmlExpatParser * _manager, const TChar * _title, const TChar ** _attrebutes );

	public:
		const TChar * getTitle() const;

	public:
		bool compareTitle( const TChar * _title );
		bool compareAttributeKey( const TChar * _title );

		void pushListener( XmlElementListener * _listener );
		void setValueListener( XmlElementValueListener * _listener );

	public:
		void beginAttributes();
		bool validAttributes();
		void nextAttributes();

		const TChar * getAttributeValue();

	protected:
		XmlExpatParser * m_expat;

		const TChar * m_title;
		const TChar ** m_attrebutes;
		unsigned int m_attrIterator;
	};
}