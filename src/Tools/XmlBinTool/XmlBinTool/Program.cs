using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace XmlBinTool
{
    class Program
    {
        static void Main(string[] args)
        {
            Protocol protocol = new Protocol("protocol.xml");

            Converter converter = new Converter(args[0], args[1], protocol);
            converter.Convert();

            int a = 0;
        }
    }
}
