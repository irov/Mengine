using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using System.Xml;
using System.IO;

namespace Editor
{
    class ResourceManagerLoader
    {
        public ResourceManagerLoader(ref ResourceManager _resourceManager)
        {
            resourceManager = _resourceManager;
        }

        public bool execute()
        {
            OpenFileDialog fileDialog = new OpenFileDialog();

            fileDialog.Title = "Resource Files";
            fileDialog.InitialDirectory = @"";
            fileDialog.Filter = "Resource Files (*.resource)|*.resource";
            fileDialog.FilterIndex = 0;
            fileDialog.RestoreDirectory = true;

            if (fileDialog.ShowDialog() == DialogResult.OK)
            {
                XmlDocument xDoc = new XmlDocument();

                xDoc.Load(fileDialog.FileName);

                String offsetPath = Path.GetDirectoryName(fileDialog.FileName);
                //SPECIFIC TO MENGE. 
                offsetPath = Path.GetDirectoryName(offsetPath);

                XmlNodeList list = xDoc.GetElementsByTagName("Resource");

                foreach (XmlNode node in list)
                {
                    XmlAttributeCollection attr = node.Attributes;

                    String nodeName = attr.Item(0).Value;
                    String nodeType = attr.Item(1).Value;

                    if (nodeType == "ResourceImageDefault")
                    {
                        ResourceImageDefault resource = new ResourceImageDefault(nodeName, nodeType);

                        foreach (XmlNode childNode in node.ChildNodes)
                        {
                            XmlNodeType nType = childNode.NodeType;

                            if (nType == XmlNodeType.Comment)
                            {
                                continue;
                            }

                            String path = childNode.Attributes.Item(0).Value;

                            String filePath = offsetPath + "\\" + path;

                            if (File.Exists(filePath))
                            {
                                resource.AddFile(filePath);
                            }
                        }

                        resourceManager.AddResource(resource);
                    }
                }
                return true;
            }

            return false;       
        }

        public ResourceManager resourceManager;
    }
}
