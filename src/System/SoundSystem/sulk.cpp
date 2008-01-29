#include "sulk.h"
#include <math.h>

//-------------------------------------------------------------
// Method: Sulk
// Description:
//-------------------------------------------------------------
Sulk::Sulk( float eBlow ):
enoughBlow_		( eBlow ),
dispersion_		( 0.0f ),
average_		( 0.0f )
{
	// Initialize Framework
	ALFWInit();

	if (!ALFWInitOpenAL())
	{
		// TODO : throw error here!
	}

	// Check for Capture Extension support
	pContext_ = alcGetCurrentContext();
	pDevice_ = alcGetContextsDevice(pContext_);
	if (alcIsExtensionPresent(pDevice_, "ALC_EXT_CAPTURE") == AL_FALSE)
	{
		// TODO : throw error here!
	}

	// Get the name of the 'default' capture device
	szDefaultCaptureDevice_ = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
	ALFWprintf("\nDefault Capture Device is '%s'\n\n", szDefaultCaptureDevice_);

	// Open the default Capture device to record a 22050Hz 16bit Mono Stream using an internal buffer
	// of BUFFERSIZE Samples (== BUFFERSIZE * 2 bytes)
	pCaptureDevice_ = alcCaptureOpenDevice(szDefaultCaptureDevice_, 22050, AL_FORMAT_MONO16, BUFFERSIZE);
	if (!pCaptureDevice_)
	{
		// TODO : throw error here!
	}

	// Start audio capture
	alcCaptureStart(pCaptureDevice_);
}

//-------------------------------------------------------------
// Method: Sulk
// Description:
//-------------------------------------------------------------
Sulk::~Sulk()
{
	// Stop capture
	alcCaptureStop(pCaptureDevice_);

	// Close the Capture Device
	alcCaptureCloseDevice(pCaptureDevice_);
}

//-------------------------------------------------------------
// Method: update
// Description:
//-------------------------------------------------------------
void Sulk::update( float dTime )
{
	// Find out how many samples have been captured
	alcGetIntegerv(pCaptureDevice_, ALC_CAPTURE_SAMPLES, 1, &iSamplesAvailable_);

	// When we have enough data to fill our BUFFERSIZE byte buffer, grab the samples
	if (iSamplesAvailable_ > (BUFFERSIZE / NBLOCKALIGN))
	{
		// Consume Samples
		alcCaptureSamples(pCaptureDevice_, Buffer_, BUFFERSIZE / NBLOCKALIGN);

		// find average
		float average = 0.f;
		for (int x = 0; x < BUFFERSIZE; ++x)
		{
			average += (int)Buffer_[x];
		}
		average /= BUFFERSIZE;

		average_ = average;

		// find dispersion
		float disp = 0.f;
		for (int x = 0; x < BUFFERSIZE - 1; ++x)
		{
			disp += powf( (int)Buffer_[x] - average, 2 );
			//disp += powf( (int)Buffer_[x] - (int)Buffer_[x+1], 2 );
		}
		disp /= BUFFERSIZE;

		dispersion_ = sqrtf(disp);
	}
}

//-------------------------------------------------------------
// Method: getBlow
// Description:
//-------------------------------------------------------------
float Sulk::getBlow()
{
	float blow = 0.f;

	if ( dispersion_ >= enoughBlow_ )
	{
		blow = ( dispersion_ - enoughBlow_ ) * 0.1f;
	}

	return dispersion_;
}

//-------------------------------------------------------------
// Method: getPoints
// Description:
//-------------------------------------------------------------
array<System::Drawing::Point, 1>^ Sulk::getPoints()
{
	array<System::Drawing::Point, 1>^ drawPoints = gcnew array<System::Drawing::Point, 1>(BUFFERSIZE);

	for (int x = 0; x < BUFFERSIZE; ++x)
	{
		int y = 200 + (int)Buffer_[x];

		drawPoints[x].X = (int)(x * 0.2f);
		drawPoints[x].Y = y;
	}

	return drawPoints;
}