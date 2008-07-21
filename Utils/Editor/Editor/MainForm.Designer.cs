namespace Editor
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
            this.sceneGroupBox = new System.Windows.Forms.GroupBox();
            this.button7 = new System.Windows.Forms.Button();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.scrollableCheckBox = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button6 = new System.Windows.Forms.Button();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.nodeNameTextBox = new System.Windows.Forms.TextBox();
            this.button5 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.sceneTreeView = new System.Windows.Forms.TreeView();
            this.spriteIndexTextBox = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.layerNameTextBox = new System.Windows.Forms.TextBox();
            this.spritePosYTextBox = new System.Windows.Forms.TextBox();
            this.layerMainCheckBox = new System.Windows.Forms.CheckBox();
            this.spritePosXTextBox = new System.Windows.Forms.TextBox();
            this.layerSizeYTextBox = new System.Windows.Forms.TextBox();
            this.resourceTreeView = new System.Windows.Forms.TreeView();
            this.nodesTreeView = new System.Windows.Forms.TreeView();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.layerSizeXTextBox = new System.Windows.Forms.TextBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.BottomToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.TopToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.RightToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.LeftToolStripPanel = new System.Windows.Forms.ToolStripPanel();
            this.ContentPanel = new System.Windows.Forms.ToolStripContentPanel();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.loadResourceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveSceneToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadSceneToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sceneGroupBox.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // sceneGroupBox
            // 
            this.sceneGroupBox.Controls.Add(this.button7);
            this.sceneGroupBox.Controls.Add(this.checkBox1);
            this.sceneGroupBox.Controls.Add(this.scrollableCheckBox);
            this.sceneGroupBox.Controls.Add(this.label2);
            this.sceneGroupBox.Controls.Add(this.button6);
            this.sceneGroupBox.Controls.Add(this.textBox2);
            this.sceneGroupBox.Controls.Add(this.textBox1);
            this.sceneGroupBox.Controls.Add(this.nodeNameTextBox);
            this.sceneGroupBox.Controls.Add(this.button5);
            this.sceneGroupBox.Controls.Add(this.label1);
            this.sceneGroupBox.Controls.Add(this.sceneTreeView);
            this.sceneGroupBox.Controls.Add(this.spriteIndexTextBox);
            this.sceneGroupBox.Controls.Add(this.button1);
            this.sceneGroupBox.Controls.Add(this.button4);
            this.sceneGroupBox.Controls.Add(this.layerNameTextBox);
            this.sceneGroupBox.Controls.Add(this.spritePosYTextBox);
            this.sceneGroupBox.Controls.Add(this.layerMainCheckBox);
            this.sceneGroupBox.Controls.Add(this.spritePosXTextBox);
            this.sceneGroupBox.Controls.Add(this.layerSizeYTextBox);
            this.sceneGroupBox.Controls.Add(this.resourceTreeView);
            this.sceneGroupBox.Controls.Add(this.nodesTreeView);
            this.sceneGroupBox.Controls.Add(this.button3);
            this.sceneGroupBox.Controls.Add(this.button2);
            this.sceneGroupBox.Controls.Add(this.layerSizeXTextBox);
            this.sceneGroupBox.Dock = System.Windows.Forms.DockStyle.Left;
            this.sceneGroupBox.Location = new System.Drawing.Point(0, 24);
            this.sceneGroupBox.Name = "sceneGroupBox";
            this.sceneGroupBox.Size = new System.Drawing.Size(227, 625);
            this.sceneGroupBox.TabIndex = 1;
            this.sceneGroupBox.TabStop = false;
            this.sceneGroupBox.Text = "Scene properties";
            // 
            // button7
            // 
            this.button7.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button7.Location = new System.Drawing.Point(138, 321);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(75, 24);
            this.button7.TabIndex = 23;
            this.button7.Text = "add tilepoly";
            this.button7.UseVisualStyleBackColor = true;
            this.button7.Visible = false;
            this.button7.Click += new System.EventHandler(this.button7_Click);
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(6, 488);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(56, 17);
            this.checkBox1.TabIndex = 22;
            this.checkBox1.Text = "center";
            this.checkBox1.UseVisualStyleBackColor = true;
            this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // scrollableCheckBox
            // 
            this.scrollableCheckBox.AutoSize = true;
            this.scrollableCheckBox.Checked = true;
            this.scrollableCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.scrollableCheckBox.Location = new System.Drawing.Point(138, 39);
            this.scrollableCheckBox.Name = "scrollableCheckBox";
            this.scrollableCheckBox.Size = new System.Drawing.Size(80, 17);
            this.scrollableCheckBox.TabIndex = 21;
            this.scrollableCheckBox.Text = "is scrollable";
            this.scrollableCheckBox.UseVisualStyleBackColor = true;
            this.scrollableCheckBox.CheckedChanged += new System.EventHandler(this.scrollableCheckBox_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 557);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(63, 13);
            this.label2.TabIndex = 20;
            this.label2.Text = "canvas size";
            // 
            // button6
            // 
            this.button6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button6.Location = new System.Drawing.Point(138, 166);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(75, 23);
            this.button6.TabIndex = 19;
            this.button6.Text = "remove";
            this.button6.UseVisualStyleBackColor = true;
            this.button6.Click += new System.EventHandler(this.button6_Click);
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(6, 599);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(65, 20);
            this.textBox2.TabIndex = 18;
            this.textBox2.Text = "4096";
            this.textBox2.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox2_KeyDown);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(6, 573);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(65, 20);
            this.textBox1.TabIndex = 17;
            this.textBox1.Text = "4096";
            this.textBox1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox1_KeyDown);
            // 
            // nodeNameTextBox
            // 
            this.nodeNameTextBox.Location = new System.Drawing.Point(6, 462);
            this.nodeNameTextBox.Name = "nodeNameTextBox";
            this.nodeNameTextBox.Size = new System.Drawing.Size(65, 20);
            this.nodeNameTextBox.TabIndex = 16;
            this.nodeNameTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.nodeNameTextBox_KeyDown);
            // 
            // button5
            // 
            this.button5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button5.Location = new System.Drawing.Point(138, 350);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(75, 24);
            this.button5.TabIndex = 15;
            this.button5.Text = "remove";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 439);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(32, 13);
            this.label1.TabIndex = 14;
            this.label1.Text = "index";
            // 
            // sceneTreeView
            // 
            this.sceneTreeView.HideSelection = false;
            this.sceneTreeView.Location = new System.Drawing.Point(6, 19);
            this.sceneTreeView.Name = "sceneTreeView";
            this.sceneTreeView.Size = new System.Drawing.Size(126, 170);
            this.sceneTreeView.TabIndex = 8;
            this.sceneTreeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.sceneTreeView_NodeMouseClick);
            this.sceneTreeView.KeyUp += new System.Windows.Forms.KeyEventHandler(this.sceneTreeView_KeyUp);
            this.sceneTreeView.KeyDown += new System.Windows.Forms.KeyEventHandler(this.sceneTreeView_KeyDown);
            // 
            // spriteIndexTextBox
            // 
            this.spriteIndexTextBox.Location = new System.Drawing.Point(40, 436);
            this.spriteIndexTextBox.Name = "spriteIndexTextBox";
            this.spriteIndexTextBox.Size = new System.Drawing.Size(31, 20);
            this.spriteIndexTextBox.TabIndex = 13;
            this.spriteIndexTextBox.Text = "0";
            this.spriteIndexTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.spriteIndexTextBox_KeyDown);
            // 
            // button1
            // 
            this.button1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button1.Location = new System.Drawing.Point(138, 140);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 7;
            this.button1.Text = "add layer";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button4
            // 
            this.button4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button4.Location = new System.Drawing.Point(6, 406);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(67, 23);
            this.button4.TabIndex = 12;
            this.button4.Text = "set resource";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // layerNameTextBox
            // 
            this.layerNameTextBox.Location = new System.Drawing.Point(138, 62);
            this.layerNameTextBox.Name = "layerNameTextBox";
            this.layerNameTextBox.Size = new System.Drawing.Size(75, 20);
            this.layerNameTextBox.TabIndex = 6;
            this.layerNameTextBox.Text = "MainLayer";
            this.layerNameTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.layerNameTextBox_KeyDown);
            // 
            // spritePosYTextBox
            // 
            this.spritePosYTextBox.Location = new System.Drawing.Point(42, 380);
            this.spritePosYTextBox.Name = "spritePosYTextBox";
            this.spritePosYTextBox.Size = new System.Drawing.Size(31, 20);
            this.spritePosYTextBox.TabIndex = 11;
            this.spritePosYTextBox.Text = "0";
            this.spritePosYTextBox.Leave += new System.EventHandler(this.spritePosYTextBox_Leave);
            this.spritePosYTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.spritePosYTextBox_KeyDown);
            // 
            // layerMainCheckBox
            // 
            this.layerMainCheckBox.AutoSize = true;
            this.layerMainCheckBox.Checked = true;
            this.layerMainCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.layerMainCheckBox.Location = new System.Drawing.Point(138, 19);
            this.layerMainCheckBox.Name = "layerMainCheckBox";
            this.layerMainCheckBox.Size = new System.Drawing.Size(58, 17);
            this.layerMainCheckBox.TabIndex = 4;
            this.layerMainCheckBox.Text = "is main";
            this.layerMainCheckBox.UseVisualStyleBackColor = true;
            // 
            // spritePosXTextBox
            // 
            this.spritePosXTextBox.Location = new System.Drawing.Point(6, 380);
            this.spritePosXTextBox.Name = "spritePosXTextBox";
            this.spritePosXTextBox.Size = new System.Drawing.Size(31, 20);
            this.spritePosXTextBox.TabIndex = 10;
            this.spritePosXTextBox.Text = "0";
            this.spritePosXTextBox.Leave += new System.EventHandler(this.spritePosXTextBox_Leave);
            this.spritePosXTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.spritePosXTextBox_KeyDown);
            // 
            // layerSizeYTextBox
            // 
            this.layerSizeYTextBox.Location = new System.Drawing.Point(138, 114);
            this.layerSizeYTextBox.Name = "layerSizeYTextBox";
            this.layerSizeYTextBox.Size = new System.Drawing.Size(75, 20);
            this.layerSizeYTextBox.TabIndex = 3;
            this.layerSizeYTextBox.Text = "0";
            this.layerSizeYTextBox.Leave += new System.EventHandler(this.layerSizeYTextBox_Leave);
            this.layerSizeYTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.layerSizeYTextBox_KeyDown);
            // 
            // resourceTreeView
            // 
            this.resourceTreeView.HideSelection = false;
            this.resourceTreeView.Location = new System.Drawing.Point(75, 380);
            this.resourceTreeView.Name = "resourceTreeView";
            this.resourceTreeView.Size = new System.Drawing.Size(146, 239);
            this.resourceTreeView.TabIndex = 6;
            // 
            // nodesTreeView
            // 
            this.nodesTreeView.HideSelection = false;
            this.nodesTreeView.Location = new System.Drawing.Point(6, 195);
            this.nodesTreeView.Name = "nodesTreeView";
            this.nodesTreeView.Size = new System.Drawing.Size(126, 179);
            this.nodesTreeView.TabIndex = 9;
            this.nodesTreeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.nodesTreeView_NodeMouseClick);
            this.nodesTreeView.KeyUp += new System.Windows.Forms.KeyEventHandler(this.nodesTreeView_KeyUp);
            this.nodesTreeView.KeyDown += new System.Windows.Forms.KeyEventHandler(this.nodesTreeView_KeyDown);
            // 
            // button3
            // 
            this.button3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button3.Location = new System.Drawing.Point(138, 262);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 25);
            this.button3.TabIndex = 5;
            this.button3.Text = "add sprite";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button2
            // 
            this.button2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.button2.Location = new System.Drawing.Point(138, 292);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 24);
            this.button2.TabIndex = 4;
            this.button2.Text = "add hotspot";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // layerSizeXTextBox
            // 
            this.layerSizeXTextBox.Location = new System.Drawing.Point(138, 88);
            this.layerSizeXTextBox.Name = "layerSizeXTextBox";
            this.layerSizeXTextBox.Size = new System.Drawing.Size(75, 20);
            this.layerSizeXTextBox.TabIndex = 2;
            this.layerSizeXTextBox.Text = "0";
            this.layerSizeXTextBox.Leave += new System.EventHandler(this.layerSizeXTextBox_Leave);
            this.layerSizeXTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.layerSizeXTextBox_KeyDown);
            // 
            // panel1
            // 
            this.panel1.AutoScroll = true;
            this.panel1.Controls.Add(this.pictureBox1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(227, 24);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(604, 625);
            this.panel1.TabIndex = 3;
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pictureBox1.InitialImage = null;
            this.pictureBox1.Location = new System.Drawing.Point(0, 0);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(4096, 4096);
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
            this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
            this.pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);
            this.pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseUp);
            // 
            // BottomToolStripPanel
            // 
            this.BottomToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.BottomToolStripPanel.Name = "BottomToolStripPanel";
            this.BottomToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.BottomToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.BottomToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // TopToolStripPanel
            // 
            this.TopToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.TopToolStripPanel.Name = "TopToolStripPanel";
            this.TopToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.TopToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.TopToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // RightToolStripPanel
            // 
            this.RightToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.RightToolStripPanel.Name = "RightToolStripPanel";
            this.RightToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.RightToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.RightToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // LeftToolStripPanel
            // 
            this.LeftToolStripPanel.Location = new System.Drawing.Point(0, 0);
            this.LeftToolStripPanel.Name = "LeftToolStripPanel";
            this.LeftToolStripPanel.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.LeftToolStripPanel.RowMargin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.LeftToolStripPanel.Size = new System.Drawing.Size(0, 0);
            // 
            // ContentPanel
            // 
            this.ContentPanel.Size = new System.Drawing.Size(150, 150);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadResourceToolStripMenuItem,
            this.saveSceneToolStripMenuItem,
            this.loadSceneToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(831, 24);
            this.menuStrip1.TabIndex = 4;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // loadResourceToolStripMenuItem
            // 
            this.loadResourceToolStripMenuItem.Name = "loadResourceToolStripMenuItem";
            this.loadResourceToolStripMenuItem.Size = new System.Drawing.Size(104, 20);
            this.loadResourceToolStripMenuItem.Text = "Load Resource";
            this.loadResourceToolStripMenuItem.Click += new System.EventHandler(this.loadResourceToolStripMenuItem_Click);
            // 
            // saveSceneToolStripMenuItem
            // 
            this.saveSceneToolStripMenuItem.Name = "saveSceneToolStripMenuItem";
            this.saveSceneToolStripMenuItem.Size = new System.Drawing.Size(87, 20);
            this.saveSceneToolStripMenuItem.Text = "Save Scene";
            this.saveSceneToolStripMenuItem.Click += new System.EventHandler(this.saveSceneToolStripMenuItem_Click);
            // 
            // loadSceneToolStripMenuItem
            // 
            this.loadSceneToolStripMenuItem.Name = "loadSceneToolStripMenuItem";
            this.loadSceneToolStripMenuItem.Size = new System.Drawing.Size(86, 20);
            this.loadSceneToolStripMenuItem.Text = "Load Scene";
            this.loadSceneToolStripMenuItem.Click += new System.EventHandler(this.loadSceneToolStripMenuItem_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(831, 649);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.sceneGroupBox);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.Text = "Editor";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.sceneGroupBox.ResumeLayout(false);
            this.sceneGroupBox.PerformLayout();
            this.panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox sceneGroupBox;
        private System.Windows.Forms.TextBox layerSizeYTextBox;
        private System.Windows.Forms.TextBox layerSizeXTextBox;
        private System.Windows.Forms.CheckBox layerMainCheckBox;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.TreeView sceneTreeView;
        private System.Windows.Forms.ToolStripPanel BottomToolStripPanel;
        private System.Windows.Forms.ToolStripPanel TopToolStripPanel;
        private System.Windows.Forms.ToolStripPanel RightToolStripPanel;
        private System.Windows.Forms.ToolStripPanel LeftToolStripPanel;
        private System.Windows.Forms.ToolStripContentPanel ContentPanel;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem loadResourceToolStripMenuItem;
        private System.Windows.Forms.TreeView resourceTreeView;
        private System.Windows.Forms.TreeView nodesTreeView;
        private System.Windows.Forms.TextBox spritePosYTextBox;
        private System.Windows.Forms.TextBox spritePosXTextBox;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox spriteIndexTextBox;
        private System.Windows.Forms.ToolStripMenuItem saveSceneToolStripMenuItem;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.TextBox nodeNameTextBox;
        private System.Windows.Forms.TextBox layerNameTextBox;
        private System.Windows.Forms.ToolStripMenuItem loadSceneToolStripMenuItem;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox scrollableCheckBox;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Button button7;
    }
}

