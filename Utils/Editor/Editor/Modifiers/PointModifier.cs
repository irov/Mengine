using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;

namespace Editor
{
    class PointModifier : Modifier
    {
        public PointModifier(ref HotSpot _node, int index)
        {
            SelectedNode = _node;
            SelectedHotSpot = _node;
            dragPoint = index;
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

            if (SelectedNode != null)
            {
                SelectedHotSpot.MovePoint(dragPoint, new Point(x, y));
            }

            X = x;
            Y = y;
        }

        void Modifier.UpNode(int x, int y)
        {
            if (SelectedNode != null)
            {
                SelectedNode.deselect();

                if (dragPoint != -1)
                {
                    SelectedHotSpot.MovePoint(dragPoint, new Point(x, y));
                }
            }

            SelectedNode = null;

            dragPoint = -1;
        }

        private Node SelectedNode = null;
        private HotSpot SelectedHotSpot = null;

        private int X = 0;
        private int Y = 0;
        private int dragPoint = -1;
    }
}
