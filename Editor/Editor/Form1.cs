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

        private void Form1_Load(object sender, EventArgs e)
        {
            ManagedFramework framework = new ManagedFramework();
            framework.createWindow(this.Handle);
        }

        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {

        }
    }
}