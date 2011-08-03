using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ProjectManager
{
    public partial class CompileProjectForm : Form
    {
        public CompileProjectForm()
        {
            InitializeComponent();
        }

        private void OkButton_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        private void CompileProjectForm_Load(object sender, EventArgs e)
        {

        }

        private void FolderBrowserButton_Click(object sender, EventArgs e)
        {
            DialogResult result = FolderBrowserDialog.ShowDialog();

            switch (result)
            {
                case DialogResult.OK:
                    {
                        ProjectLocation.Text = FolderBrowserDialog.SelectedPath;
                        break;
                    }
                case DialogResult.Cancel:
                    {
                        return;
                    }
                default:
                    {
                        return;
                    }
            }
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}