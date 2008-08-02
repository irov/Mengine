using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;

namespace Editor
{
    class MotionModifier : Modifier
    {
        public MotionModifier(ref Node _node)
        {
            SelectedNode = _node;
        }

        void Modifier.DownNode(int x, int y)
        {
            SelectedNode.select();
            X = x;
            Y = y;
        }

        void Modifier.ProcessNode(int x, int y)
        {
            int offsetX = x-X;
            int offsetY = y-Y;

            if(SelectedNode!=null)
            SelectedNode.translate(offsetX, offsetY);

            X = x;
            Y = y;
        }

        void Modifier.UpNode(int x, int y)
        {
            if (SelectedNode != null)
            SelectedNode.deselect();
            SelectedNode = null;
        }

        private Node SelectedNode = null;
        private int X = 0;
        private int Y = 0;
    }
}
