using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing;


namespace Editor
{
    static class DrawDebug
    {
        public static SolidBrush handleBrush = new SolidBrush(Color.Aqua);
        public static Pen handlePen = new Pen(Color.Black, 1);
        public static int handleSize = 10;
        public static Pen ImageNullSprite = new Pen(Color.Red, 1);
        public static Pen ImageSelectedSprite = new Pen(Color.Green, 1);
        public static Pen PolygonSelected = new Pen(Color.Yellow, 1);
        public static int ImageNullSize = 128;
        public static Matrix IdentMatrix = new Matrix();
    }
}
