using System;
using System.Collections.Generic;
using System.Text;


namespace Xml2Bin
{
    public class Main
    {
        static FileConverter converter;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_protocolPath"></param>
        [ExportDllAttribute.ExportDll("initialize",
        System.Runtime.InteropServices.CallingConvention.Cdecl)]
        public static void initializeCstyle(IntPtr _protocolPath)
        {
            Console.WriteLine("initialize");
            string strPath = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(_protocolPath);
            initialize(strPath);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_protocolPath"></param>
        public static void initialize(String _protocolPath)
        {
            Console.WriteLine(_protocolPath);
            try
            {
                converter = new FileConverter(_protocolPath);
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception: " + e.Message);
            }
            Console.WriteLine("initialize end");
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_inputPath"></param>
        /// <param name="_outputPath"></param>
        [ExportDllAttribute.ExportDll("convert",
        System.Runtime.InteropServices.CallingConvention.Cdecl)]
        public static void convertCstyle(IntPtr _inputPath, IntPtr _outputPath)
        {
            string strInput = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(_inputPath);
            string strOutput = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(_outputPath);
            convert(strInput, strOutput);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_inputPath"></param>
        /// <param name="_outPath"></param>
        public static void convert(String _inputPath, String _outPath)
        {
            converter.Convert(_inputPath, _outPath);
        }
    }
}
