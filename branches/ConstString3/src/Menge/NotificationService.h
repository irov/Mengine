#	pragma once

#	include "Interface/NotificationServiceInterace.h"

#	include <Core/Holder.h>

#	include <vector>
#	include <map>

namespace Menge
{
	class NotificationService
		: public NotificationServiceInterface
		, public Holder<NotificationService>
	{
	public:
		void addObserver( const String & _id, Observer * _observer ) override;
		void removeObserver( const String & _id, Observer * _observer ) override;

	protected:	
		void visitObservers( const String & _id, VisitorObserver * _visitor ) override;
		
	protected:
		void invalidObserver_( const String & _id );

	protected:		
		typedef std::vector<Observer *> TVectorObservers;
		typedef std::map<String, TVectorObservers> TMapObservers;
		TMapObservers m_mapObserves;		
	};	
}