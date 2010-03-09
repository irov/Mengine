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
            try
            {
                Protocol protocol = new Protocol("protocol.xml");

                Converter converter = new Converter(args[0], args[1], protocol);
                converter.Convert();
            }
            catch (Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.Message);
            }

            int a = 0;
        }
    }
}
