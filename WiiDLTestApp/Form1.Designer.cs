namespace WiiDLTestApp
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
            this.txtIsoPath = new System.Windows.Forms.TextBox();
            this.btnOpenIso = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // txtIsoPath
            // 
            this.txtIsoPath.AllowDrop = true;
            this.txtIsoPath.Location = new System.Drawing.Point(3, 3);
            this.txtIsoPath.Name = "txtIsoPath";
            this.txtIsoPath.Size = new System.Drawing.Size(199, 20);
            this.txtIsoPath.TabIndex = 0;
            this.txtIsoPath.DragDrop += new System.Windows.Forms.DragEventHandler(this.txtIsoPath_DragDrop);
            this.txtIsoPath.DragEnter += new System.Windows.Forms.DragEventHandler(this.txtIsoPath_DragEnter);
            // 
            // btnOpenIso
            // 
            this.btnOpenIso.Location = new System.Drawing.Point(206, 1);
            this.btnOpenIso.Name = "btnOpenIso";
            this.btnOpenIso.Size = new System.Drawing.Size(75, 23);
            this.btnOpenIso.TabIndex = 1;
            this.btnOpenIso.Text = "Open";
            this.btnOpenIso.UseVisualStyleBackColor = true;
            this.btnOpenIso.Click += new System.EventHandler(this.btnOpenIso_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(283, 267);
            this.Controls.Add(this.btnOpenIso);
            this.Controls.Add(this.txtIsoPath);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtIsoPath;
        private System.Windows.Forms.Button btnOpenIso;
    }
}

