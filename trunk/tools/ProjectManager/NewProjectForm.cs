using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ProjectManager
{
    public partial class NewProjectForm : Form
    {
        public NewProjectForm(String[] templates)
        {
            InitializeComponent();
            templateNames = templates;
        }

        public string newProjectName
        {
            get
            { return _newProjectName; }
        }

        public string newProjectLocation
        {
            get
            { return _newProjectLocation; }
        }

        public string templateName
        {
            get
            { return _templateName; }
        }

        private string _newProjectName;
        private string _newProjectLocation;
        private string _templateName;

        private String[] templateNames = null;

        private void OkButton_Click(object sender, EventArgs e)
        {
            _newProjectName = NewProjectName.Text;

            if (newProjectName == "")
            {
                MessageBox.Show("enter new project name!");
                return;
            }

            _newProjectLocation = ProjectLocation.Text;

            if (!System.IO.Directory.Exists(newProjectLocation))
            {
                MessageBox.Show("new project location does not exist!");
                return;
            }

            _templateName = TemplateComboBox.Items[TemplateComboBox.SelectedIndex].ToString();

            this.DialogResult = DialogResult.OK;
        }

        private void NewProject_Load(object sender, EventArgs e)
        {
            if(templateNames.Length == 0)
            {
                MessageBox.Show("there is no templates!");
                this.DialogResult = DialogResult.Cancel;
            }

            foreach (String templateName in templateNames)
            {
                TemplateComboBox.Items.Add(templateName);
            }

            TemplateComboBox.SelectedIndex = 0;           
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
    }
}