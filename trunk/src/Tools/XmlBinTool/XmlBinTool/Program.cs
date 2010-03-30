using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace XmlBinTool
{
    class Program
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="args"></param>
        static void Main(string[] args)
        {
            try
            {
                if (args.Length == 0)
                    printHelp();
                else if (args[0] == "-h")
                    createHeader(args[1], args[2]);
                else if (args[0] == "-f")
                    convertFile(args[1], args[2], args[3]);
                else
                    printHelp();
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message);
            }

            System.Console.ReadKey();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_headerPath"></param>
        static void createHeader(string _protocolPath, string _headerPath)
        {
            Protocol protocol = new Protocol(_protocolPath);
            protocol.createHeader(_headerPath);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_inputPath"></param>
        /// <param name="_outputPath"></param>
        static void convertFile(string _protocolPath, string _inputPath, string _outputPath)
        {
            Protocol protocol = new Protocol(_protocolPath);

            Converter converter = new Converter(_inputPath, _outputPath, protocol);
            converter.Convert();
        }
        /// <summary>
        /// 
        /// </summary>
        static void printHelp()
        {
            string message = "Usage:\n";
            message += "-h *protocol path* *header path* - generate header\n";
            message += "-f *protocol path* *input name* *output name* - convert xml to bin\n";

            System.Console.WriteLine(message);
        }
    }
}
