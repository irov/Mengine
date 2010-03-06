using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;

namespace XmlBinTool
{
    delegate void writeFunc(string value);

    class Converter
    {
        Protocol protocol;
        string inputPath;
        string outputPath;
        StreamWriter binDoc;
        Dictionary<string, writeFunc> funcDict;
        

        public Converter(string _inputPath, string _outputPath, Protocol _protocol)
        {
            inputPath = _inputPath;
            outputPath = _outputPath;
            protocol = _protocol;

            funcDict = new Dictionary<string, writeFunc>();
            funcDict.Add("String", WriteString);
            funcDict.Add("Bool", WriteBool);
        }

        public void Convert()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(inputPath);

            binDoc = new StreamWriter(File.Open(outputPath, FileMode.Create));

            XmlNodeList nodeList = xmlDoc.FirstChild.ChildNodes;

            binDoc.Write(nodeList.Count);

            foreach (XmlNode node in nodeList)
            {
                WriteNode(node);
            }

            binDoc.Close();
        }

        private void WriteNode(XmlNode node)
        {
            int nodeId = protocol.NodeIdDict[node.Name];
            binDoc.Write(nodeId);
            binDoc.Write(node.Attributes.Count);
            binDoc.Write(node.ChildNodes.Count);

            foreach (XmlAttribute attr in node.Attributes)
            {
                WriteAttribute(node, attr);
            }

            if (node.ChildNodes.Count == 0) return;
            foreach (XmlNode subnode in node.ChildNodes)
            {
                WriteNode(subnode);
            }
        }

        private void WriteAttribute(XmlNode node, XmlAttribute attr)
        {
            int attrId = protocol.AttrIdDict[attr.Name];
            int nodeId = protocol.NodeIdDict[node.Name];
            binDoc.Write(attrId);
            string valueType = protocol.TypeDict[node.Name + "." + attr.Name];
            funcDict[valueType](attr.Value);
        }

        private void WriteString(string value)
        {
            binDoc.Write(value);
            
        }
        private void WriteBool(string value)
        {
            bool bValue;
            if (value == "0") bValue = false;
            else bValue = true;
            binDoc.Write(bValue);
            
        }
    }
}
