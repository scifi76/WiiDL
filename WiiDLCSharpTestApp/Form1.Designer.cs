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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.pgPartInfo = new System.Windows.Forms.PropertyGrid();
            this.lvPartitions = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.pgFileInfo = new System.Windows.Forms.PropertyGrid();
            this.tvFiles = new System.Windows.Forms.TreeView();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnLoad
            // 
            this.btnLoad.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.btnLoad.Location = new System.Drawing.Point(745, 14);
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
            this.txtSourcePath.Size = new System.Drawing.Size(735, 20);
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
            this.groupBox1.Size = new System.Drawing.Size(825, 42);
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
            this.pgBasic.Margin = new System.Windows.Forms.Padding(1);
            this.pgBasic.Name = "pgBasic";
            this.pgBasic.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgBasic.Size = new System.Drawing.Size(819, 49);
            this.pgBasic.TabIndex = 3;
            this.pgBasic.ToolbarVisible = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.pgBasic);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox2.Location = new System.Drawing.Point(0, 42);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(825, 68);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Basic Properties";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.pgImage);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox4.Location = new System.Drawing.Point(0, 0);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(423, 191);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Image Properties";
            // 
            // pgImage
            // 
            this.pgImage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgImage.HelpVisible = false;
            this.pgImage.Location = new System.Drawing.Point(3, 16);
            this.pgImage.Margin = new System.Windows.Forms.Padding(1);
            this.pgImage.Name = "pgImage";
            this.pgImage.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgImage.Size = new System.Drawing.Size(417, 172);
            this.pgImage.TabIndex = 0;
            this.pgImage.ToolbarVisible = false;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.pgImageHeader);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox3.Location = new System.Drawing.Point(0, 0);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(398, 191);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Image Header";
            // 
            // pgImageHeader
            // 
            this.pgImageHeader.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pgImageHeader.HelpVisible = false;
            this.pgImageHeader.Location = new System.Drawing.Point(3, 16);
            this.pgImageHeader.Margin = new System.Windows.Forms.Padding(1);
            this.pgImageHeader.Name = "pgImageHeader";
            this.pgImageHeader.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgImageHeader.Size = new System.Drawing.Size(392, 172);
            this.pgImageHeader.TabIndex = 1;
            this.pgImageHeader.ToolbarVisible = false;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Top;
            this.splitContainer1.Location = new System.Drawing.Point(0, 110);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.groupBox3);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.groupBox4);
            this.splitContainer1.Size = new System.Drawing.Size(825, 191);
            this.splitContainer1.SplitterDistance = 398;
            this.splitContainer1.TabIndex = 7;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer2.Location = new System.Drawing.Point(0, 302);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.groupBox5);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.groupBox6);
            this.splitContainer2.Size = new System.Drawing.Size(825, 286);
            this.splitContainer2.SplitterDistance = 400;
            this.splitContainer2.TabIndex = 8;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.pgPartInfo);
            this.groupBox5.Controls.Add(this.lvPartitions);
            this.groupBox5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox5.Location = new System.Drawing.Point(0, 0);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(400, 286);
            this.groupBox5.TabIndex = 0;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Partitions";
            // 
            // pgPartInfo
            // 
            this.pgPartInfo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pgPartInfo.HelpVisible = false;
            this.pgPartInfo.Location = new System.Drawing.Point(3, 210);
            this.pgPartInfo.Margin = new System.Windows.Forms.Padding(1);
            this.pgPartInfo.Name = "pgPartInfo";
            this.pgPartInfo.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgPartInfo.Size = new System.Drawing.Size(393, 73);
            this.pgPartInfo.TabIndex = 2;
            this.pgPartInfo.ToolbarVisible = false;
            // 
            // lvPartitions
            // 
            this.lvPartitions.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lvPartitions.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this.lvPartitions.FullRowSelect = true;
            this.lvPartitions.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.lvPartitions.Location = new System.Drawing.Point(3, 16);
            this.lvPartitions.Name = "lvPartitions";
            this.lvPartitions.Size = new System.Drawing.Size(393, 195);
            this.lvPartitions.TabIndex = 0;
            this.lvPartitions.UseCompatibleStateImageBehavior = false;
            this.lvPartitions.View = System.Windows.Forms.View.Details;
            this.lvPartitions.SelectedIndexChanged += new System.EventHandler(this.lvPartitions_SelectedIndexChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Partition";
            this.columnHeader1.Width = 15;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.tvFiles);
            this.groupBox6.Controls.Add(this.pgFileInfo);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox6.Location = new System.Drawing.Point(0, 0);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(421, 286);
            this.groupBox6.TabIndex = 0;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Files";
            // 
            // pgFileInfo
            // 
            this.pgFileInfo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pgFileInfo.HelpVisible = false;
            this.pgFileInfo.Location = new System.Drawing.Point(3, 210);
            this.pgFileInfo.Margin = new System.Windows.Forms.Padding(1);
            this.pgFileInfo.Name = "pgFileInfo";
            this.pgFileInfo.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
            this.pgFileInfo.Size = new System.Drawing.Size(416, 73);
            this.pgFileInfo.TabIndex = 3;
            this.pgFileInfo.ToolbarVisible = false;
            // 
            // tvFiles
            // 
            this.tvFiles.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tvFiles.Location = new System.Drawing.Point(3, 16);
            this.tvFiles.Name = "tvFiles";
            this.tvFiles.Size = new System.Drawing.Size(416, 195);
            this.tvFiles.TabIndex = 4;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(825, 588);
            this.Controls.Add(this.splitContainer2);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "WiiDL Sample C# App";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
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
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.ListView lvPartitions;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.PropertyGrid pgPartInfo;
        private System.Windows.Forms.PropertyGrid pgFileInfo;
        private System.Windows.Forms.TreeView tvFiles;
    }
}

