namespace ProjectManager
{
    partial class NewProjectForm
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
            this.AvaibleTemplate = new System.Windows.Forms.Label();
            this.NewProjectName = new System.Windows.Forms.TextBox();
            this.ProjectName = new System.Windows.Forms.Label();
            this.OkButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.ProjectLocation = new System.Windows.Forms.TextBox();
            this.TemplateComboBox = new System.Windows.Forms.ComboBox();
            this.FolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.FolderBrowserButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // AvaibleTemplate
            // 
            this.AvaibleTemplate.AutoSize = true;
            this.AvaibleTemplate.Location = new System.Drawing.Point(12, 9);
            this.AvaibleTemplate.Name = "AvaibleTemplate";
            this.AvaibleTemplate.Size = new System.Drawing.Size(89, 13);
            this.AvaibleTemplate.TabIndex = 0;
            this.AvaibleTemplate.Text = "Avaible Template";
            // 
            // NewProjectName
            // 
            this.NewProjectName.Location = new System.Drawing.Point(15, 69);
            this.NewProjectName.Name = "NewProjectName";
            this.NewProjectName.Size = new System.Drawing.Size(291, 20);
            this.NewProjectName.TabIndex = 1;
            this.NewProjectName.Text = "NewProject";
            // 
            // ProjectName
            // 
            this.ProjectName.AutoSize = true;
            this.ProjectName.Location = new System.Drawing.Point(12, 53);
            this.ProjectName.Name = "ProjectName";
            this.ProjectName.Size = new System.Drawing.Size(71, 13);
            this.ProjectName.TabIndex = 2;
            this.ProjectName.Text = "Project Name";
            // 
            // OkButton
            // 
            this.OkButton.Location = new System.Drawing.Point(15, 146);
            this.OkButton.Name = "OkButton";
            this.OkButton.Size = new System.Drawing.Size(75, 23);
            this.OkButton.TabIndex = 3;
            this.OkButton.Text = "OK";
            this.OkButton.UseVisualStyleBackColor = true;
            this.OkButton.Click += new System.EventHandler(this.OkButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 92);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(84, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Project Location";
            // 
            // ProjectLocation
            // 
            this.ProjectLocation.Location = new System.Drawing.Point(15, 108);
            this.ProjectLocation.Name = "ProjectLocation";
            this.ProjectLocation.Size = new System.Drawing.Size(260, 20);
            this.ProjectLocation.TabIndex = 6;
            this.ProjectLocation.Text = "D:\\";
            // 
            // TemplateComboBox
            // 
            this.TemplateComboBox.Location = new System.Drawing.Point(15, 25);
            this.TemplateComboBox.Name = "TemplateComboBox";
            this.TemplateComboBox.Size = new System.Drawing.Size(291, 21);
            this.TemplateComboBox.TabIndex = 0;
            // 
            // FolderBrowserButton
            // 
            this.FolderBrowserButton.Location = new System.Drawing.Point(281, 107);
            this.FolderBrowserButton.Name = "FolderBrowserButton";
            this.FolderBrowserButton.Size = new System.Drawing.Size(25, 23);
            this.FolderBrowserButton.TabIndex = 7;
            this.FolderBrowserButton.Text = "...";
            this.FolderBrowserButton.UseVisualStyleBackColor = true;
            this.FolderBrowserButton.Click += new System.EventHandler(this.FolderBrowserButton_Click);
            // 
            // NewProjectForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(318, 189);
            this.Controls.Add(this.FolderBrowserButton);
            this.Controls.Add(this.TemplateComboBox);
            this.Controls.Add(this.ProjectLocation);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.OkButton);
            this.Controls.Add(this.ProjectName);
            this.Controls.Add(this.NewProjectName);
            this.Controls.Add(this.AvaibleTemplate);
            this.Name = "NewProjectForm";
            this.Text = "NewProject";
            this.Load += new System.EventHandler(this.NewProject_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label AvaibleTemplate;
        private System.Windows.Forms.TextBox NewProjectName;
        private System.Windows.Forms.Label ProjectName;
        private System.Windows.Forms.Button OkButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox ProjectLocation;
        private System.Windows.Forms.ComboBox TemplateComboBox;
        private System.Windows.Forms.FolderBrowserDialog FolderBrowserDialog;
        private System.Windows.Forms.Button FolderBrowserButton;
    }
}