//#	pragma once
//
//#ifdef __APPLE__
//#   include <OpenAL/al.h>
//#   include <OpenAL/alc.h>
//#else
//#	include "AL/al.h"
//#	include "AL/alc.h"
//#endif
//
//namespace Menge
//{
//	class SoundSulkCallbackInterface;
//}
//
//class SulkSystem
//{
//public:
//	SulkSystem();
//	~SulkSystem();
//
//public:
//	bool initialize();
//
//public:
//	void setEnoughBlow( float _enoughBlow );
//	float getEnoughBlow() const;
//
//public:
//	void setCallback( Menge::SoundSulkCallbackInterface * _callback );
//
//	bool activate( bool _activate );
//	float getBlow() const;
//
//public:
//	void update();
//
//protected:
//	bool m_active;
//	bool m_initialize;
//
//	float m_enoughBlow;
//	float m_dispersion;
//	float m_average;
//
//	Menge::SoundSulkCallbackInterface * m_sulkcallback;
//
//	ALCcontext * m_pContext;
//	ALCdevice * m_pDevice;
//	const ALCchar * m_szDefaultCaptureDevice;
//	ALCdevice * m_pCaptureDevice;
//
//	ALCint m_iSamplesAvailable;
//	ALCsizei m_bufferSize;
//	ALCsizei m_blockAlign;
//	ALchar * m_buffer;
//};
