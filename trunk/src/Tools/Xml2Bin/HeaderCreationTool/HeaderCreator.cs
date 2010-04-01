using System;
using System.Collections.Generic;
using System.Text;
using ProtocolReader;
using System.IO;

namespace HeaderCreationTool
{
    class HeaderCreator
    {
        Protocol protocol;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_protocolPath"></param>
        public HeaderCreator(string _protocolPath)
        {
            protocol = new Protocol(_protocolPath);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_headerPath"></param>
        public void createHeader(string _headerPath)
        {
            Console.WriteLine("Creating header " + _headerPath);

            StreamWriter writer = new StreamWriter(_headerPath);

            writer.WriteLine("namespace Menge");
            writer.WriteLine("{");
            writer.WriteLine("    namespace Protocol");
            writer.WriteLine("    {");

            foreach (string key in protocol.NodeIdDict.Keys)
            {
                writer.WriteLine("        struct " + key);
                writer.WriteLine("        {");
                writer.WriteLine("            static const int id = " + protocol.NodeIdDict[key].ToString() + ";");
                writer.WriteLine("        }");
            }

            foreach (string key in protocol.HeaderStructName.Keys)
            {
                writer.WriteLine("        struct " + protocol.HeaderStructName[key]);
                writer.WriteLine("        {");
                writer.WriteLine("            static const int id = " + protocol.AttrIdDict[key].ToString() + ";");

                string value = "";
                if (protocol.TypeDict.TryGetValue(key, out value))
                    writer.WriteLine("            typedef " + value + " Type;");

                writer.WriteLine("        }");
            }

            writer.WriteLine("    }");
            writer.WriteLine("}");

            writer.Close();

            Console.WriteLine("Creating complete");
        }
    }
}
