using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;

namespace Editor
{
    class ResourceTreeViewer
    {
        public ResourceTreeViewer(ref ResourceManager _resourceManager, ref TreeView treeView)
        {
            foreach(ResourceImageDefault resource in _resourceManager.GetValues())
            {
                TreeNode treeNode = treeView.Nodes.Add(resource.GetName());

                for (int i = 0; i < resource.GetImageCount(); i++)
                {
                    String path = resource.GetImagePath(i);
                    treeNode.Nodes.Add(path);
                }
            }
        }
    }

}
