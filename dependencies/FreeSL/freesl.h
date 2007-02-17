#ifndef __FREESL_H
#define __FREESL_H


#ifdef FREESL_EXPORTS
#define FSLAPI __declspec(dllexport)
#define FSLAPIENTRY __cdecl
#else
#define FSLAPI __declspec(dllimport)
#define FSLAPIENTRY __cdecl
#endif


// Typedefs
typedef unsigned int FSLenum;
typedef unsigned int FSLsound;
typedef unsigned int FSLbuffer;
typedef void FSLerror_callback(const char* str, bool is_critical);	// if (is_critical) then we must shutdown FreeSL


#define FSL_VERSION			int(240)


// Distance Models
#define FSL_INVERSE_DISTANCE                       0xD001
#define FSL_INVERSE_DISTANCE_CLAMPED               0xD002
#define FSL_LINEAR_DISTANCE                        0xD003
#define FSL_LINEAR_DISTANCE_CLAMPED                0xD004
#define FSL_EXPONENT_DISTANCE                      0xD005
#define FSL_EXPONENT_DISTANCE_CLAMPED              0xD006


// Sound Format
#define FSL_FORMAT_MONO8                           0x1100
#define FSL_FORMAT_MONO16                          0x1101
#define FSL_FORMAT_STEREO8                         0x1102
#define FSL_FORMAT_STEREO16                        0x1103

// Sound System
enum FSL_SOUND_SYSTEM
{
	FSL_SS_DIRECTSOUND3D,					// Direct Sound 3D
	FSL_SS_DIRECTSOUND,						// Direct Sound
	FSL_SS_NVIDIA_NFORCE_2,					// nVidia nForce 2
	FSL_SS_CREATIVE_AUDIGY_2,				// Creative Audigy 2
	FSL_SS_MMSYSTEM,						// Microsoft
	FSL_SS_ALUT,							// ALUT
	
	FSL_SS_NOSYSTEM							// no sound system
};

struct FSL_SOURCE_INFO
{
	unsigned int uiChannels;		// example: 1 = mono, 2 = stereo
	unsigned int uiSampling;		// example: 16 bit
	unsigned int uiFrequency;		// example: 44100 Hz
};


struct FSLAPI Callback
{
	virtual ~Callback(){};
	virtual void call()=0;
};

// Initialization, Shutdown and Common Functions
FSLAPI bool FSLAPIENTRY fslInit(FSL_SOUND_SYSTEM val);									// main initialization function
FSLAPI void FSLAPIENTRY fslShutDown();													// shutdown FreeSL and free any allocated memory
FSLAPI void FSLAPIENTRY fslUpdate();													// per-frame update stuff (must be called if there are streaming sounds)
FSLAPI void FSLAPIENTRY fslSetAutoUpdate(bool do_automaticly);							// dont want to call fslUpdate? Then let FreeSL do it for you
FSLAPI int FSLAPIENTRY fslGetVersion();													// get the current version of FreeSL (should be equal to FSL_VERSION)
FSLAPI void FSLAPIENTRY fslSetErrorCallback(FSLerror_callback* func);					// set error callback function
FSLAPI void FSLAPIENTRY fslSleep(float duration);										// delay the execution of the current thread for a number of seconds

// General Functions
FSLAPI void FSLAPIENTRY fslSetVolume(float gain_mult);									// multiply all sounds with the following gain value
FSLAPI int FSLAPIENTRY fslCountSoundsStreaming();										// get the number of sounds sources that are playing and that are stream sounds
FSLAPI int FSLAPIENTRY fslCountSoundsTotal();											// get the total number of sound sources
FSLAPI unsigned long FSLAPIENTRY fslGetSoundMemoryUsage();								// return the memory usage for the sounds (not including stream sounds)

// Sound Creation and Loading
FSLAPI char* FSLAPIENTRY fslGetSupportedSoundFileFormats();								// returns a string with the extension names of the supported sound files
FSLAPI char* FSLAPIENTRY fslGetSupportedSoundStreamFormats();							// returns a string with the extension names of the supported stream sound files
FSLAPI int FSLAPIENTRY fslGetMaxSources();												// return the amount of sound sources supported by the current hardware
FSLAPI FSLsound FSLAPIENTRY fslCreateEmptySound();										// create a sound object that has not buffer
FSLAPI FSLsound FSLAPIENTRY fslCreateDummySound();										// create an empty sound object
FSLAPI FSLsound FSLAPIENTRY fslCreateSoundFromBuffer(FSLbuffer buffer);					// create a sound source using a pre-defined sound buffer
FSLAPI FSLsound FSLAPIENTRY fslLoadSoundFromData(const char* strFileFormat,
												 const void* pData, unsigned int size);	// load a sound from data
FSLAPI FSLsound FSLAPIENTRY fslCreateSound(const void* pData, FSLenum format,
										   unsigned int size, unsigned int freq);		// creates a sound from data
FSLAPI FSLsound FSLAPIENTRY fslStreamSound(const void* pData, unsigned int isize);
FSLAPI FSLsound FSLAPIENTRY fslCloneSound(FSLsound obj);								// create a new sound source, copy the buffer
FSLAPI void FSLAPIENTRY fslFreeSound(FSLsound obj, bool remove_buffer = true);			// destroy sound source
FSLAPI int FSLAPIENTRY fslFreeAllSounds();												// destroy all loaded/created sound sources (returns remove count)

// Sound Buffer Functions
FSLAPI FSLbuffer FSLAPIENTRY fslGetBufferFromSound(FSLsound obj);						// retrieve the sound's buffer
FSLAPI void FSLAPIENTRY fslSetSoundBuffer(FSLsound obj, FSLbuffer buffer);				// set the buffer for a sound (if you want to replace the current buffer then you have to destroy it)
FSLAPI FSLbuffer FSLAPIENTRY fslCreateBuffer();											// create an empty buffer
FSLAPI void FSLAPIENTRY fslDestroyBuffer(FSLbuffer obj);								// destroy buffer
FSLAPI void FSLAPIENTRY fslSetBufferData(FSLbuffer obj, FSLenum format,
										 const void* pData, unsigned int size,
										 unsigned int freq);							// set the data for a buffer
FSLAPI void FSLAPIENTRY fslSoundQueueBuffers(FSLsound obj, unsigned int num_buffers,
											 FSLbuffer* buffers);						// queue buffers for a sound
FSLAPI void FSLAPIENTRY fslSoundUnqueueBuffers(FSLsound obj, unsigned int num_buffers,
											   FSLbuffer* buffers);						// unqueue buffers for a sound
FSLAPI FSLbuffer FSLAPIENTRY fslLoadBufferFromData(const char* strFileFormat,
												   const void* pData, unsigned int size);// load a sound buffer from data
FSLAPI int FSLAPIENTRY fslFreeAllBuffers();												// destroy all loaded/created buffers (returns remove count)
FSLAPI FSL_SOURCE_INFO FSLAPIENTRY fslBufferGetInfo(FSLbuffer obj);						// get info about the sound buffer

//CALLBACKS
FSLAPI void FSLAPIENTRY fslSoundCallbackPause(FSLsound obj, Callback*);
FSLAPI void FSLAPIENTRY fslSoundCallbackStop(FSLsound obj, Callback*);
// Sound Functions
FSLAPI void FSLAPIENTRY fslSoundPlay(FSLsound obj);										// play the sound
FSLAPI void FSLAPIENTRY fslSoundRewind(FSLsound obj);									// rewind the sound
FSLAPI void FSLAPIENTRY fslSoundStop(FSLsound obj);										// stop the sound from playing
FSLAPI void FSLAPIENTRY fslSoundPause(FSLsound obj);									// pause the sound
FSLAPI bool FSLAPIENTRY fslSoundIsPlaying(FSLsound obj);								// is the sound playing?
FSLAPI bool FSLAPIENTRY fslSoundIsPaused(FSLsound obj);									// is the sound paused?
FSLAPI bool FSLAPIENTRY fslSoundIsStopped(FSLsound obj);									// is the sound paused?

FSLAPI void FSLAPIENTRY fslSoundSetLooping(FSLsound obj, bool loop_sound);				// should the sound loop?
FSLAPI void FSLAPIENTRY fslSoundSetPositionVec(FSLsound obj, float* position);			// set position (float[3])
FSLAPI void FSLAPIENTRY fslSoundSetPosition(FSLsound obj, float x, float y, float z);	// set position
FSLAPI void FSLAPIENTRY fslSoundSetVelocityVec(FSLsound obj, float* velocity);			// set velocity (float[3])
FSLAPI void FSLAPIENTRY fslSoundSetVelocity(FSLsound obj, float x, float y, float z);	// set velocity
FSLAPI void FSLAPIENTRY fslSoundSetPitch(FSLsound obj, float pitch);					// set pitch (0,1]
FSLAPI void FSLAPIENTRY fslSoundSetGain(FSLsound obj, float gain);						// set gain (0,oo]
FSLAPI void FSLAPIENTRY fslSoundSetMaxDistance(FSLsound obj, float max_distance);		// set max distance
FSLAPI void FSLAPIENTRY fslSoundSetReferenceDistance(FSLsound obj, float ref_distance);	// set reference distance
FSLAPI void FSLAPIENTRY fslSoundSetConeOrientationVec(FSLsound obj, float* direction);	// set cone direction
FSLAPI void FSLAPIENTRY fslSoundSetConeOrientation(FSLsound obj,
												   float x, float y, float z);			// set cone direction
FSLAPI void FSLAPIENTRY fslSoundSetConeInsideAngle(FSLsound obj, unsigned int val);		// set inside angle
FSLAPI void FSLAPIENTRY fslSoundSetConeOutsideAngle(FSLsound obj, unsigned int val);	// set outside angle
FSLAPI void FSLAPIENTRY fslSoundSetConeOutsideVolume(FSLsound obj, int val);			// set outside volume

FSLAPI bool FSLAPIENTRY fslSoundIsLooping(FSLsound obj);								// is sound looping?
FSLAPI void FSLAPIENTRY fslSoundGetPosition(FSLsound obj, 
											float& x, float& y, float& z);				// get position
FSLAPI void FSLAPIENTRY fslSoundGetVelocity(FSLsound obj, 
											float& x, float& y, float& z);				// get velocity
FSLAPI float FSLAPIENTRY fslSoundGetPitch(FSLsound obj);								// get pitch
FSLAPI float FSLAPIENTRY fslSoundGetGain(FSLsound obj);									// get gain
FSLAPI float FSLAPIENTRY fslSoundGetMaxDistance(FSLsound obj);							// get max distance
FSLAPI float FSLAPIENTRY fslSoundGetReferenceDistance(FSLsound obj);					// get reference distance
FSLAPI void FSLAPIENTRY fslSoundGetConeOrientation(FSLsound obj, 
												   float& x, float& y, float& z);		// get cone orientation
FSLAPI unsigned int FSLAPIENTRY fslSoundGetConeInsideAngle(FSLsound obj);				// get cone inside angle
FSLAPI unsigned int FSLAPIENTRY fslSoundGetConeOutsideAngle(FSLsound obj);				// get cone outside angle
FSLAPI int FSLAPIENTRY fslSoundGetConeOutsideVolume(FSLsound obj);						// get cone outside volume
FSLAPI void FSLAPIENTRY fslSoundSetSourceRelative(FSLsound obj, bool is_relative);		// should the sound be relative to the listener
FSLAPI bool FSLAPIENTRY fslSoundIsSourceRelative(FSLsound obj);							// is the sound relative to the listener
FSLAPI void FSLAPIENTRY fslSoundSetRolloff(FSLsound obj, float val);					// set rolloff factor (default: 1.0)
FSLAPI float FSLAPIENTRY fslSoundGetRolloff(FSLsound obj);								// get rolloff factor

FSLAPI int FSLAPIENTRY fslSoundGetBufferLength(FSLsound obj);							// get length in milisec
FSLAPI int FSLAPIENTRY fslSoundGetBufferPosition(FSLsound obj);							// get the current position in milisec
FSLAPI void FSLAPIENTRY fslSoundSetBufferPosition(FSLsound obj, int m);					// set the position in milisec (will be applied on the next fslSoundPlay call)
FSLAPI FSL_SOURCE_INFO FSLAPIENTRY fslSoundGetInfo(FSLsound obj);						// get info about the sound buffer

FSLAPI void FSLAPIENTRY fslSoundPauseAllSounds();										// pause all sounds that are currently playing
FSLAPI void FSLAPIENTRY fslSoundUnPauseAllSounds();										// unpause all sounds that have been paused
FSLAPI void FSLAPIENTRY fslSoundStopAllSounds();										// stop playing all sounds that are playing
FSLAPI void FSLAPIENTRY fslSoundSetPitchAllSounds(float pitch);							// apply the following pitch to all sounds
FSLAPI void FSLAPIENTRY fslSoundSetGainAllSounds(float gain);							// apply the following gain to all sounds

// Listener Functions
FSLAPI void FSLAPIENTRY fslSetListenerPositionVec(float* position);						// set listener position (float[3])
FSLAPI void FSLAPIENTRY fslSetListenerVelocityVec(float* velocity);						// set listener velocity (float[3])
FSLAPI void FSLAPIENTRY fslSetListenerOrientationVec(float* at, float* up);				// set listener orientation (view direction) (float[3],float[3])
FSLAPI void FSLAPIENTRY fslSetListenerPosition(float x, float y, float z);				// set listener position
FSLAPI void FSLAPIENTRY fslSetListenerVelocity(float x, float y, float z);				// set listener velocity
FSLAPI void FSLAPIENTRY fslSetListenerOrientation(float atx, float aty, float atz,
												  float upx, float upy, float upz);		// set listener orientation (view direction)
FSLAPI void FSLAPIENTRY fslSetListenerDistanceModel(FSLenum model);						// set distance model
FSLAPI void FSLAPIENTRY fslSetDopplerParameters(float factor, float velocity);			// set doppler factor and velocity (default: 1.0)
FSLAPI void FSLAPIENTRY fslSetSpeedOfSound(float val);									// set reference speed used in doppler calculation(default: 343.3)

FSLAPI void FSLAPIENTRY fslInitSourceManager(int max_sources);												// initialize source manager
FSLAPI void FSLAPIENTRY fslShutDownSourceManager();															// shutdown source manager
FSLAPI FSLsound FSLAPIENTRY fslSourceManagerGetSound(FSLbuffer b);											// get the handle to a free FSLsound slot (returns 0 if there arent any free slots)
FSLAPI int FSLAPIENTRY fslSourceManagerCountFreeSlots();													// return the number of free slots

#endif	// __FREESL_H