namespace MengeTextEditor
{
    partial class FindNext
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
            this.label1 = new System.Windows.Forms.Label();
            this.m_findTextEdit = new System.Windows.Forms.TextBox();
            this.m_findBtn = new System.Windows.Forms.Button();
            this.m_matchCaseChk = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 19);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Find what:";
            // 
            // m_findTextEdit
            // 
            this.m_findTextEdit.Location = new System.Drawing.Point(12, 35);
            this.m_findTextEdit.Name = "m_findTextEdit";
            this.m_findTextEdit.Size = new System.Drawing.Size(306, 20);
            this.m_findTextEdit.TabIndex = 1;
            this.m_findTextEdit.KeyDown += new System.Windows.Forms.KeyEventHandler(this.m_findTextEdit_KeyDown);
            // 
            // m_findBtn
            // 
            this.m_findBtn.Location = new System.Drawing.Point(189, 101);
            this.m_findBtn.Name = "m_findBtn";
            this.m_findBtn.Size = new System.Drawing.Size(127, 28);
            this.m_findBtn.TabIndex = 2;
            this.m_findBtn.Text = "Find";
            this.m_findBtn.UseVisualStyleBackColor = true;
            this.m_findBtn.Click += new System.EventHandler(this.m_findBtn_Click);
            // 
            // m_matchCaseChk
            // 
            this.m_matchCaseChk.AutoSize = true;
            this.m_matchCaseChk.Checked = global::MengeTextEditor.Properties.Settings.Default.FindNextMatchCaseChk;
            this.m_matchCaseChk.DataBindings.Add(new System.Windows.Forms.Binding("Checked", global::MengeTextEditor.Properties.Settings.Default, "FindNextMatchCaseChk", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.m_matchCaseChk.Location = new System.Drawing.Point(12, 61);
            this.m_matchCaseChk.Name = "m_matchCaseChk";
            this.m_matchCaseChk.Size = new System.Drawing.Size(82, 17);
            this.m_matchCaseChk.TabIndex = 3;
            this.m_matchCaseChk.Text = "Match case";
            this.m_matchCaseChk.UseVisualStyleBackColor = true;
            // 
            // FindNext
            // 
            this.AcceptButton = this.m_findBtn;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(328, 141);
            this.Controls.Add(this.m_matchCaseChk);
            this.Controls.Add(this.m_findBtn);
            this.Controls.Add(this.m_findTextEdit);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FindNext";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "FindNext";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FindNext_FormClosing);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FindNext_KeyDown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox m_findTextEdit;
        private System.Windows.Forms.Button m_findBtn;
        private System.Windows.Forms.CheckBox m_matchCaseChk;
    }
}