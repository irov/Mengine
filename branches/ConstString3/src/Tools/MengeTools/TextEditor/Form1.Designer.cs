namespace MengeTextEditor
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItemEdit = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.m_exitMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_findNextMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.m_btnPlus = new System.Windows.Forms.Button();
            this.m_btnMinus = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.m_fontCombo = new System.Windows.Forms.ComboBox();
            this.m_maxWidthNumeric = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.m_maxHeightNumeric = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.m_charOffsetNumeric = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.m_lineOffsetNumeric = new System.Windows.Forms.NumericUpDown();
            this.m_saveFileDialog = new System.Windows.Forms.SaveFileDialog();
            this.m_actualSizeLbl = new System.Windows.Forms.Label();
            this.m_actualSizeEdit = new System.Windows.Forms.TextBox();
            this.m_editOriginal = new System.Windows.Forms.RichTextBox();
            this.m_treeView = new System.Windows.Forms.TreeView();
            this.m_importDialog = new System.Windows.Forms.OpenFileDialog();
            this.m_exportDialog = new System.Windows.Forms.SaveFileDialog();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_maxWidthNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_maxHeightNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_charOffsetNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_lineOffsetNumeric)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(810, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "MainMenu";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItemEdit,
            this.saveToolStripMenuItem,
            this.toolStripMenuItem1,
            this.toolStripMenuItem2,
            this.toolStripMenuItem3,
            this.toolStripSeparator1,
            this.m_exitMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // toolStripMenuItemEdit
            // 
            this.toolStripMenuItemEdit.Name = "toolStripMenuItemEdit";
            this.toolStripMenuItemEdit.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.toolStripMenuItemEdit.Size = new System.Drawing.Size(176, 22);
            this.toolStripMenuItemEdit.Text = "&Open";
            this.toolStripMenuItemEdit.Click += new System.EventHandler(this.toolStripMenuItemEdit_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(176, 22);
            this.saveToolStripMenuItem.Text = "&Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(176, 22);
            this.toolStripMenuItem1.Text = "Save as...";
            this.toolStripMenuItem1.Click += new System.EventHandler(this.toolStripMenuItem1_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(176, 22);
            this.toolStripMenuItem2.Text = "Export to CSV...";
            this.toolStripMenuItem2.Click += new System.EventHandler(this.onExportToCSVMenuItem);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(176, 22);
            this.toolStripMenuItem3.Text = "Import from CSV...";
            this.toolStripMenuItem3.Click += new System.EventHandler(this.onImportFromCSVMenuItem);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(173, 6);
            // 
            // m_exitMenuItem
            // 
            this.m_exitMenuItem.Name = "m_exitMenuItem";
            this.m_exitMenuItem.Size = new System.Drawing.Size(176, 22);
            this.m_exitMenuItem.Text = "E&xit";
            this.m_exitMenuItem.Click += new System.EventHandler(this.m_exitMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_findNextMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.editToolStripMenuItem.Text = "&Edit";
            // 
            // m_findNextMenuItem
            // 
            this.m_findNextMenuItem.Name = "m_findNextMenuItem";
            this.m_findNextMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.F)));
            this.m_findNextMenuItem.Size = new System.Drawing.Size(181, 22);
            this.m_findNextMenuItem.Text = "Find Next...";
            this.m_findNextMenuItem.Click += new System.EventHandler(this.m_findNextMenuItem_Click);
            // 
            // m_openFileDialog
            // 
            this.m_openFileDialog.DefaultExt = "*.xml";
            this.m_openFileDialog.Filter = "Resource XML files (*.xml)|*.xml|All files (*.*)|*.*";
            this.m_openFileDialog.Title = "Select Resource File of Localizing Pack";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(-3, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(73, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Text Handles:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(382, 35);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Original Text:";
            // 
            // m_btnPlus
            // 
            this.m_btnPlus.Location = new System.Drawing.Point(350, 61);
            this.m_btnPlus.Name = "m_btnPlus";
            this.m_btnPlus.Size = new System.Drawing.Size(25, 23);
            this.m_btnPlus.TabIndex = 8;
            this.m_btnPlus.Text = "+";
            this.m_btnPlus.UseVisualStyleBackColor = true;
            this.m_btnPlus.Click += new System.EventHandler(this.m_btnPlus_Click);
            // 
            // m_btnMinus
            // 
            this.m_btnMinus.Location = new System.Drawing.Point(350, 90);
            this.m_btnMinus.Name = "m_btnMinus";
            this.m_btnMinus.Size = new System.Drawing.Size(25, 23);
            this.m_btnMinus.TabIndex = 9;
            this.m_btnMinus.Text = "-";
            this.m_btnMinus.UseVisualStyleBackColor = true;
            this.m_btnMinus.Click += new System.EventHandler(this.m_btnMinus_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(382, 287);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 13);
            this.label4.TabIndex = 10;
            this.label4.Text = "Font:";
            // 
            // m_fontCombo
            // 
            this.m_fontCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_fontCombo.FormattingEnabled = true;
            this.m_fontCombo.Location = new System.Drawing.Point(462, 284);
            this.m_fontCombo.Name = "m_fontCombo";
            this.m_fontCombo.Size = new System.Drawing.Size(167, 21);
            this.m_fontCombo.TabIndex = 11;
            this.m_fontCombo.Leave += new System.EventHandler(this.editTextNode_Leave);
            this.m_fontCombo.TextUpdate += new System.EventHandler(this.editTextNode_Leave);
            // 
            // m_maxWidthNumeric
            // 
            this.m_maxWidthNumeric.Location = new System.Drawing.Point(462, 311);
            this.m_maxWidthNumeric.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.m_maxWidthNumeric.Name = "m_maxWidthNumeric";
            this.m_maxWidthNumeric.Size = new System.Drawing.Size(58, 20);
            this.m_maxWidthNumeric.TabIndex = 12;
            this.m_maxWidthNumeric.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.m_maxWidthNumeric.ValueChanged += new System.EventHandler(this.editTextNode_Leave);
            this.m_maxWidthNumeric.Leave += new System.EventHandler(this.editTextNode_Leave);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(382, 313);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(61, 13);
            this.label5.TabIndex = 13;
            this.label5.Text = "Max. Width";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(382, 341);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(64, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "Max. Height";
            // 
            // m_maxHeightNumeric
            // 
            this.m_maxHeightNumeric.Location = new System.Drawing.Point(462, 339);
            this.m_maxHeightNumeric.Maximum = new decimal(new int[] {
            768,
            0,
            0,
            0});
            this.m_maxHeightNumeric.Name = "m_maxHeightNumeric";
            this.m_maxHeightNumeric.Size = new System.Drawing.Size(58, 20);
            this.m_maxHeightNumeric.TabIndex = 15;
            this.m_maxHeightNumeric.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.m_maxHeightNumeric.ValueChanged += new System.EventHandler(this.editTextNode_Leave);
            this.m_maxHeightNumeric.Leave += new System.EventHandler(this.editTextNode_Leave);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(550, 313);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(63, 13);
            this.label7.TabIndex = 16;
            this.label7.Text = "Char Offset:";
            // 
            // m_charOffsetNumeric
            // 
            this.m_charOffsetNumeric.Location = new System.Drawing.Point(632, 311);
            this.m_charOffsetNumeric.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.m_charOffsetNumeric.Name = "m_charOffsetNumeric";
            this.m_charOffsetNumeric.Size = new System.Drawing.Size(58, 20);
            this.m_charOffsetNumeric.TabIndex = 12;
            this.m_charOffsetNumeric.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.m_charOffsetNumeric.ValueChanged += new System.EventHandler(this.editTextNode_Leave);
            this.m_charOffsetNumeric.Leave += new System.EventHandler(this.editTextNode_Leave);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(550, 341);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(61, 13);
            this.label8.TabIndex = 16;
            this.label8.Text = "Line Offset:";
            // 
            // m_lineOffsetNumeric
            // 
            this.m_lineOffsetNumeric.Location = new System.Drawing.Point(632, 339);
            this.m_lineOffsetNumeric.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.m_lineOffsetNumeric.Name = "m_lineOffsetNumeric";
            this.m_lineOffsetNumeric.Size = new System.Drawing.Size(58, 20);
            this.m_lineOffsetNumeric.TabIndex = 12;
            this.m_lineOffsetNumeric.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.m_lineOffsetNumeric.ValueChanged += new System.EventHandler(this.editTextNode_Leave);
            this.m_lineOffsetNumeric.Leave += new System.EventHandler(this.editTextNode_Leave);
            // 
            // m_actualSizeLbl
            // 
            this.m_actualSizeLbl.AutoSize = true;
            this.m_actualSizeLbl.Location = new System.Drawing.Point(563, 35);
            this.m_actualSizeLbl.Name = "m_actualSizeLbl";
            this.m_actualSizeLbl.Size = new System.Drawing.Size(63, 13);
            this.m_actualSizeLbl.TabIndex = 17;
            this.m_actualSizeLbl.Text = "Actual Size:";
            // 
            // m_actualSizeEdit
            // 
            this.m_actualSizeEdit.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_actualSizeEdit.Location = new System.Drawing.Point(632, 35);
            this.m_actualSizeEdit.Name = "m_actualSizeEdit";
            this.m_actualSizeEdit.ReadOnly = true;
            this.m_actualSizeEdit.Size = new System.Drawing.Size(162, 13);
            this.m_actualSizeEdit.TabIndex = 18;
            // 
            // m_editOriginal
            // 
            this.m_editOriginal.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_editOriginal.DetectUrls = false;
            this.m_editOriginal.Location = new System.Drawing.Point(385, 51);
            this.m_editOriginal.Name = "m_editOriginal";
            this.m_editOriginal.Size = new System.Drawing.Size(413, 227);
            this.m_editOriginal.TabIndex = 19;
            this.m_editOriginal.Text = "";
            this.m_editOriginal.WordWrap = false;
            this.m_editOriginal.Leave += new System.EventHandler(this.editTextNode_Leave);
            this.m_editOriginal.TextChanged += new System.EventHandler(this.m_editOriginal_TextChanged);
            // 
            // m_treeView
            // 
            this.m_treeView.AllowDrop = true;
            this.m_treeView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.m_treeView.FullRowSelect = true;
            this.m_treeView.HideSelection = false;
            this.m_treeView.LabelEdit = true;
            this.m_treeView.Location = new System.Drawing.Point(0, 51);
            this.m_treeView.MaximumSize = new System.Drawing.Size(344, 800);
            this.m_treeView.MinimumSize = new System.Drawing.Size(344, 150);
            this.m_treeView.Name = "m_treeView";
            this.m_treeView.PathSeparator = "/";
            this.m_treeView.ShowLines = false;
            this.m_treeView.Size = new System.Drawing.Size(344, 472);
            this.m_treeView.TabIndex = 20;
            this.m_treeView.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.m_treeView_NodeMouseDoubleClick);
            this.m_treeView.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.m_treeView_AfterLabelEdit);
            this.m_treeView.DragDrop += new System.Windows.Forms.DragEventHandler(this.m_treeView_DragDrop);
            this.m_treeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.m_treeView_AfterSelect);
            this.m_treeView.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.m_treeView_ItemDrag);
            this.m_treeView.DragOver += new System.Windows.Forms.DragEventHandler(this.m_treeView_DragOver);
            // 
            // m_importDialog
            // 
            this.m_importDialog.DefaultExt = "*.csv";
            this.m_importDialog.Filter = "CSV files (*.csv)|*.csv|All files (*.*)|*.*";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(810, 523);
            this.Controls.Add(this.m_treeView);
            this.Controls.Add(this.m_editOriginal);
            this.Controls.Add(this.m_actualSizeEdit);
            this.Controls.Add(this.m_actualSizeLbl);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.m_maxHeightNumeric);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.m_lineOffsetNumeric);
            this.Controls.Add(this.m_charOffsetNumeric);
            this.Controls.Add(this.m_maxWidthNumeric);
            this.Controls.Add(this.m_fontCombo);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.m_btnMinus);
            this.Controls.Add(this.m_btnPlus);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Menge Text Editor";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_maxWidthNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_maxHeightNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_charOffsetNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_lineOffsetNumeric)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem m_exitMenuItem;
        private System.Windows.Forms.OpenFileDialog m_openFileDialog;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItemEdit;
        private System.Windows.Forms.Button m_btnPlus;
        private System.Windows.Forms.Button m_btnMinus;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox m_fontCombo;
        private System.Windows.Forms.NumericUpDown m_maxWidthNumeric;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.NumericUpDown m_maxHeightNumeric;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown m_charOffsetNumeric;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.NumericUpDown m_lineOffsetNumeric;
        private System.Windows.Forms.SaveFileDialog m_saveFileDialog;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.Label m_actualSizeLbl;
        private System.Windows.Forms.TextBox m_actualSizeEdit;
        private System.Windows.Forms.RichTextBox m_editOriginal;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem m_findNextMenuItem;
        private System.Windows.Forms.TreeView m_treeView;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
        private System.Windows.Forms.OpenFileDialog m_importDialog;
        private System.Windows.Forms.SaveFileDialog m_exportDialog;
    }
}

