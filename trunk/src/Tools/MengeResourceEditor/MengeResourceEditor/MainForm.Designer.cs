namespace MengeResourceEditor
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
            this.m_pgResourceProperty = new System.Windows.Forms.PropertyGrid();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_cbPackName = new System.Windows.Forms.ComboBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.m_cbResourceName = new System.Windows.Forms.ComboBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.m_twResourceTree = new System.Windows.Forms.TreeView();
            this.m_gbGlobalGroup = new System.Windows.Forms.GroupBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.menuStrip1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.m_gbGlobalGroup.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_pgResourceProperty
            // 
            this.m_pgResourceProperty.Location = new System.Drawing.Point(356, 19);
            this.m_pgResourceProperty.Name = "m_pgResourceProperty";
            this.m_pgResourceProperty.Size = new System.Drawing.Size(483, 508);
            this.m_pgResourceProperty.TabIndex = 4;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(869, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.toolStripSeparator1,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.openToolStripMenuItem.Text = "Open...";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.onFileOpenClick);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.onBtnSaveClick);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(149, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            // 
            // m_cbPackName
            // 
            this.m_cbPackName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbPackName.FormattingEnabled = true;
            this.m_cbPackName.Location = new System.Drawing.Point(6, 19);
            this.m_cbPackName.Name = "m_cbPackName";
            this.m_cbPackName.Size = new System.Drawing.Size(331, 21);
            this.m_cbPackName.TabIndex = 1;
            this.m_cbPackName.SelectedIndexChanged += new System.EventHandler(this.onPackNameChange);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.m_cbPackName);
            this.groupBox1.Location = new System.Drawing.Point(7, 19);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(343, 53);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Pack Name";
            // 
            // m_cbResourceName
            // 
            this.m_cbResourceName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbResourceName.FormattingEnabled = true;
            this.m_cbResourceName.Location = new System.Drawing.Point(6, 19);
            this.m_cbResourceName.Name = "m_cbResourceName";
            this.m_cbResourceName.Size = new System.Drawing.Size(331, 21);
            this.m_cbResourceName.TabIndex = 1;
            this.m_cbResourceName.SelectedIndexChanged += new System.EventHandler(this.onResourceNameChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.m_cbResourceName);
            this.groupBox2.Location = new System.Drawing.Point(7, 78);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(343, 53);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Resource Name";
            // 
            // m_twResourceTree
            // 
            this.m_twResourceTree.Location = new System.Drawing.Point(7, 159);
            this.m_twResourceTree.Name = "m_twResourceTree";
            this.m_twResourceTree.Size = new System.Drawing.Size(343, 368);
            this.m_twResourceTree.TabIndex = 5;
            this.m_twResourceTree.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.onResourceTreeSelect);
            // 
            // m_gbGlobalGroup
            // 
            this.m_gbGlobalGroup.Controls.Add(this.m_twResourceTree);
            this.m_gbGlobalGroup.Controls.Add(this.m_pgResourceProperty);
            this.m_gbGlobalGroup.Controls.Add(this.groupBox2);
            this.m_gbGlobalGroup.Controls.Add(this.groupBox1);
            this.m_gbGlobalGroup.Controls.Add(this.tabControl1);
            this.m_gbGlobalGroup.Enabled = false;
            this.m_gbGlobalGroup.Location = new System.Drawing.Point(12, 27);
            this.m_gbGlobalGroup.Name = "m_gbGlobalGroup";
            this.m_gbGlobalGroup.Size = new System.Drawing.Size(845, 534);
            this.m_gbGlobalGroup.TabIndex = 6;
            this.m_gbGlobalGroup.TabStop = false;
            this.m_gbGlobalGroup.Text = "Resource Editor";
            // 
            // tabControl1
            // 
            this.tabControl1.Location = new System.Drawing.Point(6, 137);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(338, 100);
            this.tabControl1.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
            this.tabControl1.TabIndex = 6;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(869, 566);
            this.Controls.Add(this.m_gbGlobalGroup);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.Text = "Menge Resource Editor";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.m_gbGlobalGroup.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ComboBox m_cbPackName;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox m_cbResourceName;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TreeView m_twResourceTree;
        private System.Windows.Forms.PropertyGrid m_pgResourceProperty;
        private System.Windows.Forms.GroupBox m_gbGlobalGroup;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.TabControl tabControl1;
    }
}

