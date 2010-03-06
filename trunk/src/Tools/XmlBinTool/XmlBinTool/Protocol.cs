using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace XmlBinTool
{
    class Protocol
    {
        Dictionary<string, string> typeDict = new Dictionary<string, string>();
        Dictionary<string, int> nodeIdDict = new Dictionary<string, int>();
        Dictionary<string, int> attrIdDict = new Dictionary<string,int>();

        public void updateFromXml(string _fileName)
        {
            XmlDocument protocolXml = new XmlDocument();
            protocolXml.Load("protocol.xml");
            XmlNodeList nodeList = protocolXml.GetElementsByTagName("Node");

            int nodeID = 0;
            int attrID = 0;
            foreach (XmlNode node in nodeList)
            {
                string nodeType = node.Attributes.GetNamedItem("NodeType").Value;
                string attrName = node.Attributes.GetNamedItem("AttrName").Value;
                string attrType = node.Attributes.GetNamedItem("AttrType").Value;

                string key = nodeType + "." + attrName;

                typeDict.Add(key, attrType);

                if (!nodeIdDict.ContainsKey(nodeType))
                {
                    nodeIdDict.Add(nodeType, nodeID);
                    nodeID++;
                }
                if (!attrIdDict.ContainsKey(attrName))
                {
                    attrIdDict.Add(attrName, attrID);
                    attrID++;
                }
            }
        }

        public Protocol(string _fileName)
        {
            updateFromXml(_fileName);
        }

        public Dictionary<string, string> TypeDict
        {
            get
            {
                return typeDict;
            }
        }

        public Dictionary<string, int> NodeIdDict
        {
            get
            {
                return nodeIdDict;
            }
        }

        public Dictionary<string, int> AttrIdDict
        {
            get
            {
                return attrIdDict;
            }
        }
    }
}
