namespace MengeProjectBuilder
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
            this.label1 = new System.Windows.Forms.Label();
            this.btn_src = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.m_buttonBuild = new System.Windows.Forms.Button();
            this.brsDlg_dst = new System.Windows.Forms.FolderBrowserDialog();
            this.brsDlg_src = new System.Windows.Forms.FolderBrowserDialog();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btn_Cancel = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.num_jpegQual = new System.Windows.Forms.NumericUpDown();
            this.chk_convert = new System.Windows.Forms.CheckBox();
            this.chk_alphaEdgeCorrection = new System.Windows.Forms.CheckBox();
            this.num_maxAlphaValue = new System.Windows.Forms.NumericUpDown();
            this.chk_trimAlpha = new System.Windows.Forms.CheckBox();
            this.num_atlasImageMaxSize = new System.Windows.Forms.NumericUpDown();
            this.num_atlasMaxSize = new System.Windows.Forms.NumericUpDown();
            this.chk_atlas = new System.Windows.Forms.CheckBox();
            this.cmb_src = new System.Windows.Forms.ComboBox();
            this.txtBox_dst = new System.Windows.Forms.TextBox();
            this.grp_imageResources = new System.Windows.Forms.GroupBox();
            this.chk_makePaks = new System.Windows.Forms.CheckBox();
            this.m_companyNameEdit = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.num_jpegQual)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.num_maxAlphaValue)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.num_atlasImageMaxSize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.num_atlasMaxSize)).BeginInit();
            this.grp_imageResources.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(61, 55);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(44, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Source:";
            // 
            // btn_src
            // 
            this.btn_src.Location = new System.Drawing.Point(468, 49);
            this.btn_src.Name = "btn_src";
            this.btn_src.Size = new System.Drawing.Size(33, 19);
            this.btn_src.TabIndex = 2;
            this.btn_src.Text = "...";
            this.btn_src.UseVisualStyleBackColor = true;
            this.btn_src.Click += new System.EventHandler(this.btn_src_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(43, 95);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(63, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Destination:";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(468, 90);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(33, 19);
            this.button2.TabIndex = 2;
            this.button2.Text = "...";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // m_buttonBuild
            // 
            this.m_buttonBuild.Enabled = false;
            this.m_buttonBuild.Location = new System.Drawing.Point(223, 415);
            this.m_buttonBuild.Name = "m_buttonBuild";
            this.m_buttonBuild.Size = new System.Drawing.Size(86, 27);
            this.m_buttonBuild.TabIndex = 3;
            this.m_buttonBuild.Text = "Build";
            this.m_buttonBuild.UseVisualStyleBackColor = true;
            this.m_buttonBuild.Click += new System.EventHandler(this.m_buttonBuild_Click);
            // 
            // brsDlg_dst
            // 
            this.brsDlg_dst.Description = "Choose destination folder";
            // 
            // brsDlg_src
            // 
            this.brsDlg_src.Description = "Choose svn project folder";
            this.brsDlg_src.ShowNewFolderButton = false;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(42, 95);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(82, 13);
            this.label3.TabIndex = 11;
            this.label3.Text = "Max. Atlas Size:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(211, 98);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(88, 13);
            this.label4.TabIndex = 12;
            this.label4.Text = "Max. Image Size:";
            // 
            // btn_Cancel
            // 
            this.btn_Cancel.Location = new System.Drawing.Point(223, 415);
            this.btn_Cancel.Name = "btn_Cancel";
            this.btn_Cancel.Size = new System.Drawing.Size(86, 27);
            this.btn_Cancel.TabIndex = 3;
            this.btn_Cancel.Text = "Cancel";
            this.btn_Cancel.UseVisualStyleBackColor = true;
            this.btn_Cancel.Visible = false;
            this.btn_Cancel.Click += new System.EventHandler(this.btn_Cancel_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(42, 44);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(93, 13);
            this.label5.TabIndex = 15;
            this.label5.Text = "Max. Alpha Value:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(46, 151);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(89, 13);
            this.label6.TabIndex = 20;
            this.label6.Text = "JPEG Quality (%):";
            // 
            // num_jpegQual
            // 
            this.num_jpegQual.DataBindings.Add(new System.Windows.Forms.Binding("Value", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultJPEGQual", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.num_jpegQual.Location = new System.Drawing.Point(141, 146);
            this.num_jpegQual.Name = "num_jpegQual";
            this.num_jpegQual.Size = new System.Drawing.Size(48, 20);
            this.num_jpegQual.TabIndex = 19;
            this.num_jpegQual.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.num_jpegQual.Value = global::MengeProjectBuilder.Properties.Settings.Default.DefaultJPEGQual;
            // 
            // chk_convert
            // 
            this.chk_convert.AutoSize = true;
            this.chk_convert.Checked = global::MengeProjectBuilder.Properties.Settings.Default.DefaultConvert;
            this.chk_convert.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chk_convert.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultConvert", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.chk_convert.Location = new System.Drawing.Point(12, 123);
            this.chk_convert.Name = "chk_convert";
            this.chk_convert.Size = new System.Drawing.Size(142, 17);
            this.chk_convert.TabIndex = 18;
            this.chk_convert.Text = "Convert into MNE/JPEG";
            this.chk_convert.UseVisualStyleBackColor = true;
            this.chk_convert.CheckedChanged += new System.EventHandler(this.chk_convert_CheckedChanged);
            // 
            // chk_alphaEdgeCorrection
            // 
            this.chk_alphaEdgeCorrection.AutoSize = true;
            this.chk_alphaEdgeCorrection.Checked = global::MengeProjectBuilder.Properties.Settings.Default.DefaultAlphaEdgeCorrection;
            this.chk_alphaEdgeCorrection.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultAlphaEdgeCorrection", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.chk_alphaEdgeCorrection.Location = new System.Drawing.Point(214, 43);
            this.chk_alphaEdgeCorrection.Name = "chk_alphaEdgeCorrection";
            this.chk_alphaEdgeCorrection.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.chk_alphaEdgeCorrection.Size = new System.Drawing.Size(132, 17);
            this.chk_alphaEdgeCorrection.TabIndex = 17;
            this.chk_alphaEdgeCorrection.Text = "Alpha Edge Correction";
            this.chk_alphaEdgeCorrection.UseVisualStyleBackColor = true;
            // 
            // num_maxAlphaValue
            // 
            this.num_maxAlphaValue.DataBindings.Add(new System.Windows.Forms.Binding("Value", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultMaxAlphaValue", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.num_maxAlphaValue.Location = new System.Drawing.Point(141, 42);
            this.num_maxAlphaValue.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.num_maxAlphaValue.Name = "num_maxAlphaValue";
            this.num_maxAlphaValue.Size = new System.Drawing.Size(49, 20);
            this.num_maxAlphaValue.TabIndex = 16;
            this.num_maxAlphaValue.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.num_maxAlphaValue.Value = global::MengeProjectBuilder.Properties.Settings.Default.DefaultMaxAlphaValue;
            // 
            // chk_trimAlpha
            // 
            this.chk_trimAlpha.AutoSize = true;
            this.chk_trimAlpha.Checked = global::MengeProjectBuilder.Properties.Settings.Default.DefaultTrimAlpha;
            this.chk_trimAlpha.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chk_trimAlpha.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultTrimAlpha", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.chk_trimAlpha.Location = new System.Drawing.Point(12, 19);
            this.chk_trimAlpha.Name = "chk_trimAlpha";
            this.chk_trimAlpha.Size = new System.Drawing.Size(76, 17);
            this.chk_trimAlpha.TabIndex = 14;
            this.chk_trimAlpha.Text = "Trim Alpha";
            this.chk_trimAlpha.UseVisualStyleBackColor = true;
            this.chk_trimAlpha.CheckedChanged += new System.EventHandler(this.chk_trimAlpha_CheckedChanged);
            // 
            // num_atlasImageMaxSize
            // 
            this.num_atlasImageMaxSize.DataBindings.Add(new System.Windows.Forms.Binding("Value", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultMaxImageSize", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.num_atlasImageMaxSize.Location = new System.Drawing.Point(305, 93);
            this.num_atlasImageMaxSize.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.num_atlasImageMaxSize.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.num_atlasImageMaxSize.Name = "num_atlasImageMaxSize";
            this.num_atlasImageMaxSize.Size = new System.Drawing.Size(59, 20);
            this.num_atlasImageMaxSize.TabIndex = 13;
            this.num_atlasImageMaxSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.num_atlasImageMaxSize.Value = global::MengeProjectBuilder.Properties.Settings.Default.DefaultMaxImageSize;
            // 
            // num_atlasMaxSize
            // 
            this.num_atlasMaxSize.DataBindings.Add(new System.Windows.Forms.Binding("Value", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultMaxAtlasSize", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.num_atlasMaxSize.Location = new System.Drawing.Point(130, 93);
            this.num_atlasMaxSize.Maximum = new decimal(new int[] {
            2048,
            0,
            0,
            0});
            this.num_atlasMaxSize.Minimum = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.num_atlasMaxSize.Name = "num_atlasMaxSize";
            this.num_atlasMaxSize.Size = new System.Drawing.Size(59, 20);
            this.num_atlasMaxSize.TabIndex = 13;
            this.num_atlasMaxSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.num_atlasMaxSize.Value = global::MengeProjectBuilder.Properties.Settings.Default.DefaultMaxAtlasSize;
            // 
            // chk_atlas
            // 
            this.chk_atlas.AutoSize = true;
            this.chk_atlas.Checked = global::MengeProjectBuilder.Properties.Settings.Default.DefaultMakeAtlases;
            this.chk_atlas.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chk_atlas.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultMakeAtlases", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.chk_atlas.Location = new System.Drawing.Point(12, 70);
            this.chk_atlas.Name = "chk_atlas";
            this.chk_atlas.Size = new System.Drawing.Size(90, 17);
            this.chk_atlas.TabIndex = 8;
            this.chk_atlas.Text = "Make Atlases";
            this.chk_atlas.UseVisualStyleBackColor = true;
            this.chk_atlas.CheckedChanged += new System.EventHandler(this.chk_atlas_CheckedChanged);
            // 
            // cmb_src
            // 
            this.cmb_src.AllowDrop = true;
            this.cmb_src.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.cmb_src.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystemDirectories;
            this.cmb_src.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultSource", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.cmb_src.FormattingEnabled = true;
            this.cmb_src.Location = new System.Drawing.Point(112, 47);
            this.cmb_src.Name = "cmb_src";
            this.cmb_src.Size = new System.Drawing.Size(329, 21);
            this.cmb_src.TabIndex = 7;
            this.cmb_src.Text = global::MengeProjectBuilder.Properties.Settings.Default.DefaultSource;
            // 
            // txtBox_dst
            // 
            this.txtBox_dst.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.txtBox_dst.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystemDirectories;
            this.txtBox_dst.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultDestination", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.txtBox_dst.Location = new System.Drawing.Point(111, 89);
            this.txtBox_dst.Name = "txtBox_dst";
            this.txtBox_dst.Size = new System.Drawing.Size(330, 20);
            this.txtBox_dst.TabIndex = 0;
            this.txtBox_dst.Text = global::MengeProjectBuilder.Properties.Settings.Default.DefaultDestination;
            this.txtBox_dst.TextChanged += new System.EventHandler(this.textBox2_TextChanged);
            // 
            // grp_imageResources
            // 
            this.grp_imageResources.Controls.Add(this.label6);
            this.grp_imageResources.Controls.Add(this.num_jpegQual);
            this.grp_imageResources.Controls.Add(this.chk_convert);
            this.grp_imageResources.Controls.Add(this.chk_alphaEdgeCorrection);
            this.grp_imageResources.Controls.Add(this.num_maxAlphaValue);
            this.grp_imageResources.Controls.Add(this.label5);
            this.grp_imageResources.Controls.Add(this.chk_trimAlpha);
            this.grp_imageResources.Controls.Add(this.num_atlasImageMaxSize);
            this.grp_imageResources.Controls.Add(this.num_atlasMaxSize);
            this.grp_imageResources.Controls.Add(this.label4);
            this.grp_imageResources.Controls.Add(this.label3);
            this.grp_imageResources.Controls.Add(this.chk_atlas);
            this.grp_imageResources.Location = new System.Drawing.Point(34, 148);
            this.grp_imageResources.Name = "grp_imageResources";
            this.grp_imageResources.Size = new System.Drawing.Size(466, 182);
            this.grp_imageResources.TabIndex = 21;
            this.grp_imageResources.TabStop = false;
            this.grp_imageResources.Text = "Image Resources";
            // 
            // chk_makePaks
            // 
            this.chk_makePaks.AutoSize = true;
            this.chk_makePaks.Checked = global::MengeProjectBuilder.Properties.Settings.Default.DefaultMakePaks;
            this.chk_makePaks.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chk_makePaks.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultMakePaks", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.chk_makePaks.Location = new System.Drawing.Point(46, 355);
            this.chk_makePaks.Name = "chk_makePaks";
            this.chk_makePaks.Size = new System.Drawing.Size(80, 17);
            this.chk_makePaks.TabIndex = 22;
            this.chk_makePaks.Text = "Make Paks";
            this.chk_makePaks.UseVisualStyleBackColor = true;
            // 
            // m_companyNameEdit
            // 
            this.m_companyNameEdit.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::MengeProjectBuilder.Properties.Settings.Default, "DefaultCompanyName", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.m_companyNameEdit.Location = new System.Drawing.Point(263, 352);
            this.m_companyNameEdit.Name = "m_companyNameEdit";
            this.m_companyNameEdit.Size = new System.Drawing.Size(237, 20);
            this.m_companyNameEdit.TabIndex = 23;
            this.m_companyNameEdit.Text = global::MengeProjectBuilder.Properties.Settings.Default.DefaultCompanyName;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(172, 356);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(85, 13);
            this.label7.TabIndex = 24;
            this.label7.Text = "Company Name:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(529, 454);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.m_companyNameEdit);
            this.Controls.Add(this.chk_makePaks);
            this.Controls.Add(this.grp_imageResources);
            this.Controls.Add(this.cmb_src);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btn_src);
            this.Controls.Add(this.txtBox_dst);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_buttonBuild);
            this.Controls.Add(this.btn_Cancel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "Menge Project Builder";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.num_jpegQual)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.num_maxAlphaValue)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.num_atlasImageMaxSize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.num_atlasMaxSize)).EndInit();
            this.grp_imageResources.ResumeLayout(false);
            this.grp_imageResources.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btn_src;
        private System.Windows.Forms.TextBox txtBox_dst;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button m_buttonBuild;
        private System.Windows.Forms.FolderBrowserDialog brsDlg_src;
        private System.Windows.Forms.ComboBox cmb_src;
        private System.Windows.Forms.FolderBrowserDialog brsDlg_dst;
        delegate void SetControlValueCallback(System.Windows.Forms.Control oControl, string propName, object propValue);

        private string m_utilsPath;
        private string m_svnBin;
        private System.Threading.Thread m_thread;
        private System.Windows.Forms.CheckBox chk_atlas;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown num_atlasMaxSize;
        private System.Windows.Forms.NumericUpDown num_atlasImageMaxSize;
        private System.Windows.Forms.Button btn_Cancel;
        private System.Windows.Forms.CheckBox chk_trimAlpha;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown num_maxAlphaValue;
        private System.Windows.Forms.CheckBox chk_alphaEdgeCorrection;
        private System.Windows.Forms.CheckBox chk_convert;
        private System.Windows.Forms.NumericUpDown num_jpegQual;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox grp_imageResources;
        private System.Windows.Forms.CheckBox chk_makePaks;
        private System.Windows.Forms.TextBox m_companyNameEdit;
        private System.Windows.Forms.Label label7;
    }
}

