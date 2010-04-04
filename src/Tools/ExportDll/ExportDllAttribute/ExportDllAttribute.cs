using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace ExportDllAttribute
{
    [AttributeUsage(AttributeTargets.Method)] 
    public class ExportDllAttribute: Attribute
    {
        public ExportDllAttribute(string exportName)
            : this(exportName, System.Runtime.InteropServices.CallingConvention.StdCall)
        {
        }

        static Dictionary<System.Runtime.InteropServices.CallingConvention, string> dic = new Dictionary<CallingConvention,string>();

        static ExportDllAttribute()
        {
            dic[System.Runtime.InteropServices.CallingConvention.Cdecl] = typeof(CallConvStdcall).FullName;
            dic[System.Runtime.InteropServices.CallingConvention.FastCall] = typeof(CallConvFastcall).FullName;
            dic[System.Runtime.InteropServices.CallingConvention.StdCall] = typeof(CallConvStdcall).FullName;
            dic[System.Runtime.InteropServices.CallingConvention.ThisCall] = typeof(CallConvThiscall).FullName;
            dic[System.Runtime.InteropServices.CallingConvention.Winapi] = typeof(CallConvStdcall).FullName;
        }

        public ExportDllAttribute(string exportName, System.Runtime.InteropServices.CallingConvention CallingConvention)
        {
            m_ExportName = exportName;
            m_CallingConvention = dic[CallingConvention];
        }

        private string m_ExportName;
        public string ExportName
        {
            get { return m_ExportName; }
        }

        private string m_CallingConvention;
        public string CallingConvention
        {
            get { return m_CallingConvention; }
        }
	
    }
}
