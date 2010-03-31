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

            FileConverter converter = new FileConverter(_inputPath, _outputPath, protocol);
            converter.Convert();
        }
        /// <summary>
        /// 
        /// </summary>
        static void printHelp()
        {
            System.Console.WriteLine("Usage:");
            System.Console.WriteLine("-h *protocol path* *header path* - generate header");
            System.Console.WriteLine("-f *protocol path* *input name* *output name* - convert xml to bin");
            System.Console.WriteLine("-p *protocol path* *app xml path* - convert whole menge project");
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_protocolPath"></param>
        /// <param name="_appXmlPath"></param>
        static void convertProject(string _protocolPath, string _appXmlPath)
        {
        }
    }
}
