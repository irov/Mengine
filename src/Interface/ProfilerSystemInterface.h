#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	struct ProfileHistory
	{
		Menge::String	name;
		double	currentTime; // %
		double	maxTime; // %
		double	minTime; // %
		unsigned int	numCallsThisFrame;
		double	totalTime; // %
		unsigned long	totalCalls; // %
		unsigned int	hierarchicalLvl;
	};

	typedef std::list<ProfileHistory> ProfileHistoryList;

	class ProfilerSystemInterface
	{
	public:
		virtual void reset() = 0;
		virtual void beginProfile(const String& profileName) = 0;
		virtual void endProfile(const String& profileName) = 0;
		virtual void setEnabled(bool enabled) = 0;
		virtual bool getEnabled() const = 0;
		virtual void enableProfile(const String& profileName) = 0;
		virtual void disableProfile(const String& profileName) = 0;
		virtual void setUpdateDisplayFrequency(unsigned int freq) = 0;
		virtual unsigned int getUpdateDisplayFrequency() const = 0;
		virtual const ProfileHistoryList & getProfileHistoryList() const = 0;
	};
}

bool	initInterfaceSystem( Menge::ProfilerSystemInterface** _ptrInterface);
void	releaseInterfaceSystem( Menge::ProfilerSystemInterface* _ptrInterface);