#pragma once

#include "Kernel/Bitset.h"
#include "Kernel/Array.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32CPUInfo
    {
    public:
        Win32CPUInfo();
        ~Win32CPUInfo();

    public:
        const String & Vendor() const;
        const String & Brand() const;
        bool SSE3() const;
        bool PCLMULQDQ() const;
        bool MONITOR() const;
        bool SSSE3() const;
        bool FMA() const;
        bool CMPXCHG16B() const;
        bool SSE41() const;
        bool SSE42() const;
        bool MOVBE() const;
        bool POPCNT() const;
        bool AES() const;
        bool XSAVE() const;
        bool OSXSAVE() const;
        bool AVX() const;
        bool F16C() const;
        bool RDRAND() const;
        bool MSR() const;
        bool CX8() const;
        bool SEP() const;
        bool CMOV() const;
        bool CLFSH() const;
        bool MMX() const;
        bool FXSR() const;
        bool SSE() const;
        bool SSE2() const;
        bool FSGSBASE() const;
        bool BMI1() const;
        bool HLE() const;
        bool AVX2() const;
        bool BMI2() const;
        bool ERMS() const;
        bool INVPCID() const;
        bool RTM() const;
        bool AVX512F() const;
        bool RDSEED() const;
        bool ADX() const;
        bool AVX512PF() const;
        bool AVX512ER() const;
        bool AVX512CD() const;
        bool SHA() const;
        bool PREFETCHWT1() const;
        bool LAHF() const;
        bool LZCNT() const;
        bool ABM() const;
        bool SSE4a() const;
        bool XOP() const;
        bool TBM() const;
        bool SYSCALL() const;
        bool MMXEXT() const;
        bool RDTSCP() const;
        bool _3DNOWEXT() const;
        bool _3DNOW() const;

    protected:
        int m_nIds;
        int m_nExIds;
        String m_vendor;
        String m_brand;
        bool m_isIntel;
        bool m_isAMD;
        Bitset<32> m_f_1_ECX;
        Bitset<32> m_f_1_EDX;
        Bitset<32> m_f_7_EBX;
        Bitset<32> m_f_7_ECX;
        Bitset<32> m_f_81_ECX;
        Bitset<32> m_f_81_EDX;
        Vector<Array<int, 4>> m_data;
        Vector<Array<int, 4>> m_extdata;
    };
}