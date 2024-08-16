#include "Win32CPUInfo.h"

#include "Config/StdString.h"

#include <intrin.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32CPUInfo::Win32CPUInfo()
        : m_nIds( 0 )
        , m_nExIds( 0 )
        , m_isIntel( false )
        , m_isAMD( false )
        , m_f_1_ECX( 0 )
        , m_f_1_EDX( 0 )
        , m_f_7_EBX( 0 )
        , m_f_7_ECX( 0 )
        , m_f_81_ECX( 0 )
        , m_f_81_EDX( 0 )
        , m_data{}
        , m_extdata{}
    {
        Array<int32_t, 4> cpui;

        // Calling __cpuid with 0x0 as the function_id argument
        // gets the number of the highest valid function ID.
        __cpuid( cpui.data(), 0 );
        m_nIds = cpui[0];

        for( int32_t i = 0; i <= m_nIds; ++i )
        {
            __cpuidex( cpui.data(), i, 0 );
            m_data.push_back( cpui );
        }

        // Capture vendor string
        Char vendor[0x20];
        StdString::memset( vendor, 0, sizeof( vendor ) );
        *reinterpret_cast<int32_t *>(vendor) = m_data[0][1];
        *reinterpret_cast<int32_t *>(vendor + 4) = m_data[0][3];
        *reinterpret_cast<int32_t *>(vendor + 8) = m_data[0][2];
        m_vendor = vendor;
        if( m_vendor == "GenuineIntel" )
        {
            m_isIntel = true;
        }
        else if( m_vendor == "AuthenticAMD" )
        {
            m_isAMD = true;
        }

        // load bitset with flags for function 0x00000001
        if( m_nIds >= 1 )
        {
            m_f_1_ECX = m_data[1][2];
            m_f_1_EDX = m_data[1][3];
        }

        // load bitset with flags for function 0x00000007
        if( m_nIds >= 7 )
        {
            m_f_7_EBX = m_data[7][1];
            m_f_7_ECX = m_data[7][2];
        }

        // Calling __cpuid with 0x80000000 as the function_id argument
        // gets the number of the highest valid extended ID.
        __cpuid( cpui.data(), 0x80000000 );
        m_nExIds = cpui[0];

        Char brand[0x40];
        StdString::memset( brand, 0, sizeof( brand ) );

        for( int32_t i = 0x80000000; i <= m_nExIds; ++i )
        {
            __cpuidex( cpui.data(), i, 0 );
            m_extdata.push_back( cpui );
        }

        // load bitset with flags for function 0x80000001
        if( m_nExIds >= 0x80000001 )
        {
            m_f_81_ECX = m_extdata[1][2];
            m_f_81_EDX = m_extdata[1][3];
        }

        // Interpret CPU brand string if reported
        if( m_nExIds >= 0x80000004 )
        {
            StdString::memcpy( brand, m_extdata[2].data(), sizeof( cpui ) );
            StdString::memcpy( brand + 16, m_extdata[3].data(), sizeof( cpui ) );
            StdString::memcpy( brand + 32, m_extdata[4].data(), sizeof( cpui ) );
            m_brand = brand;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Win32CPUInfo::~Win32CPUInfo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const String & Win32CPUInfo::Vendor() const
    {
        return m_vendor;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & Win32CPUInfo::Brand() const
    {
        return m_brand;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SSE3() const
    {
        return m_f_1_ECX[0];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::PCLMULQDQ() const
    {
        return m_f_1_ECX[1];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::MONITOR() const
    {
        return m_f_1_ECX[3];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SSSE3() const
    {
        return m_f_1_ECX[9];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::FMA() const
    {
        return m_f_1_ECX[12];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::CMPXCHG16B() const
    {
        return m_f_1_ECX[13];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SSE41() const
    {
        return m_f_1_ECX[19];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SSE42() const
    {
        return m_f_1_ECX[20];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::MOVBE() const
    {
        return m_f_1_ECX[22];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::POPCNT() const
    {
        return m_f_1_ECX[23];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::AES() const
    {
        return m_f_1_ECX[25];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::XSAVE() const
    {
        return m_f_1_ECX[26];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::OSXSAVE() const
    {
        return m_f_1_ECX[27];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::AVX() const
    {
        return m_f_1_ECX[28];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::F16C() const
    {
        return m_f_1_ECX[29];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::RDRAND() const
    {
        return m_f_1_ECX[30];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::MSR() const
    {
        return m_f_1_EDX[5];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::CX8() const
    {
        return m_f_1_EDX[8];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SEP() const
    {
        return m_f_1_EDX[11];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::CMOV() const
    {
        return m_f_1_EDX[15];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::CLFSH() const
    {
        return m_f_1_EDX[19];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::MMX() const
    {
        return m_f_1_EDX[23];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::FXSR() const
    {
        return m_f_1_EDX[24];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SSE() const
    {
        return m_f_1_EDX[25];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SSE2() const
    {
        return m_f_1_EDX[26];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::FSGSBASE() const
    {
        return m_f_7_EBX[0];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::BMI1() const
    {
        return m_f_7_EBX[3];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::HLE() const
    {
        return m_isIntel && m_f_7_EBX[4];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::AVX2() const
    {
        return m_f_7_EBX[5];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::BMI2() const
    {
        return m_f_7_EBX[8];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::ERMS() const
    {
        return m_f_7_EBX[9];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::INVPCID() const
    {
        return m_f_7_EBX[10];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::RTM() const
    {
        return m_isIntel && m_f_7_EBX[11];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::AVX512F() const
    {
        return m_f_7_EBX[16];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::RDSEED() const
    {
        return m_f_7_EBX[18];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::ADX() const
    {
        return m_f_7_EBX[19];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::AVX512PF() const
    {
        return m_f_7_EBX[26];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::AVX512ER() const
    {
        return m_f_7_EBX[27];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::AVX512CD() const
    {
        return m_f_7_EBX[28];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SHA() const
    {
        return m_f_7_EBX[29];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::PREFETCHWT1() const
    {
        return m_f_7_ECX[0];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::LAHF() const
    {
        return m_f_81_ECX[0];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::LZCNT() const
    {
        return m_isIntel && m_f_81_ECX[5];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::ABM() const
    {
        return m_isAMD && m_f_81_ECX[5];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SSE4a() const
    {
        return m_isAMD && m_f_81_ECX[6];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::XOP() const
    {
        return m_isAMD && m_f_81_ECX[11];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::TBM() const
    {
        return m_isAMD && m_f_81_ECX[21];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::SYSCALL() const
    {
        return m_isIntel && m_f_81_EDX[11];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::MMXEXT() const
    {
        return m_isAMD && m_f_81_EDX[22];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::RDTSCP() const
    {
        return m_isIntel && m_f_81_EDX[27];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::_3DNOWEXT() const
    {
        return m_isAMD && m_f_81_EDX[30];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CPUInfo::_3DNOW() const
    {
        return m_isAMD && m_f_81_EDX[31];
    }
    //////////////////////////////////////////////////////////////////////////
}