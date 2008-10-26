#	pragma once

#	include "Interface/ProfilerSystemInterface.h"

#	include <fstream>
#	include <map>
#	include <list>

class MengeProfileSystem
	: public Menge::ProfilerSystemInterface
{
private:
	Menge::TimerInterface * m_timer;
public:
	MengeProfileSystem();
	~MengeProfileSystem();

	void setTimer(Menge::TimerInterface * _timer) override;

	void beginProfile(const Menge::String& profileName) override;
	void endProfile(const Menge::String& profileName) override;
	void setEnabled(bool enabled) override;
	bool getEnabled() const override;
	void enableProfile(const Menge::String& profileName) override;
	void disableProfile(const Menge::String& profileName) override;
	void reset() override;
	void setUpdateDisplayFrequency(unsigned int freq) override;
	unsigned int getUpdateDisplayFrequency() const override;
	const Menge::ProfileHistoryList & getProfileHistoryList() const override;
	const Menge::ProfileResourceVec & getProfileResourceList() const override;

	bool watchForMax(const Menge::String& profileName);
	bool watchForMin(const Menge::String& profileName);
	bool watchForLimit(const Menge::String& profileName, double limit, bool greaterThan = true);
	
	void addResourceToProfile(const Menge::String & _name) override;
	void removeResourceToProfile(const Menge::String & _name) override;

	int getTotalReleased() const;
	int getTotalCompiled() const;
	const Menge::String & getCurrentCompiled() const;
private:
	void changeEnableState();

	void processFrameStats();

	//Timer mTimer;
	/// Represents an individual profile call
	struct ProfileInstance {

		/// The name of the profile
		Menge::String		name;

		/// The name of the parent, empty string if root
		Menge::String		parent;

		/// The time this profile was started
		unsigned long		currTime;

		/// Represents the total time of all child profiles to subtract
		/// from this profile
		unsigned long		accum;

		/// The hierarchical level of this profile, 0 being the root profile
		unsigned int		hierarchicalLvl;
	};

	/// Represents the total timing information of a profile
	/// since profiles can be called more than once each frame
	struct ProfileFrame {

		/// The name of the profile
		Menge::String	name;

		/// The total time this profile has taken this frame
		unsigned long	frameTime;

		/// The number of times this profile was called this frame
		unsigned int	calls;

		/// The hierarchical level of this profile, 0 being the main loop
		unsigned int	hierarchicalLvl;

	};

	typedef std::list<ProfileInstance> ProfileStack;
	typedef std::list<ProfileFrame> ProfileFrameList;
	typedef std::list<Menge::ProfileHistory> ProfileHistoryList;
	typedef std::map<Menge::String, ProfileHistoryList::iterator> ProfileHistoryMap;
	typedef std::map<Menge::String, bool> DisabledProfileMap;

	ProfileStack mProfiles;
	ProfileFrameList mProfileFrame;
	ProfileHistoryList mProfileHistory;
	ProfileHistoryMap mProfileHistoryMap;
	DisabledProfileMap mDisabledProfiles;

	unsigned int maxProfiles;
	unsigned int mUpdateDisplayFrequency;
	unsigned int mCurrentFrame;
	unsigned long mTotalFrameTime;
	bool mEnabled;
	bool mEnableStateChangePending;
	bool mNewEnableState;

private:
	Menge::String m_currentCompiledResource;

	int m_totalCompiled;
	int m_totalReleased;

	typedef std::map<Menge::String, Menge::ResourceStat*> ProfileResourceMap;

	Menge::ProfileResourceVec m_profileResourceVec;
	ProfileResourceMap m_profileResourceMap;
};
