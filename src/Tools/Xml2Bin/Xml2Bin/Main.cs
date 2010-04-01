using System;
using System.Collections.Generic;
using System.Text;


namespace Xml2Bin
{
    class Main
    {
        static FileConverter converter;

        [ExportDllAttribute.ExportDll("initialize",
        System.Runtime.InteropServices.CallingConvention.Cdecl)]
        public static void initialize(IntPtr _protocolPath)
        {
            Console.WriteLine("initialize");
            string strPath = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(_protocolPath);
            Console.WriteLine(strPath);
            try
            {
                converter = new FileConverter(strPath);
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception: " + e.Message);
            }
            Console.WriteLine("initialize end");
        }

        [ExportDllAttribute.ExportDll("convert",
        System.Runtime.InteropServices.CallingConvention.Cdecl)]
        public static void convert(IntPtr _inputPath, IntPtr _outputPath)
        {
            string strInput = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(_inputPath);
            string strOutput = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(_outputPath);
            converter.Convert(strInput, strOutput);
        }
    }
}
