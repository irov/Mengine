namespace MengeTextLocalizer
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
            this.m_mainMenu = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newLocalizationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_openLocalizationMainMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_saveLocalizationMainMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.m_exitMainMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label1 = new System.Windows.Forms.Label();
            this.m_editOriginal = new System.Windows.Forms.RichTextBox();
            this.m_maxSizeEdit = new System.Windows.Forms.TextBox();
            this.m_maxSizeLbl = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.m_fontCombo = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.m_lineOffsetNumeric = new System.Windows.Forms.NumericUpDown();
            this.m_charOffsetNumeric = new System.Windows.Forms.NumericUpDown();
            this.m_editTranslation = new System.Windows.Forms.RichTextBox();
            this.m_translationLbl = new System.Windows.Forms.Label();
            this.m_actualSizeLbl = new System.Windows.Forms.Label();
            this.m_actualSizeEdit = new System.Windows.Forms.TextBox();
            this.m_doneLbl = new System.Windows.Forms.Label();
            this.m_doneEdit = new System.Windows.Forms.TextBox();
            this.m_itemList = new MengeTextLocalizer.CheckedListBoxColored();
            this.m_openFileLocal = new System.Windows.Forms.OpenFileDialog();
            this.m_openOriginal = new System.Windows.Forms.OpenFileDialog();
            this.m_mainMenu.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_lineOffsetNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_charOffsetNumeric)).BeginInit();
            this.SuspendLayout();
            // 
            // m_mainMenu
            // 
            this.m_mainMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.m_mainMenu.Location = new System.Drawing.Point(0, 0);
            this.m_mainMenu.Name = "m_mainMenu";
            this.m_mainMenu.Size = new System.Drawing.Size(810, 24);
            this.m_mainMenu.TabIndex = 1;
            this.m_mainMenu.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newLocalizationToolStripMenuItem,
            this.m_openLocalizationMainMenuItem,
            this.m_saveLocalizationMainMenuItem,
            this.toolStripSeparator1,
            this.m_exitMainMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // newLocalizationToolStripMenuItem
            // 
            this.newLocalizationToolStripMenuItem.Name = "newLocalizationToolStripMenuItem";
            this.newLocalizationToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
            this.newLocalizationToolStripMenuItem.Text = "&New Localization";
            // 
            // m_openLocalizationMainMenuItem
            // 
            this.m_openLocalizationMainMenuItem.Name = "m_openLocalizationMainMenuItem";
            this.m_openLocalizationMainMenuItem.Size = new System.Drawing.Size(169, 22);
            this.m_openLocalizationMainMenuItem.Text = "&Open Localization";
            this.m_openLocalizationMainMenuItem.Click += new System.EventHandler(this.m_openLocalizationMainMenuItem_Click);
            // 
            // m_saveLocalizationMainMenuItem
            // 
            this.m_saveLocalizationMainMenuItem.Name = "m_saveLocalizationMainMenuItem";
            this.m_saveLocalizationMainMenuItem.Size = new System.Drawing.Size(169, 22);
            this.m_saveLocalizationMainMenuItem.Text = "Save Localization";
            this.m_saveLocalizationMainMenuItem.Click += new System.EventHandler(this.m_saveLocalizationMainMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(166, 6);
            // 
            // m_exitMainMenuItem
            // 
            this.m_exitMainMenuItem.Name = "m_exitMainMenuItem";
            this.m_exitMainMenuItem.Size = new System.Drawing.Size(169, 22);
            this.m_exitMainMenuItem.Text = "E&xit";
            this.m_exitMainMenuItem.Click += new System.EventHandler(this.m_exitMainMenuItem_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(-3, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(94, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Items to Translate:";
            // 
            // m_editOriginal
            // 
            this.m_editOriginal.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.m_editOriginal.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.m_editOriginal.Location = new System.Drawing.Point(385, 51);
            this.m_editOriginal.Name = "m_editOriginal";
            this.m_editOriginal.ReadOnly = true;
            this.m_editOriginal.Size = new System.Drawing.Size(413, 180);
            this.m_editOriginal.TabIndex = 3;
            this.m_editOriginal.Text = "";
            // 
            // m_maxSizeEdit
            // 
            this.m_maxSizeEdit.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_maxSizeEdit.Location = new System.Drawing.Point(632, 35);
            this.m_maxSizeEdit.Name = "m_maxSizeEdit";
            this.m_maxSizeEdit.ReadOnly = true;
            this.m_maxSizeEdit.Size = new System.Drawing.Size(162, 13);
            this.m_maxSizeEdit.TabIndex = 21;
            // 
            // m_maxSizeLbl
            // 
            this.m_maxSizeLbl.AutoSize = true;
            this.m_maxSizeLbl.Location = new System.Drawing.Point(563, 35);
            this.m_maxSizeLbl.Name = "m_maxSizeLbl";
            this.m_maxSizeLbl.Size = new System.Drawing.Size(53, 13);
            this.m_maxSizeLbl.TabIndex = 20;
            this.m_maxSizeLbl.Text = "Max Size:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(382, 35);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 13);
            this.label2.TabIndex = 19;
            this.label2.Text = "Original Text:";
            // 
            // m_fontCombo
            // 
            this.m_fontCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_fontCombo.FormattingEnabled = true;
            this.m_fontCombo.Location = new System.Drawing.Point(419, 460);
            this.m_fontCombo.Name = "m_fontCombo";
            this.m_fontCombo.Size = new System.Drawing.Size(167, 21);
            this.m_fontCombo.TabIndex = 23;
            this.m_fontCombo.SelectionChangeCommitted += new System.EventHandler(this.m_fontCombo_SelectionChangeCommitted);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(382, 463);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 13);
            this.label4.TabIndex = 22;
            this.label4.Text = "Font:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(540, 493);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(61, 13);
            this.label8.TabIndex = 26;
            this.label8.Text = "Line Offset:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(382, 493);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(63, 13);
            this.label7.TabIndex = 27;
            this.label7.Text = "Char Offset:";
            // 
            // m_lineOffsetNumeric
            // 
            this.m_lineOffsetNumeric.Location = new System.Drawing.Point(622, 491);
            this.m_lineOffsetNumeric.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.m_lineOffsetNumeric.Name = "m_lineOffsetNumeric";
            this.m_lineOffsetNumeric.Size = new System.Drawing.Size(58, 20);
            this.m_lineOffsetNumeric.TabIndex = 24;
            this.m_lineOffsetNumeric.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.m_lineOffsetNumeric.ValueChanged += new System.EventHandler(this.m_lineOffsetNumeric_ValueChanged);
            // 
            // m_charOffsetNumeric
            // 
            this.m_charOffsetNumeric.Location = new System.Drawing.Point(464, 491);
            this.m_charOffsetNumeric.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            -2147483648});
            this.m_charOffsetNumeric.Name = "m_charOffsetNumeric";
            this.m_charOffsetNumeric.Size = new System.Drawing.Size(58, 20);
            this.m_charOffsetNumeric.TabIndex = 25;
            this.m_charOffsetNumeric.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.m_charOffsetNumeric.ValueChanged += new System.EventHandler(this.m_charOffsetNumeric_ValueChanged);
            // 
            // m_editTranslation
            // 
            this.m_editTranslation.Location = new System.Drawing.Point(385, 259);
            this.m_editTranslation.Name = "m_editTranslation";
            this.m_editTranslation.Size = new System.Drawing.Size(413, 180);
            this.m_editTranslation.TabIndex = 28;
            this.m_editTranslation.Text = "";
            this.m_editTranslation.TextChanged += new System.EventHandler(this.m_editTranslation_TextChanged);
            // 
            // m_translationLbl
            // 
            this.m_translationLbl.AutoSize = true;
            this.m_translationLbl.Location = new System.Drawing.Point(382, 243);
            this.m_translationLbl.Name = "m_translationLbl";
            this.m_translationLbl.Size = new System.Drawing.Size(62, 13);
            this.m_translationLbl.TabIndex = 29;
            this.m_translationLbl.Text = "Translation:";
            // 
            // m_actualSizeLbl
            // 
            this.m_actualSizeLbl.AutoSize = true;
            this.m_actualSizeLbl.Location = new System.Drawing.Point(563, 243);
            this.m_actualSizeLbl.Name = "m_actualSizeLbl";
            this.m_actualSizeLbl.Size = new System.Drawing.Size(63, 13);
            this.m_actualSizeLbl.TabIndex = 30;
            this.m_actualSizeLbl.Text = "Actual Size:";
            // 
            // m_actualSizeEdit
            // 
            this.m_actualSizeEdit.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_actualSizeEdit.Location = new System.Drawing.Point(632, 243);
            this.m_actualSizeEdit.Name = "m_actualSizeEdit";
            this.m_actualSizeEdit.ReadOnly = true;
            this.m_actualSizeEdit.Size = new System.Drawing.Size(162, 13);
            this.m_actualSizeEdit.TabIndex = 31;
            // 
            // m_doneLbl
            // 
            this.m_doneLbl.AutoSize = true;
            this.m_doneLbl.Location = new System.Drawing.Point(214, 35);
            this.m_doneLbl.Name = "m_doneLbl";
            this.m_doneLbl.Size = new System.Drawing.Size(36, 13);
            this.m_doneLbl.TabIndex = 32;
            this.m_doneLbl.Text = "Done:";
            // 
            // m_doneEdit
            // 
            this.m_doneEdit.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.m_doneEdit.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_doneEdit.Location = new System.Drawing.Point(256, 35);
            this.m_doneEdit.Name = "m_doneEdit";
            this.m_doneEdit.ReadOnly = true;
            this.m_doneEdit.Size = new System.Drawing.Size(88, 13);
            this.m_doneEdit.TabIndex = 33;
            this.m_doneEdit.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // m_itemList
            // 
            this.m_itemList.FormattingEnabled = true;
            this.m_itemList.Location = new System.Drawing.Point(0, 51);
            this.m_itemList.Name = "m_itemList";
            this.m_itemList.Size = new System.Drawing.Size(344, 469);
            this.m_itemList.TabIndex = 0;
            this.m_itemList.SelectedIndexChanged += new System.EventHandler(this.m_itemList_SelectedIndexChanged);
            this.m_itemList.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.m_itemList_ItemCheck);
            // 
            // m_openFileLocal
            // 
            this.m_openFileLocal.FileName = global::MengeTextLocalizer.Properties.Settings.Default.s_localFilename;
            this.m_openFileLocal.Filter = "Resource XML Files (*.xml)|*.xml|All Files (*.*)|*.*";
            this.m_openFileLocal.InitialDirectory = global::MengeTextLocalizer.Properties.Settings.Default.s_initDirLocal;
            this.m_openFileLocal.Title = "Choose Resource XML file of Localization pack";
            // 
            // m_openOriginal
            // 
            this.m_openOriginal.FileName = "Resource.xml";
            this.m_openOriginal.Filter = "Resource XML Files (*.xml)|*.xml|All Files (*.*)|*.*";
            this.m_openOriginal.InitialDirectory = global::MengeTextLocalizer.Properties.Settings.Default.s_initDirOriginal;
            this.m_openOriginal.Title = "Choose Resource XML file of Original pack";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(810, 523);
            this.Controls.Add(this.m_doneEdit);
            this.Controls.Add(this.m_doneLbl);
            this.Controls.Add(this.m_actualSizeEdit);
            this.Controls.Add(this.m_actualSizeLbl);
            this.Controls.Add(this.m_translationLbl);
            this.Controls.Add(this.m_editTranslation);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.m_lineOffsetNumeric);
            this.Controls.Add(this.m_charOffsetNumeric);
            this.Controls.Add(this.m_fontCombo);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.m_maxSizeEdit);
            this.Controls.Add(this.m_maxSizeLbl);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_editOriginal);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_itemList);
            this.Controls.Add(this.m_mainMenu);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.m_mainMenu;
            this.Name = "Form1";
            this.Text = "Menge Text Localizer";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.m_mainMenu.ResumeLayout(false);
            this.m_mainMenu.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_lineOffsetNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_charOffsetNumeric)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private CheckedListBoxColored m_itemList;
        private System.Windows.Forms.MenuStrip m_mainMenu;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newLocalizationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem m_openLocalizationMainMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem m_exitMainMenuItem;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ToolStripMenuItem m_saveLocalizationMainMenuItem;
        private System.Windows.Forms.RichTextBox m_editOriginal;
        private System.Windows.Forms.TextBox m_maxSizeEdit;
        private System.Windows.Forms.Label m_maxSizeLbl;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox m_fontCombo;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown m_lineOffsetNumeric;
        private System.Windows.Forms.NumericUpDown m_charOffsetNumeric;
        private System.Windows.Forms.RichTextBox m_editTranslation;
        private System.Windows.Forms.Label m_translationLbl;
        private System.Windows.Forms.OpenFileDialog m_openOriginal;
        private System.Windows.Forms.OpenFileDialog m_openFileLocal;
        private System.Windows.Forms.Label m_actualSizeLbl;
        private System.Windows.Forms.TextBox m_actualSizeEdit;
        private System.Windows.Forms.Label m_doneLbl;
        private System.Windows.Forms.TextBox m_doneEdit;
    }
}

