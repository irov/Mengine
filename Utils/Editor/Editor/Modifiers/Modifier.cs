using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;

namespace Editor
{
    interface Modifier
    {
        void DownNode(int x, int y);
        void ProcessNode(int x, int y);
        void UpNode(int x, int y);
    }
}
