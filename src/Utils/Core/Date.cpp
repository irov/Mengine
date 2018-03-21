#include "Date.h"

#include "Config/Stringstream.h"

#include <ctime>
#include <iomanip>

namespace Mengine
{
	namespace Helper
	{
		void makeDateTime( String & _date )
		{
			std::time_t ctTime;
			std::time( &ctTime );
			std::tm* sTime = std::localtime( &ctTime );

			Stringstream dateStream;
			dateStream << 1900 + sTime->tm_year
				<< "_" << std::setw( 2 ) << std::setfill( '0' ) << (sTime->tm_mon + 1)
				<< "_" << std::setw( 2 ) << std::setfill( '0' ) << sTime->tm_mday
				<< "_" << std::setw( 2 ) << std::setfill( '0' ) << sTime->tm_hour
				<< "_" << std::setw( 2 ) << std::setfill( '0' ) << sTime->tm_min
				<< "_" << std::setw( 2 ) << std::setfill( '0' ) << sTime->tm_sec;

			_date = dateStream.str();
		}
		//////////////////////////////////////////////////////////////////////////
		void makeDateTimeW( WString & _date )
		{
			std::time_t ctTime;
			std::time( &ctTime );
			std::tm* sTime = std::localtime( &ctTime );

			WStringstream dateStream;
			dateStream << 1900 + sTime->tm_year
				<< L"_" << std::setw( 2 ) << std::setfill( L'0' ) << (sTime->tm_mon + 1)
				<< L"_" << std::setw( 2 ) << std::setfill( L'0' ) << sTime->tm_mday
				<< L"_" << std::setw( 2 ) << std::setfill( L'0' ) << sTime->tm_hour
				<< L"_" << std::setw( 2 ) << std::setfill( L'0' ) << sTime->tm_min
				<< L"_" << std::setw( 2 ) << std::setfill( L'0' ) << sTime->tm_sec;

			_date = dateStream.str();
		}
	}
}