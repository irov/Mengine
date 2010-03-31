using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;

namespace XmlBinTool
{
    /// <summary>
    /// описание делегата на функции записи данных
    /// </summary>
    /// <param name="value"></param>
    delegate void writeFunc(string value);

    class Converter
    {
        Protocol protocol;                      //объект протокола
        string inputPath;                       //путь к файлу файлу
        string outputPath;                      //путь в исходящему файлу
        BinaryWriter binDoc;                    //объект бинарного файла
        Dictionary<string, writeFunc> funcDict; //делегат на функции записи данных

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_inputPath"></param>
        /// <param name="_outputPath"></param>
        /// <param name="_protocol"></param>
        public Converter(string _inputPath, string _outputPath, Protocol _protocol)
        {
            inputPath = _inputPath;
            outputPath = _outputPath;
            protocol = _protocol;

            funcDict = new Dictionary<string, writeFunc>();
            funcDict.Add("String", WriteString);
            funcDict.Add("bool", WriteBool);
            funcDict.Add("vec2f", WriteNFloat);
            funcDict.Add("vec4f", WriteNFloat);
            funcDict.Add("mat3f", WriteNFloat);
            funcDict.Add("Resolution", WriteNFloat);
            funcDict.Add("ColourValue", WriteNFloat);
            funcDict.Add("float", WriteNFloat);
            funcDict.Add("int", WriteInt);
            funcDict.Add("size_t", WriteUint);
            funcDict.Add("Viewport", WriteNFloat);
        }
        /// <summary>
        /// 
        /// </summary>
        public void Convert()
        {
            System.Console.WriteLine("Converting " + inputPath + "...");

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(inputPath);

            binDoc = new BinaryWriter(File.Open(outputPath, FileMode.Create));

            
            //XmlNodeList nodeList = xmlDoc.FirstChild.ChildNodes;
            //XmlNodeList nodeList = xmlDoc.FirstChild.SelectNodes("*");
            XmlNodeList nodeList = xmlDoc.SelectNodes("*");

            //binDoc.Write(nodeList.Count);

            foreach (XmlNode node in nodeList)
            {
                WriteNode(node);
            }

            binDoc.Close();

            System.Console.WriteLine("Convert complete");
        }
        /// <summary>
        /// записать ноду
        /// </summary>
        /// <param name="node"></param>
        private void WriteNode(XmlNode node)
        {
            int nodeId;
            try
            {
                nodeId = protocol.NodeIdDict[node.Name];
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message);
                System.Console.WriteLine(node.Name);
                return;
            }

            binDoc.Write(nodeId);
            binDoc.Write(node.Attributes.Count);
                        
            foreach (XmlAttribute attr in node.Attributes)
            {
                WriteAttribute(node, attr);
            }

            binDoc.Write(node.ChildNodes.Count);
            foreach (XmlNode subnode in node.ChildNodes)
            {
                WriteNode(subnode);
            }
        }

        /// <summary>
        /// записать атрибут
        /// </summary>
        /// <param name="node"></param>
        /// <param name="attr"></param>
        private void WriteAttribute(XmlNode node, XmlAttribute attr)
        {
            string attrFullKey = node.Name + "." + attr.Name;

            int attrId;
            try
            {
                attrId = protocol.AttrIdDict[attrFullKey];
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message);
                System.Console.WriteLine(attrFullKey);
                return;
            }

            //int nodeId = protocol.NodeIdDict[node.Name];
            binDoc.Write(attrId);
            string valueType = protocol.TypeDict[attrFullKey];
            funcDict[valueType](attr.Value);
        }

        /// <summary>
        /// запись строкового значения
        /// </summary>
        /// <param name="value"></param>
        private void WriteString(string value)
        {
            binDoc.Write(value);
            
        }
        /// <summary>
        /// запись булевского значения
        /// </summary>
        /// <param name="value"></param>
        private void WriteBool(string value)
        {
            bool bValue;
            if (value == "0") bValue = false;
            else bValue = true;
            binDoc.Write(bValue);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="value"></param>
        private void WriteNFloat(string value)
        {
            string[] strValues = value.Split(';');
            foreach (string strValue in strValues)
            {
                float fValue = System.Convert.ToSingle(strValue);
                binDoc.Write(fValue);
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="value"></param>
        private void WriteInt(string value)
        {
            Int32 iValue = System.Convert.ToInt32(value);
            binDoc.Write(iValue);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="value"></param>
        private void WriteUint(string value)
        {
            UInt32 iValue = System.Convert.ToUInt32(value);
            binDoc.Write(iValue);
        }
    }
}
