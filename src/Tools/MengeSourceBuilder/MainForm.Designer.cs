namespace MengeSourceBuilder
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
            this.m_btnUpdate = new System.Windows.Forms.Button();
            this.m_editDestFolder = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.m_dlgDestFolder = new System.Windows.Forms.FolderBrowserDialog();
            this.SuspendLayout();
            // 
            // m_btnUpdate
            // 
            this.m_btnUpdate.Location = new System.Drawing.Point(114, 212);
            this.m_btnUpdate.Name = "m_btnUpdate";
            this.m_btnUpdate.Size = new System.Drawing.Size(116, 40);
            this.m_btnUpdate.TabIndex = 0;
            this.m_btnUpdate.Text = "Update";
            this.m_btnUpdate.UseVisualStyleBackColor = true;
            this.m_btnUpdate.Click += new System.EventHandler(this.onUpdateClick);
            // 
            // m_editDestFolder
            // 
            this.m_editDestFolder.Location = new System.Drawing.Point(12, 41);
            this.m_editDestFolder.Name = "m_editDestFolder";
            this.m_editDestFolder.Size = new System.Drawing.Size(218, 20);
            this.m_editDestFolder.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(248, 36);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(77, 25);
            this.button1.TabIndex = 2;
            this.button1.Text = "Browse...";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.onBrowseClick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(95, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Destination Folder:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(337, 298);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.m_editDestFolder);
            this.Controls.Add(this.m_btnUpdate);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(343, 328);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(343, 328);
            this.Name = "MainForm";
            this.Text = "Menge Source Builder";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_btnUpdate;
        private System.Windows.Forms.TextBox m_editDestFolder;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.FolderBrowserDialog m_dlgDestFolder;
    }
}