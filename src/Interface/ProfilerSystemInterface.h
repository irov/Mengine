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

	struct ResourceStat
	{
		String name;
		int numCompiled;
		int numReleased;
	};

	typedef std::list<ProfileHistory> ProfileHistoryList;
	typedef std::vector<ResourceStat*> ProfileResourceVec;

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
		virtual void addResourceToProfile(const Menge::String & _name) = 0;
		virtual void removeResourceToProfile(const Menge::String & _name) = 0;
		virtual const ProfileResourceVec & getProfileResourceList() const = 0;
		virtual int getTotalReleased() const = 0;
		virtual int getTotalCompiled() const = 0;
		virtual const Menge::String & getCurrentCompiled() const = 0;
	};
}

bool	initInterfaceSystem( Menge::ProfilerSystemInterface** _ptrInterface);
void	releaseInterfaceSystem( Menge::ProfilerSystemInterface* _ptrInterface);