#	include "ProfilerSystem.h"

#	include "Interface/ApplicationInterface.h"

#	include <iostream>
#	include <ctime>
#	include <iomanip>
#	include <algorithm>
#	include <assert.h>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::ProfilerSystemInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}
	
	*_interface = new MengeProfileSystem();
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::ProfilerSystemInterface* _interface )
{
	if( _interface )
	{
		delete static_cast<MengeProfileSystem*>( _interface );
	}
}
//////////////////////////////////////////////////////////////////////////
MengeProfileSystem::MengeProfileSystem() 
: m_totalCompiled(0)
, m_totalReleased(0)
, m_currentCompiledResource("")
{
	//m_timer->reset();
	mTotalFrameTime = 0;
	mUpdateDisplayFrequency = 0;
	mCurrentFrame = 0;
	mEnabled = mNewEnableState = false; 
	mEnableStateChangePending = false;
	maxProfiles = 50;

	mUpdateDisplayFrequency = 10;

	m_profileResourceVec.reserve(1000);
}
//-----------------------------------------------------------------------
MengeProfileSystem::~MengeProfileSystem() {

	if (!mProfileHistory.empty())
	{
	}

	mProfiles.clear();
	mProfileFrame.clear();
	mProfileHistoryMap.clear();
	mProfileHistory.clear();
	mDisabledProfiles.clear();

	for(Menge::ProfileResourceVec::iterator it = m_profileResourceVec.begin();
		it != m_profileResourceVec.end();
		it++)
	{
		delete *it;
	}

	m_profileResourceVec.clear();
	m_profileResourceMap.clear();
}
//////////////////////////////////////////////////////////////////////////
void MengeProfileSystem::setTimer(Menge::TimerInterface * _timer)
{
	m_timer = _timer;
}
//////////////////////////////////////////////////////////////////////////
void MengeProfileSystem::addResourceToProfile(const Menge::String & _name)
{
	ProfileResourceMap::iterator it = m_profileResourceMap.find(_name);

	if (it == m_profileResourceMap.end()) 
	{
		Menge::ResourceStat * stat = new Menge::ResourceStat();
		stat->name = _name;
		stat->numCompiled = 1;
		stat->numReleased = 0;
		
		m_profileResourceVec.push_back(stat);
		it = m_profileResourceMap.insert(std::make_pair(_name,stat)).first;
	}
	else
	{
		Menge::ResourceStat * stat = it->second;
		stat->numCompiled++;

		struct SortStats
		{
			bool operator()(const Menge::ResourceStat * lhs, const Menge::ResourceStat * rhs)
			{
				if(lhs == rhs)
				{
					return false;
				}

				if (lhs->numCompiled >= rhs->numCompiled)
					return true;

				else if (rhs->numCompiled >= lhs->numCompiled)
					return false;

				return false;
			}
		};

		std::sort(m_profileResourceVec.begin(),m_profileResourceVec.end(),SortStats());
	}

	m_totalCompiled++;

	m_currentCompiledResource = _name;
}
//////////////////////////////////////////////////////////////////////////
int MengeProfileSystem::getTotalReleased() const
{
	return m_totalReleased;
}
//////////////////////////////////////////////////////////////////////////
int MengeProfileSystem::getTotalCompiled() const
{
	return m_totalCompiled;
}
//////////////////////////////////////////////////////////////////////////
const Menge::String & MengeProfileSystem::getCurrentCompiled() const
{
	return m_currentCompiledResource;
}
//////////////////////////////////////////////////////////////////////////
const Menge::ProfileResourceVec & MengeProfileSystem::getProfileResourceList() const
{
	return m_profileResourceVec;
}
//////////////////////////////////////////////////////////////////////////
void MengeProfileSystem::removeResourceToProfile(const Menge::String & _name)
{
	if(mEnabled == false)
	{
		return;
	}

	ProfileResourceMap::iterator it = m_profileResourceMap.find(_name);

	if (it == m_profileResourceMap.end()) 
	{
		return;
	};

	Menge::ResourceStat & stat = *it->second;
	stat.numReleased++;
	m_totalReleased++;
}
//-----------------------------------------------------------------------
void MengeProfileSystem::setEnabled(bool enabled)
{
	mEnableStateChangePending = true;
	mNewEnableState = enabled;
}
//-----------------------------------------------------------------------
bool MengeProfileSystem::getEnabled() const
{
	return mEnabled;
}
//-----------------------------------------------------------------------
void MengeProfileSystem::disableProfile(const Menge::String& profileName) 
{
	DisabledProfileMap::iterator iter;
	iter = mDisabledProfiles.find(profileName);

	// make sure you don't disable a profile in the middle of that profile
	ProfileStack::iterator pIter;
	for (pIter = mProfiles.begin(); pIter != mProfiles.end(); ++pIter) {

		if (profileName == (*pIter).name)
			break;

	}

	// if those two conditions are met, disable the profile
	if ( (iter == mDisabledProfiles.end()) && (pIter == mProfiles.end()) ) {

		mDisabledProfiles.insert(std::pair<Menge::String, bool>(profileName, true));

	}

}
//-----------------------------------------------------------------------
void MengeProfileSystem::enableProfile(const Menge::String& profileName) {

	// make sure the profile is actually disabled
	DisabledProfileMap::iterator iter;
	iter = mDisabledProfiles.find(profileName);

	// make sure you don't enable a profile in the middle of that profile
	ProfileStack::iterator pIter;
	for (pIter = mProfiles.begin(); pIter != mProfiles.end(); ++pIter) {

		if (profileName == (*pIter).name)
			break;

	}

	// if those two conditions are met, enable the profile by removing it from
	// the disabled list
	if ( (iter != mDisabledProfiles.end()) && (pIter == mProfiles.end()) ) {

		mDisabledProfiles.erase(iter);

	}

}
//-----------------------------------------------------------------------
const Menge::ProfileHistoryList & MengeProfileSystem::getProfileHistoryList() const
{
	return mProfileHistory;
}
//-----------------------------------------------------------------------
void MengeProfileSystem::beginProfile(const Menge::String& profileName) {

	// if the profiler is enabled
	if (!mEnabled) {
		return;
	}

	// empty string is reserved for the root
	assert ((profileName != "") && ("Profile name can't be an empty string"));

	ProfileStack::iterator iter;
	for (iter = mProfiles.begin(); iter != mProfiles.end(); ++iter) {

		if ((*iter).name == profileName) {

			break;

		}

	}

	// make sure this profile isn't being used more than once
	assert ((iter == mProfiles.end()) && ("This profile name is already being used"));

	// we only process this profile if isn't disabled
	DisabledProfileMap::iterator dIter;
	dIter = mDisabledProfiles.find(profileName);
	if ( dIter != mDisabledProfiles.end() ) {

		return;

	}

	ProfileInstance p;
	p.hierarchicalLvl = static_cast<unsigned int>(mProfiles.size());

	// this is the root, it has no parent
	if (mProfiles.empty()) {

		p.parent = "";

	}
	// otherwise peek at the stack and use the top as the parent
	else {

		ProfileInstance parent = mProfiles.back();
		p.parent = parent.name;

	}

	ProfileFrameList::iterator fIter;
	ProfileHistoryList::iterator hIter;

	// we check to see if this profile has been called in the frame before
	for (fIter = mProfileFrame.begin(); fIter != mProfileFrame.end(); ++fIter) {

		if ((*fIter).name == profileName)
			break;

	}
	// if it hasn't been called before, set its position in the stack
	if (fIter == mProfileFrame.end()) {

		ProfileFrame f;
		f.name = profileName;
		f.frameTime = 0;
		f.calls = 0;
		f.hierarchicalLvl = (unsigned int) mProfiles.size();
		mProfileFrame.push_back(f);

	}

	// we check to see if this profile has been called in the app before
	ProfileHistoryMap::iterator histMapIter;
	histMapIter = mProfileHistoryMap.find(profileName);

	// if not we add a profile with just the name into the history
	if (histMapIter == mProfileHistoryMap.end()) {

		Menge::ProfileHistory h;
		h.name = profileName;
		h.numCallsThisFrame = 0;
		h.totalTime = 0;
		h.totalCalls = 0;
		h.maxTime = 0;
		h.minTime = 1;
		h.hierarchicalLvl = p.hierarchicalLvl;
		h.currentTime = 0;

		// we add this to the history
		hIter = mProfileHistory.insert(mProfileHistory.end(), h);

		// for quick look-ups, we'll add it to the history map as well
		mProfileHistoryMap.insert(std::pair<Menge::String, ProfileHistoryList::iterator>(profileName, hIter));

	}

	// add the stats to this profile and push it on the stack
	// we do this at the very end of the function to get the most
	// accurate timing results
	p.name = profileName;
	p.currTime = m_timer->getMicroseconds();
	p.accum = 0;
	mProfiles.push_back(p);

}
//-----------------------------------------------------------------------
void MengeProfileSystem::endProfile(const Menge::String& profileName) 
{
	// if the profiler received a request to be enabled or disabled
	// we reached the end of the frame so we can safely do this
	if (mEnableStateChangePending)
	{
		changeEnableState();
	}

	// if the profiler is enabled
	if(!mEnabled) {

		return;

	}

	if(mProfiles.empty() == true) //ALARM
	{
		return;
	}

	unsigned long endTime = m_timer->getMicroseconds();

	assert ((profileName != "") && ("Profile name can't be an empty string"));

	DisabledProfileMap::iterator dIter;
	dIter = mDisabledProfiles.find(profileName);
	if ( dIter != mDisabledProfiles.end() ) 
	{
		return;
	}

	assert (!mProfiles.empty());

	ProfileInstance bProfile;
	bProfile = mProfiles.back();
	mProfiles.pop_back();

	unsigned long timeElapsed = endTime - bProfile.currTime;

	if (bProfile.parent != "" ) 
	{

		// find the parent
		ProfileStack::iterator iter;
		for(iter = mProfiles.begin(); iter != mProfiles.end(); ++iter) {

			if ((*iter).name == bProfile.parent)
				break;

		}

		assert(iter != mProfiles.end());

		(*iter).accum += timeElapsed;

	}

	ProfileFrameList::iterator iter;
	for (iter = mProfileFrame.begin(); iter != mProfileFrame.end(); ++iter) {

		if ((*iter).name == bProfile.name)
			break;

	}

	(*iter).frameTime += timeElapsed - bProfile.accum;
	(*iter).calls++;

	if (mProfiles.empty()) 
	{
		mTotalFrameTime = timeElapsed;
		processFrameStats();
		mProfileFrame.clear();
	}
}
//-----------------------------------------------------------------------
void MengeProfileSystem::processFrameStats() {

	ProfileFrameList::iterator frameIter;
	ProfileHistoryList::iterator historyIter;

	for (historyIter = mProfileHistory.begin(); historyIter != mProfileHistory.end(); ++historyIter) {

		(*historyIter).numCallsThisFrame = 0;

	}

	for (frameIter = mProfileFrame.begin(); frameIter != mProfileFrame.end(); ++frameIter) {

		Menge::String s = (*frameIter).name;

		historyIter = (*mProfileHistoryMap.find(s)).second;

		unsigned long frameTime = (*frameIter).frameTime;
		unsigned int calls = (*frameIter).calls;
		unsigned int lvl = (*frameIter).hierarchicalLvl;

		double framePercentage = (double) frameTime / (double) mTotalFrameTime;

		(*historyIter).currentTime = framePercentage;
		(*historyIter).totalTime += framePercentage;
		(*historyIter).totalCalls++;
		(*historyIter).numCallsThisFrame = calls;
		(*historyIter).hierarchicalLvl = lvl;

		if ((framePercentage) < ((*historyIter).minTime)) {
			(*historyIter).minTime = framePercentage;
		}

		if ((framePercentage) > ((*historyIter).maxTime)) {
			(*historyIter).maxTime = framePercentage;
		}

	}

}
//-----------------------------------------------------------------------
bool MengeProfileSystem::watchForMax(const Menge::String& profileName) {

	ProfileHistoryMap::iterator mapIter;
	ProfileHistoryList::iterator iter;

	mapIter = mProfileHistoryMap.find(profileName);

	if (mapIter == mProfileHistoryMap.end())
		return false;

	iter = (*mapIter).second;

	return ((*iter).currentTime == (*iter).maxTime);

}
//-----------------------------------------------------------------------
bool MengeProfileSystem::watchForMin(const Menge::String& profileName) {

	ProfileHistoryMap::iterator mapIter;
	ProfileHistoryList::iterator iter;

	mapIter = mProfileHistoryMap.find(profileName);

	if (mapIter == mProfileHistoryMap.end())
		return false;

	iter = (*mapIter).second;

	return ((*iter).currentTime == (*iter).minTime);

}
//-----------------------------------------------------------------------
bool MengeProfileSystem::watchForLimit(const Menge::String& profileName, double limit, bool greaterThan) {

	ProfileHistoryMap::iterator mapIter;
	ProfileHistoryList::iterator iter;

	mapIter = mProfileHistoryMap.find(profileName);

	if (mapIter == mProfileHistoryMap.end())
		return false;

	iter = (*mapIter).second;

	if (greaterThan)
		return ((*iter).currentTime > limit);
	else
		return ((*iter).currentTime < limit);

}
//-----------------------------------------------------------------------
void MengeProfileSystem::reset() {

	ProfileHistoryList::iterator iter;
	for (iter = mProfileHistory.begin(); iter != mProfileHistory.end(); ++iter) {
		(*iter).currentTime = (*iter).maxTime = (*iter).totalTime = 0;
		(*iter).numCallsThisFrame = (*iter).totalCalls = 0;
		(*iter).minTime = 1;
	}

}
//-----------------------------------------------------------------------
void MengeProfileSystem::setUpdateDisplayFrequency(unsigned int freq) {

	mUpdateDisplayFrequency = freq;

}
//-----------------------------------------------------------------------
unsigned int MengeProfileSystem::getUpdateDisplayFrequency() const {

	return mUpdateDisplayFrequency;

}
//-----------------------------------------------------------------------
void MengeProfileSystem::changeEnableState() {

	if (mNewEnableState) 
	{
	}
	else 
	{

	}
	mEnabled = mNewEnableState;
	mEnableStateChangePending = false;
}
//-----------------------------------------------------------------------
