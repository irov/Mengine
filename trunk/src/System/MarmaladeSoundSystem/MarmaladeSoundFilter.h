#	pragma once

#	include <math.h>

#	define MARMALADE_SOUND_NUM_COEFFICIENTS 129

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	enum ESoundWindowConstants
	{
		ESWC_BLACKMAN,
		ESWC_HANNING,
		ESWC_HAMMING
	};
	//////////////////////////////////////////////////////////////////////////
	const double PI = 3.14159265358979323846;
	//////////////////////////////////////////////////////////////////////////
	static void s_genSinc( double sinc[], size_t N, double fc )
	{		
		const double M = N - 1;

		// Generate sinc delayed by (N-1)/2
		for( size_t i = 0; i != N; ++i ) 
		{
			double n = (double)i - M / 2.0;
			sinc[i] = sin( 2.0 * PI * fc * n ) / (PI * n);
		}        
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wBlackman( double w[], size_t N )
	{
		const double M = N - 1;

		for( size_t i = 0; i != N; ++i)
		{
			double ii = (double)i;
			double a = ( 0.5 * cos( 2.0 * PI * ii / M ) );
			double b = ( 0.08 * cos( 4.0 * PI * ii / M ) );
			w[i] = 0.42 - a + b;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static inline void s_wHanning( double w[], size_t N )
	{
		const double M = N - 1;

		for( size_t i = 0; i != N; ++i)
		{
			w[i] = 0.5 * (1.0 - cos( 2.0 * PI * (double)i/M ));
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static inline void s_wHamming( double w[], size_t N )
	{
		const double M = N-1;

		for( size_t i = 0; i != N; ++i )
		{
			w[i] = 0.54 - (0.46 * cos( 2.0 * PI * (double)i/M ));
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_wsfirLP( double h[], ESoundWindowConstants mode, double fc )
	{
		double w[MARMALADE_SOUND_NUM_COEFFICIENTS];		// window function
		double sinc[MARMALADE_SOUND_NUM_COEFFICIENTS];	// sinc function

		// 1. Generate Sinc function
		s_genSinc( sinc, MARMALADE_SOUND_NUM_COEFFICIENTS, fc );

		// 2. Generate Window function
		switch(mode) 
		{
		case ESWC_BLACKMAN:	// W_BLACKMAN
			s_wBlackman(w, MARMALADE_SOUND_NUM_COEFFICIENTS);
			break;
		case ESWC_HANNING:		// W_HANNING
			s_wBlackman(w, MARMALADE_SOUND_NUM_COEFFICIENTS);
			break;
		case ESWC_HAMMING:		// W_HAMMING
			s_wBlackman(w, MARMALADE_SOUND_NUM_COEFFICIENTS);
			break;
		default:
			break;
		}

		// 3. Make lowpass filter
		for( size_t i = 0; i != MARMALADE_SOUND_NUM_COEFFICIENTS; ++i )
		{
			h[i] = sinc[i] * w[i];
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_setupFilterCoefficients( double _filterCoefficients[], int _inputFrequence, int _outputFrequence )
	{
		int minimumFrequence = _inputFrequence < _outputFrequence ? _inputFrequence : _outputFrequence;
		double fc = ((double)minimumFrequence / 2.0) / (double)_outputFrequence; // half the input sample rate (eg nyquist limit of input)

		// Generate filter coefficients
		s_wsfirLP( _filterCoefficients, ESWC_BLACKMAN, fc );
	}
	//////////////////////////////////////////////////////////////////////////
	static int32 s_GCD( int32 a, int32 b )
	{
		while( true )
		{
			a = a % b;
			if (a == 0)
				return b;
			b = b % a;

			if (b == 0)
				return a;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static inline int16 s_clipToInt16( int32 sval )
	{
		const int32 minval = INT16_MIN;
		const int32 maxval = INT16_MAX;
		const int32 allbits = UINT16_MAX;	

		// quick overflow test, the addition moves valid range to 0-allbits
		if( ( sval - minval ) & ~allbits )
		{
			if( sval > maxval )
			{
				sval = maxval;
			}
			else
			{
				if( sval < minval )
				{
					sval = minval;
				}
			}
		}

		return (int16)sval;
	}
}