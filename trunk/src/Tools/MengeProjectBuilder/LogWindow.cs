using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MengeProjectBuilder
{
    public partial class LogWindow : Form
    {
        public LogWindow()
        {
            InitializeComponent();
            richTextBox1.Size = this.ClientSize;
        }

        public void logMessage( string _message, System.Drawing.Color _color )
        {
            Font fBold = new Font("Tahoma", 8, FontStyle.Regular);
            richTextBox1.SelectionFont = fBold;
            richTextBox1.SelectionColor = _color;
            richTextBox1.SelectedText = _message;
        }

        public void Clear()
        {
            richTextBox1.Text = "";
        }

        private void LogWindow_Resize(object sender, EventArgs e)
        {
            richTextBox1.Size = this.ClientSize;
        }

        private void LogWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
        }
    }
}