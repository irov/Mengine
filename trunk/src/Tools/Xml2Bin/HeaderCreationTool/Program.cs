using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace HeaderCreationTool
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                switch (args.Length)
                {
                    case 0: 
                        PrintHelp(); 
                        break;
                    case 2: 
                        Start(args[0], args[1]); 
                        break;
                    default: 
                        System.Console.WriteLine("Incorrect params count");
                        break;
                }
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message);
            }

            Console.ReadKey();
        }

        static void PrintHelp()
        {
            System.Console.WriteLine("Usage:");
            System.Console.WriteLine("*protocol path* *header path* - create header");
        }

        static void Start(string _protocolPath, string _headerPath)
        {
            HeaderCreator creator = new HeaderCreator(_protocolPath);
            creator.createHeader(_headerPath);
        }
    }
}
