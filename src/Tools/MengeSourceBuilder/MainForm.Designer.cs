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
            this.components = new System.ComponentModel.Container();
            this.m_btnUpdate = new System.Windows.Forms.Button();
            this.m_editDestFolder = new System.Windows.Forms.TextBox();
            this.m_btnBrowse = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.m_dlgDestFolder = new System.Windows.Forms.FolderBrowserDialog();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_progressAll = new System.Windows.Forms.ProgressBar();
            this.m_jobsTimer = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // m_btnUpdate
            // 
            this.m_btnUpdate.Location = new System.Drawing.Point(164, 211);
            this.m_btnUpdate.Name = "m_btnUpdate";
            this.m_btnUpdate.Size = new System.Drawing.Size(116, 40);
            this.m_btnUpdate.TabIndex = 3;
            this.m_btnUpdate.Text = "Update";
            this.m_btnUpdate.UseVisualStyleBackColor = true;
            this.m_btnUpdate.Click += new System.EventHandler(this.onUpdateClick);
            // 
            // m_editDestFolder
            // 
            this.m_editDestFolder.Location = new System.Drawing.Point(12, 41);
            this.m_editDestFolder.Name = "m_editDestFolder";
            this.m_editDestFolder.Size = new System.Drawing.Size(316, 20);
            this.m_editDestFolder.TabIndex = 1;
            // 
            // m_btnBrowse
            // 
            this.m_btnBrowse.Location = new System.Drawing.Point(334, 36);
            this.m_btnBrowse.Name = "m_btnBrowse";
            this.m_btnBrowse.Size = new System.Drawing.Size(77, 25);
            this.m_btnBrowse.TabIndex = 2;
            this.m_btnBrowse.Text = "Browse...";
            this.m_btnBrowse.UseVisualStyleBackColor = true;
            this.m_btnBrowse.Click += new System.EventHandler(this.onBrowseClick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(95, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Destination Folder:";
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.Location = new System.Drawing.Point(164, 211);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.Size = new System.Drawing.Size(116, 40);
            this.m_btnCancel.TabIndex = 4;
            this.m_btnCancel.Text = "Cancel";
            this.m_btnCancel.UseVisualStyleBackColor = true;
            this.m_btnCancel.Visible = false;
            this.m_btnCancel.Click += new System.EventHandler(this.onCancelClick);
            // 
            // m_progressAll
            // 
            this.m_progressAll.Location = new System.Drawing.Point(41, 166);
            this.m_progressAll.Name = "m_progressAll";
            this.m_progressAll.Size = new System.Drawing.Size(340, 23);
            this.m_progressAll.TabIndex = 5;
            this.m_progressAll.Visible = false;
            // 
            // m_jobsTimer
            // 
            this.m_jobsTimer.Interval = 1000;
            this.m_jobsTimer.Tick += new System.EventHandler(this.onJobsUpdateEvent_);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(423, 298);
            this.Controls.Add(this.m_progressAll);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_btnBrowse);
            this.Controls.Add(this.m_editDestFolder);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnUpdate);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.Text = "Menge Source Builder";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.onFormClosed);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_btnUpdate;
        private System.Windows.Forms.TextBox m_editDestFolder;
        private System.Windows.Forms.Button m_btnBrowse;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.FolderBrowserDialog m_dlgDestFolder;
        private System.Windows.Forms.Button m_btnCancel;
        private System.Windows.Forms.ProgressBar m_progressAll;
        private System.Windows.Forms.Timer m_jobsTimer;
    }
}