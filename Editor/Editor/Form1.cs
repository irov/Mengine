using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Editor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        ManagedFramework framework = new ManagedFramework();

        public void Initialise()
        {
            this.SuspendLayout();

            this.ClientSize = new System.Drawing.Size(1016, 734);
            this.Text = "gameWindow";
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.Opaque, true);
            this.ResumeLayout(false);
        }

        public void Initialise(Size wndSize, string wndText)
        {
            this.SuspendLayout();

            this.ClientSize = wndSize;
            this.Text = wndText;
           this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.Opaque, true);
            this.ResumeLayout(false);
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            System.IO.Directory.SetCurrentDirectory("D:\\NewProject\\Bin");
            
            Initialise(new Size(1024, 768), "Example");
            framework.createWindow(this.Handle);
        }

        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {         
        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            //framework.step();
          //  this.Invalidate();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            framework.step(1000);
        }
    }
}