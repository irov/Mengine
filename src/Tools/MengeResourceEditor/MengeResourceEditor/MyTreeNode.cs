using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace MengeResourceEditor
{
    
    class MyTreeNode: TreeNode
    {
        Nodes.Node xmlNode;
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_xmlNode"></param>
        public MyTreeNode(Nodes.Node _xmlNode)
        {
            Initialize(_xmlNode);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_xmlNode"></param>
        public MyTreeNode(Nodes.ResourceImageDefault _xmlNode)
        {
            Initialize(_xmlNode);

            MengeResourceEditor.Nodes.ResourceImageDefault image = xmlNode as MengeResourceEditor.Nodes.ResourceImageDefault;

            foreach(Nodes.File file in image.FileList)
            {
                MyTreeNode subNode = new MyTreeNode(file);
                Nodes.Add(subNode);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="_xmlNode"></param>
        public void Initialize(Nodes.Node _xmlNode)
        {
            xmlNode = _xmlNode;
            Text = ToString();

            Toggle();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return xmlNode.ToString();
        }
        /// <summary>
        /// 
        /// </summary>
        public Nodes.Node XmlNode
        {
            get{
                return xmlNode;
            }
        }
    }
}
