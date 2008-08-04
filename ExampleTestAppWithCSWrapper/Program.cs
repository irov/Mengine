using System;
using System.Runtime.InteropServices;

namespace ExampleTestAppWithCSWrapper
{
	class Program
	{
        [DllImport("Framework.dll")]
        static public extern void MengeInit();

        [DllImport("Framework.dll")]
        static public extern void MengeFree();

		static void Main(string[] args)
		{
            MengeInit();
		}
	}
}
