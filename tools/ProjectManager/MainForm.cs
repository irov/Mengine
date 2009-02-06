using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace ProjectManager
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            ProjectManager.Instance.InitProjectManagerDir();
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String[] templateNames = ProjectManager.Instance.GetAvaibleTemplates();

            using (NewProjectForm newProject = new NewProjectForm(templateNames))
            {
                if (newProject.ShowDialog() == DialogResult.OK)
                {
                    ProjectManager.Instance.CreateNewProject(newProject.newProjectName, newProject.newProjectLocation, newProject.templateName);
                    GameProperties gameProperties = ProjectManager.Instance.CurrentProject.GetGamePropertiesInfo();
                    GamePropertyGrid.SelectedObject = gameProperties;
                }
            }
        }

        private void runGameToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (SettingsForm settingsProject = new SettingsForm())
            {
                String args = "";

                if (settingsProject.ShowDialog() == DialogResult.OK)
                {
                    if(settingsProject.sound)
                    {
                        args += "-sound ";
                    }

                    if (settingsProject.debuginfo)
                    {
                        args += "-debuginfo ";
                    }

                    if (settingsProject.particles)
                    {
                        args += "-particles";
                    }

                    ProjectManager.Instance.RunCurrentProject(args);
                }
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult result = openFileDialog.ShowDialog();

            treeView1.Nodes.Clear();

            if (result == DialogResult.OK)
            {
                ProjectManager.Instance.OpenProject(openFileDialog.FileName);
                GameProperties gameProperties = ProjectManager.Instance.CurrentProject.GetGamePropertiesInfo();
                GamePropertyGrid.SelectedObject = gameProperties;

                ProjectManager.Instance.CurrentProject.GetResourcesXMLPath();

                foreach (String resource in ProjectManager.Instance.GetResourceNames())
                {
                    TreeNode treeNode = treeView1.Nodes.Add(resource);
                }
            }
        }

        private void closeToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            ProjectManager.Instance.CloseProject();
            GamePropertyGrid.SelectedObject = null;
            treeView1.Nodes.Clear();
        }

        private void newFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
    
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ProjectManager.Instance.SaveProject();
        }

        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (treeView1.SelectedNode!=null)
            {
                String NodeName = treeView1.SelectedNode.Text;
                ResourceImageDefault res = ProjectManager.Instance.resourceManager.GetResource(NodeName);

                propertyGrid1.SelectedObject = res;
            }
        }

        private void compileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (CompileProjectForm compileProjectForm = new CompileProjectForm())
            {
                if (compileProjectForm.ShowDialog() == DialogResult.OK)
                {
                }
            }
        }
    }
}