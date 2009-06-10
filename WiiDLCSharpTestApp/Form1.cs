using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WiiDLCSharpTestApp
{
    public partial class Form1 : Form
    {
        

        public Form1()
        {
            InitializeComponent();
        }

        private void btnTest_Click(object sender, EventArgs e)
        {
            WiiDLManagedWrapper.ManagedDisc d = new WiiDLManagedWrapper.ManagedDisc(@"C:\test.iso.RVM");
            

        }
    }
}
