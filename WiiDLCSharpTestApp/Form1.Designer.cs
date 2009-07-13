namespace WiiDLCSharpTestApp
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
            this.btnLoad = new System.Windows.Forms.Button();
            this.txtSourcePath = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.pgBasic = new System.Windows.Forms.PropertyGrid();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.pgImage = new System.Windows.Forms.PropertyGrid();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.pgImageHeader = new System.Windows.Forms.PropertyGrid();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnLoad
            // 
            this.btnLoad.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.btnLoad.Location = new System.Drawing.Point(775, 14);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(75, 23);
            this.btnLoad.TabIndex = 0;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // txtSourcePath
            // 
            this.txtSourcePath.AllowDrop = true;
            this.txtSourcePath.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtSourcePath.Location = new System.Drawing.Point(3, 16);
            this.txtSourcePath.Name = "txtSourcePath";
            this.txtSourcePath.ReadOnly = true;
            this.txtSourcePath.Size = new System.Drawing.Size(765, 20);
            this.txtSourcePath.TabIndex = 1;
            this.txtSourcePath.Text = "Drag image file here...";
            this.txtSourcePath.DragDrop += new System.Windows.Forms.DragEventHandler(this.txtSourcePath_DragDrop);
            this.txtSourcePath.DragEnter += new System.Windows.Forms.DragEventHandler(this.txtSourcePath_DragEnter);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.txtSourcePath);
            this.groupBox1.Controls.Add(this.btnLoad);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(855, 42);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Source Image...";
            // 
            // pgBasic
            // 
            this.pgBasic.CommandsDisabledLinkColor = System.Drawing.Color.Black;
            this.pgBasic.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgBasic.HelpVisible = false;
            this.pgBasic.Location = new System.Drawing.Point(3, 16);
            this.pgBasic.Name = "pgBasic";
            this.pgBasic.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgBasic.Size = new System.Drawing.Size(849, 67);
            this.pgBasic.TabIndex = 3;
            this.pgBasic.ToolbarVisible = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.pgBasic);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox2.Location = new System.Drawing.Point(0, 42);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(855, 86);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Basic Properties";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.pgImage);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox4.Location = new System.Drawing.Point(0, 128);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(855, 205);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Image Properties";
            // 
            // pgImage
            // 
            this.pgImage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgImage.HelpVisible = false;
            this.pgImage.Location = new System.Drawing.Point(3, 16);
            this.pgImage.Name = "pgImage";
            this.pgImage.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgImage.Size = new System.Drawing.Size(849, 186);
            this.pgImage.TabIndex = 0;
            this.pgImage.ToolbarVisible = false;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.pgImageHeader);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox3.Location = new System.Drawing.Point(0, 333);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(855, 115);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Image Header";
            // 
            // pgImageHeader
            // 
            this.pgImageHeader.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgImageHeader.HelpVisible = false;
            this.pgImageHeader.Location = new System.Drawing.Point(3, 16);
            this.pgImageHeader.Name = "pgImageHeader";
            this.pgImageHeader.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgImageHeader.Size = new System.Drawing.Size(849, 96);
            this.pgImageHeader.TabIndex = 1;
            this.pgImageHeader.ToolbarVisible = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(855, 448);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "WiiDL Sample C# App";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.TextBox txtSourcePath;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.PropertyGrid pgBasic;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.PropertyGrid pgImage;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.PropertyGrid pgImageHeader;
    }
}

