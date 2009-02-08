namespace ProjectManager
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.mainMenuStrip = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.projectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.runGameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.compileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.groupBoxProjectSettings = new System.Windows.Forms.GroupBox();
            this.gamePropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.groupBoxResourceSettings = new System.Windows.Forms.GroupBox();
            this.resourceTreeView = new System.Windows.Forms.TreeView();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.resourcePropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.mainMenuStrip.SuspendLayout();
            this.groupBoxProjectSettings.SuspendLayout();
            this.groupBoxResourceSettings.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainMenuStrip
            // 
            this.mainMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.projectToolStripMenuItem});
            this.mainMenuStrip.Location = new System.Drawing.Point(0, 0);
            this.mainMenuStrip.Name = "mainMenuStrip";
            this.mainMenuStrip.Size = new System.Drawing.Size(786, 24);
            this.mainMenuStrip.TabIndex = 0;
            this.mainMenuStrip.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.closeToolStripMenuItem1,
            this.toolStripSeparator1,
            this.settingsToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
            this.fileToolStripMenuItem.Text = "File";
            this.fileToolStripMenuItem.Click += new System.EventHandler(this.fileToolStripMenuItem_Click);
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(135, 22);
            this.newToolStripMenuItem.Text = "New";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(135, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(135, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem1
            // 
            this.closeToolStripMenuItem1.Name = "closeToolStripMenuItem1";
            this.closeToolStripMenuItem1.Size = new System.Drawing.Size(135, 22);
            this.closeToolStripMenuItem1.Text = "Close";
            this.closeToolStripMenuItem1.Click += new System.EventHandler(this.closeToolStripMenuItem1_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(132, 6);
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(135, 22);
            this.settingsToolStripMenuItem.Text = "Settings";
            // 
            // projectToolStripMenuItem
            // 
            this.projectToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.runGameToolStripMenuItem,
            this.compileToolStripMenuItem});
            this.projectToolStripMenuItem.Name = "projectToolStripMenuItem";
            this.projectToolStripMenuItem.Size = new System.Drawing.Size(60, 20);
            this.projectToolStripMenuItem.Text = "Project";
            // 
            // runGameToolStripMenuItem
            // 
            this.runGameToolStripMenuItem.Name = "runGameToolStripMenuItem";
            this.runGameToolStripMenuItem.Size = new System.Drawing.Size(148, 22);
            this.runGameToolStripMenuItem.Text = "Run Game";
            this.runGameToolStripMenuItem.Click += new System.EventHandler(this.runGameToolStripMenuItem_Click);
            // 
            // compileToolStripMenuItem
            // 
            this.compileToolStripMenuItem.Name = "compileToolStripMenuItem";
            this.compileToolStripMenuItem.Size = new System.Drawing.Size(148, 22);
            this.compileToolStripMenuItem.Text = "Compile";
            this.compileToolStripMenuItem.Click += new System.EventHandler(this.compileToolStripMenuItem_Click);
            // 
            // groupBoxProjectSettings
            // 
            this.groupBoxProjectSettings.Controls.Add(this.gamePropertyGrid);
            this.groupBoxProjectSettings.Dock = System.Windows.Forms.DockStyle.Left;
            this.groupBoxProjectSettings.Location = new System.Drawing.Point(0, 24);
            this.groupBoxProjectSettings.Name = "groupBoxProjectSettings";
            this.groupBoxProjectSettings.Size = new System.Drawing.Size(219, 588);
            this.groupBoxProjectSettings.TabIndex = 1;
            this.groupBoxProjectSettings.TabStop = false;
            this.groupBoxProjectSettings.Text = "Project Structure";
            // 
            // gamePropertyGrid
            // 
            this.gamePropertyGrid.Dock = System.Windows.Forms.DockStyle.Left;
            this.gamePropertyGrid.Location = new System.Drawing.Point(3, 16);
            this.gamePropertyGrid.Name = "gamePropertyGrid";
            this.gamePropertyGrid.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.gamePropertyGrid.Size = new System.Drawing.Size(212, 569);
            this.gamePropertyGrid.TabIndex = 1;
            // 
            // openFileDialog
            // 
            this.openFileDialog.FileName = "openFileDialog1";
            this.openFileDialog.Filter = "app |*.xml";
            // 
            // groupBoxResourceSettings
            // 
            this.groupBoxResourceSettings.Controls.Add(this.resourceTreeView);
            this.groupBoxResourceSettings.Dock = System.Windows.Forms.DockStyle.Left;
            this.groupBoxResourceSettings.Location = new System.Drawing.Point(219, 24);
            this.groupBoxResourceSettings.Name = "groupBoxResourceSettings";
            this.groupBoxResourceSettings.Size = new System.Drawing.Size(200, 588);
            this.groupBoxResourceSettings.TabIndex = 2;
            this.groupBoxResourceSettings.TabStop = false;
            this.groupBoxResourceSettings.Text = "Game Structure";
            // 
            // resourceTreeView
            // 
            this.resourceTreeView.Dock = System.Windows.Forms.DockStyle.Left;
            this.resourceTreeView.Location = new System.Drawing.Point(3, 16);
            this.resourceTreeView.Name = "resourceTreeView";
            this.resourceTreeView.Size = new System.Drawing.Size(191, 569);
            this.resourceTreeView.TabIndex = 0;
            this.resourceTreeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView1_NodeMouseClick);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.treeView1);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Right;
            this.groupBox2.Location = new System.Drawing.Point(586, 24);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(200, 588);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Resources";
            // 
            // treeView1
            // 
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.Location = new System.Drawing.Point(3, 16);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(194, 569);
            this.treeView1.TabIndex = 0;
            // 
            // resourcePropertyGrid
            // 
            this.resourcePropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.resourcePropertyGrid.Location = new System.Drawing.Point(3, 16);
            this.resourcePropertyGrid.Name = "resourcePropertyGrid";
            this.resourcePropertyGrid.Size = new System.Drawing.Size(361, 569);
            this.resourcePropertyGrid.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.resourcePropertyGrid);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(419, 24);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(367, 588);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Resource Settings";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(786, 612);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.groupBoxResourceSettings);
            this.Controls.Add(this.groupBoxProjectSettings);
            this.Controls.Add(this.mainMenuStrip);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.mainMenuStrip;
            this.Name = "MainForm";
            this.Text = "ProjectManager";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.mainMenuStrip.ResumeLayout(false);
            this.mainMenuStrip.PerformLayout();
            this.groupBoxProjectSettings.ResumeLayout(false);
            this.groupBoxResourceSettings.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip mainMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem projectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem runGameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem compileToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBoxProjectSettings;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.PropertyGrid gamePropertyGrid;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBoxResourceSettings;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.TreeView resourceTreeView;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.PropertyGrid resourcePropertyGrid;
        private System.Windows.Forms.GroupBox groupBox1;
    }
}

