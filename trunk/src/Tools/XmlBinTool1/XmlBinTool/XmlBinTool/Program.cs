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

            Dictionary<string, string> typeDict = new Dictionary<string,string>();

            XmlDocument protocolXml = new XmlDocument();
            protocolXml.Load("protocol.xml");
            XmlNodeList nodeList = protocolXml.GetElementsByTagName("Node");
            foreach(XmlNode node in nodeList)
            {
                string nodeType = node.Attributes.GetNamedItem("NodeType").Value;
                string attrName = node.Attributes.GetNamedItem("AttrName").Value;
                string attrType = node.Attributes.GetNamedItem("AttrType").Value;

                string key = nodeType + "." + attrName;
                typeDict.Add(key, attrType);
            }

            int a = 0;
        }
    }
}
