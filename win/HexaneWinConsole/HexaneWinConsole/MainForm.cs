using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using HexaneWrapper;

namespace HexaneWinConsole
{
    public partial class MainForm : Form
    {
        private string Prompt = ">> ";
        private List<string> History;
        private int HistoryIndex;

        public MainForm()
        {
            InitializeComponent();

            this.History = new List<string>();

            textBox_console.AppendText(this.Prompt);
        }

        private void PassInputLineToHexaneEngine()
        {
            int line = textBox_console.GetLineFromCharIndex(textBox_console.SelectionStart);
            string linestr = textBox_console.Lines[line];
            linestr = linestr.TrimStart(this.Prompt.ToCharArray());

            if(String.IsNullOrEmpty(linestr))
            {
                return;
            }

            this.History.Add(linestr);
            this.HistoryIndex = this.History.Count - 1;
            
            String result = HexaneEngine.Run(linestr + "\n");

            result = result.Replace("\n", Environment.NewLine);

            textBox_console.AppendText(Environment.NewLine + result);
        }

        private string GetHistory(Keys keyCode)
        {
            if(this.History.Count == 0)
            {
                return null;
            }
            string historyInput = this.History[this.HistoryIndex];

            this.HistoryIndex = (keyCode == Keys.Up) ? (this.HistoryIndex - 1) : (this.HistoryIndex + 1);
            this.HistoryIndex = (this.HistoryIndex < 0) ? 0 : this.HistoryIndex;
            this.HistoryIndex = (this.HistoryIndex >= this.History.Count) ? (this.History.Count - 1) : this.HistoryIndex;

            return historyInput;
        }

        private void textBox_console_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.KeyCode == Keys.Enter)
            {
                PassInputLineToHexaneEngine();
                textBox_console.AppendText(Environment.NewLine + this.Prompt);
                e.SuppressKeyPress = true;
                e.Handled = true;
            }
            else if(e.KeyCode == Keys.Left || e.KeyCode == Keys.Back)
            {
                int line = textBox_console.GetLineFromCharIndex(textBox_console.SelectionStart);
                int column = textBox_console.SelectionStart - textBox_console.GetFirstCharIndexFromLine(line);

                if (column <= this.Prompt.Length)
                {
                    e.SuppressKeyPress = true;
                    e.Handled = true;
                }
            }
            else if(e.KeyCode == Keys.Home)
            {
                int line = textBox_console.GetLineFromCharIndex(textBox_console.SelectionStart);
                textBox_console.SelectionStart = textBox_console.GetFirstCharIndexFromLine(line) + this.Prompt.Length;
                e.SuppressKeyPress = true;
                e.Handled = true;
            }
            else if(e.KeyCode == Keys.Up || e.KeyCode == Keys.Down)
            {
                if (textBox_console.Text.LastIndexOf(Environment.NewLine) > 0)
                {
                    textBox_console.Text = textBox_console.Text.Remove(textBox_console.Text.LastIndexOf(Environment.NewLine));
                    textBox_console.SelectionStart = textBox_console.Text.Length;
                    textBox_console.AppendText(Environment.NewLine + this.Prompt);
                }

                string historyInput = GetHistory(e.KeyCode);
                if (historyInput != null)
                {
                    textBox_console.AppendText(historyInput);
                }

                e.SuppressKeyPress = true;
                e.Handled = true;
            }
            else if(e.KeyCode == Keys.PageUp || e.KeyCode == Keys.PageDown)
            {
                e.SuppressKeyPress = true;
                e.Handled = true;
            }
        }

        private void textBox_console_MouseClick(object sender, MouseEventArgs e)
        {
            if(this.textBox_console.SelectedText.Length == 0)
            {
                textBox_console.SelectionStart = textBox_console.Text.Length;
                return;
            }
            Clipboard.SetText(this.textBox_console.SelectedText);
            textBox_console.SelectionStart = textBox_console.Text.Length;
        }
    }
}
