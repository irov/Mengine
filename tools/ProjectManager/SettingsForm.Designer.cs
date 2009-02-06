namespace ProjectManager
{
    partial class SettingsForm
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
            this.SoundCheckBox = new System.Windows.Forms.CheckBox();
            this.DebugInfoCheckBox = new System.Windows.Forms.CheckBox();
            this.ParticlesCheckBox = new System.Windows.Forms.CheckBox();
            this.OkButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // SoundCheckBox
            // 
            this.SoundCheckBox.AutoSize = true;
            this.SoundCheckBox.Checked = true;
            this.SoundCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.SoundCheckBox.Location = new System.Drawing.Point(12, 12);
            this.SoundCheckBox.Name = "SoundCheckBox";
            this.SoundCheckBox.Size = new System.Drawing.Size(55, 17);
            this.SoundCheckBox.TabIndex = 0;
            this.SoundCheckBox.Text = "sound";
            this.SoundCheckBox.UseVisualStyleBackColor = true;
            // 
            // DebugInfoCheckBox
            // 
            this.DebugInfoCheckBox.AutoSize = true;
            this.DebugInfoCheckBox.Location = new System.Drawing.Point(12, 35);
            this.DebugInfoCheckBox.Name = "DebugInfoCheckBox";
            this.DebugInfoCheckBox.Size = new System.Drawing.Size(73, 17);
            this.DebugInfoCheckBox.TabIndex = 1;
            this.DebugInfoCheckBox.Text = "debuginfo";
            this.DebugInfoCheckBox.UseVisualStyleBackColor = true;
            // 
            // ParticlesCheckBox
            // 
            this.ParticlesCheckBox.AutoSize = true;
            this.ParticlesCheckBox.Checked = true;
            this.ParticlesCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ParticlesCheckBox.Location = new System.Drawing.Point(12, 57);
            this.ParticlesCheckBox.Name = "ParticlesCheckBox";
            this.ParticlesCheckBox.Size = new System.Drawing.Size(65, 17);
            this.ParticlesCheckBox.TabIndex = 2;
            this.ParticlesCheckBox.Text = "particles";
            this.ParticlesCheckBox.UseVisualStyleBackColor = true;
            // 
            // OkButton
            // 
            this.OkButton.Location = new System.Drawing.Point(12, 116);
            this.OkButton.Name = "OkButton";
            this.OkButton.Size = new System.Drawing.Size(75, 23);
            this.OkButton.TabIndex = 4;
            this.OkButton.Text = "OK";
            this.OkButton.UseVisualStyleBackColor = true;
            this.OkButton.Click += new System.EventHandler(this.OkButton_Click);
            // 
            // SettingsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(334, 151);
            this.Controls.Add(this.OkButton);
            this.Controls.Add(this.ParticlesCheckBox);
            this.Controls.Add(this.DebugInfoCheckBox);
            this.Controls.Add(this.SoundCheckBox);
            this.Name = "SettingsForm";
            this.Text = "Settings";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox SoundCheckBox;
        private System.Windows.Forms.CheckBox DebugInfoCheckBox;
        private System.Windows.Forms.CheckBox ParticlesCheckBox;
        private System.Windows.Forms.Button OkButton;
    }
}