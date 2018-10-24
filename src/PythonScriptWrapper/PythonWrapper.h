#pragma once

namespace Mengine
{
    class PythonWrapper
    {
    public:
        static bool constsWrap();
        static bool helperWrap();
        static bool mathWrap();
        static bool nodeWrap();
        static bool entityWrap();
        static bool engineWrap();
        static bool soundWrap();
    };
}
