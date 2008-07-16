using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;

using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace Editor
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private int layerSizeCounter = 0;
        private int spriteSizeCounter = 0;
        private int hotspotSizeCounter = 0;
        Graphics graph = null;
        private BufferedGraphics bufferedGraphics = null;


        Scene scene = new Scene();

        private void sceneTreeView_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyValue == (char)Keys.Up && Control.ModifierKeys == Keys.Shift)
            {
                e.Handled = true;

                TreeNode prevNode = sceneTreeView.SelectedNode.PrevNode;

                if (prevNode != null)
                {
                    String prevNodeName = prevNode.Text;
                    String nodeName = sceneTreeView.SelectedNode.Text;

                    int index = prevNode.Index;
                    TreeNode copy = (TreeNode)prevNode.Clone();
                    prevNode.Remove();
                    sceneTreeView.Nodes.Insert(index + 1, copy);

                    scene.SwapLayers(prevNodeName, nodeName);
                    scene.Draw(graph,bufferedGraphics);
                }
            }

            if (e.KeyValue == (char)Keys.Down && Control.ModifierKeys == Keys.Shift)
            {
                e.Handled = true;

                TreeNode nextNode = sceneTreeView.SelectedNode.NextNode;

                if (nextNode != null)
                {
                    String prevNodeName = nextNode.Text;
                    String nodeName = sceneTreeView.SelectedNode.Text;

                    int index = nextNode.Index;
                    TreeNode copy = (TreeNode)nextNode.Clone();
                    nextNode.Remove();
                    sceneTreeView.Nodes.Insert(index - 1, copy);

                    scene.SwapLayers(prevNodeName, nodeName);
                    scene.Draw(graph,bufferedGraphics);
                }
            }            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            TreeNode node = sceneTreeView.SelectedNode;

            if (node != null)
            {
                int index = node.Index;
                String newItem = "layer" + layerSizeCounter;
                sceneTreeView.Nodes.Insert(index + 1, newItem);
                layerSizeCounter++;

                scene.AddLayer(newItem);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (sceneTreeView.Nodes.Count == 0)
            {
                sceneTreeView.Nodes.Insert(0, "MainLayer");
                sceneTreeView.Select();

                Layer layer = scene.AddLayer("MainLayer");
                layer.SetMain(true);
            }

            updateListBox("MainLayer");

            bufferedGraphics = BufferedGraphicsManager.Current.Allocate(
                pictureBox1.CreateGraphics(),
                new Rectangle(0, 0, 4096,
                4096));

            graph = bufferedGraphics.Graphics;
        }

        private void sceneTreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Node != null)
            {
                updateListBox(e.Node.Text);
            }
        }

        public void updateListBox(String layerName)
        {
            Layer layer = scene.GetLayer(layerName);

            if (layer != null)
            {
                layerSizeXTextBox.Text = layer.GetPosition().X.ToString();
                layerSizeYTextBox.Text = layer.GetPosition().Y.ToString();
                layerMainCheckBox.Checked = layer.GetMain();
                layerNameTextBox.Text = layerName;

                layerSizeXTextBox.Text = layer.GetSize().X.ToString();
                layerSizeYTextBox.Text = layer.GetSize().Y.ToString();

                scrollableCheckBox.Checked = layer.IsScroll();
                           
                nodesTreeView.Nodes.Clear();
                foreach (Node node in layer.Nodes)
                {
                    String name = node.getName();
                    nodesTreeView.Nodes.Add(name);
                }

                //nodesTreeView.Select();
                if (nodesTreeView.Nodes.Count != 0)
                    nodesTreeView.SelectedNode = nodesTreeView.Nodes[0];

                currentLayer = layer;

                if (nodesTreeView.SelectedNode != null)
                {
                    String NodeName = nodesTreeView.SelectedNode.Text;
                    Node selectedNode = currentLayer.GetNode(NodeName);
                    if (selectedNode == null) return;
                    spritePosXTextBox.Text = selectedNode.getPosX().ToString();
                    spritePosYTextBox.Text = selectedNode.getPosY().ToString();
                    nodeNameTextBox.Text = selectedNode.getName();


                    checkBox1.Checked = selectedNode.isAligned();

                }                
            }
        }

        public static string InputBox(string prompt, string title, string defaultValue)
        {
            InputBoxDialog ib = new InputBoxDialog();
            ib.FormPrompt = prompt;
            ib.FormCaption = title;
            ib.DefaultValue = defaultValue;
            ib.ShowDialog();
            string s = ib.InputResponse;
            ib.Close();
            return s;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            String SpriteName = "Sprite" + spriteSizeCounter.ToString();
            spriteSizeCounter++;

            String layerName = sceneTreeView.SelectedNode.Text;
            Layer layer = scene.GetLayer(layerName);

            Point point = new Point(panel1.HorizontalScroll.Value + panel1.Width / 2, panel1.VerticalScroll.Value + panel1.Height / 2);
            layer.AddSprite(SpriteName, null, point.X, point.Y);

            updateListBox(layerName);
            nodesTreeView.SelectedNode = nodesTreeView.Nodes[nodesTreeView.Nodes.Count-1]; 
        }

   
        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            scene.Draw(graph,bufferedGraphics);
        }

        private void sceneTreeView_KeyUp(object sender, KeyEventArgs e)
        {
            if(sceneTreeView.SelectedNode!=null)
            updateListBox(sceneTreeView.SelectedNode.Text);
        }

        private void loadResourceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ResourceManagerLoader loader = new ResourceManagerLoader(ref resourceManager);

            if(loader.execute())
            {
                resourceTreeView.Nodes.Clear();
                ResourceTreeViewer treeViewer = new ResourceTreeViewer(ref resourceManager, ref resourceTreeView);
            }
        }

        private ResourceManager resourceManager = new ResourceManager();

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (currentModifier != null)
                {
                    currentModifier.ProcessNode(e.X, e.Y);
                }
            }

            if (e.Button == MouseButtons.Right)
            {
                if (currentModifier != null)
                {
                    currentModifier.ProcessNode(e.X, e.Y);
                }
            }

            if (nodesTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;
                Node selectedNode = currentLayer.GetNode(NodeName);
                if (selectedNode == null) return;
                spritePosXTextBox.Text = selectedNode.getPosX().ToString();
                spritePosYTextBox.Text = selectedNode.getPosY().ToString();
                checkBox1.Checked = selectedNode.isAligned();
            }

            scene.Draw(graph,bufferedGraphics);
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (nodesTreeView.SelectedNode != null)
            {
                if (e.Button == MouseButtons.Left)
                {
                    String NodeName = nodesTreeView.SelectedNode.Text;
                    Node selectedNode = currentLayer.GetNode(NodeName);

                    currentModifier = new MotionModifier(ref selectedNode);
                    currentModifier.DownNode(e.X, e.Y);

                    this.Cursor = Cursors.Cross;
                }

                if (e.Button == MouseButtons.Right)
                {
                    String NodeName = nodesTreeView.SelectedNode.Text;
                    Node selectedNode = currentLayer.GetNode(NodeName);

                    if (selectedNode.isHotSpot() == false)
                    {
                        return;
                    }

                    HotSpot hotspot = (HotSpot)selectedNode;
                    /*
                    if (hotspot == null)
                    {
                        return;
                    }*/

                    if (Control.ModifierKeys == Keys.Control)
                    {
                        int eraseIndex = hotspot.SelectPoint(new Point(e.X, e.Y));

                        if (eraseIndex != -1)
                        {
                            hotspot.ErasePoint(eraseIndex);
                        }

                        return;
                    }

                    if (Control.ModifierKeys == Keys.Shift)
                    {
                        hotspot.InsertPoint(new Point(e.X,e.Y));
                        return;
                    }

                    int index = -1;

                    if (hotspot != null)
                    {
                        index = hotspot.SelectPoint(new Point(e.X, e.Y));
                    }

                    if (index != -1)
                    {
                        currentModifier = new PointModifier(ref hotspot, index);
                        currentModifier.DownNode(e.X, e.Y);

                        this.Cursor = Cursors.Cross;
                    }
                }
            }

        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (currentModifier != null)
            currentModifier.UpNode(e.X, e.Y);
            this.Cursor = Cursors.Arrow;
            scene.Draw(graph,bufferedGraphics);
        }

        private Modifier currentModifier = null;
        private Layer currentLayer = null;

        private void spritePosXTextBox_Leave(object sender, EventArgs e)
        {
            if (spritePosXTextBox.Text == "") return;
            if (spritePosYTextBox.Text == "") return;

            if (nodesTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;
                Node selectedNode = currentLayer.GetNode(NodeName);
                int X = Convert.ToInt32(spritePosXTextBox.Text);
                selectedNode.setPosX(X);
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void spritePosXTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                if (nodesTreeView.SelectedNode != null)
                {
                    String NodeName = nodesTreeView.SelectedNode.Text;
                    Node selectedNode = currentLayer.GetNode(NodeName);
                    if (selectedNode == null) return;
                    int X = Convert.ToInt32(spritePosXTextBox.Text);
                    selectedNode.setPosX(X);
                    scene.Draw(graph,bufferedGraphics);
                }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (nodesTreeView.SelectedNode != null && resourceTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;
                Node selectedNode = currentLayer.GetNode(NodeName);
                if (selectedNode == null) return;
                Sprite sprite = (Sprite)selectedNode;
                if (sprite != null)
                {
                    String resourceName = resourceTreeView.SelectedNode.Text;
                    ResourceImageDefault res = resourceManager.GetResource(resourceName);
                    sprite.setResource(ref res);
                    scene.Draw(graph,bufferedGraphics);
                }
            }
        }

        private void spriteIndexTextBox_KeyDown(object sender, KeyEventArgs e)
        {
                if (e.KeyCode == Keys.Enter)
                if (nodesTreeView.SelectedNode != null)
                {
                    String NodeName = nodesTreeView.SelectedNode.Text;
                    Node selectedNode = currentLayer.GetNode(NodeName);
                    if (selectedNode == null) return;

                    int index = Convert.ToInt32(spriteIndexTextBox.Text);

                    Sprite sprite = (Sprite)selectedNode;
                    if (sprite != null)
                    {
                        sprite.setImageIndex(index);
                    }
                    scene.Draw(graph,bufferedGraphics);
                }
        }

        private void spritePosYTextBox_Leave(object sender, EventArgs e)
        {
            if (spritePosXTextBox.Text == "") return;
            if (spritePosYTextBox.Text == "") return;

            if (nodesTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;
                Node selectedNode = currentLayer.GetNode(NodeName);
                if (selectedNode == null) return;

                int Y = Convert.ToInt32(spritePosYTextBox.Text);
                selectedNode.setPosY(Y);
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void spritePosYTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                if (nodesTreeView.SelectedNode != null)
                {
                    String NodeName = nodesTreeView.SelectedNode.Text;
                    Node selectedNode = currentLayer.GetNode(NodeName);
                    if (selectedNode == null) return;
                    int Y = Convert.ToInt32(spritePosYTextBox.Text);
                    selectedNode.setPosY(Y);
                    scene.Draw(graph,bufferedGraphics);
                }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            String HotspotName = "HotSpot" + hotspotSizeCounter.ToString();
            hotspotSizeCounter++;

            String layerName = sceneTreeView.SelectedNode.Text;
            Layer layer = scene.GetLayer(layerName); 

            Point point = new Point(panel1.HorizontalScroll.Value + panel1.Width / 2, panel1.VerticalScroll.Value + panel1.Height / 2);
            layer.AddHotSpot(HotspotName, point.X, point.Y);

            updateListBox(layerName);

            nodesTreeView.SelectedNode = nodesTreeView.Nodes[nodesTreeView.Nodes.Count - 1];
            scene.Draw(graph,bufferedGraphics);
        }

        private void saveSceneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            scene.Save();
            
            scene.Draw(graph,bufferedGraphics);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (nodesTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;

                DialogResult dr;
                dr = MessageBox.Show("Are you sure you wish to delete " + NodeName + "?",
                                "Confirmation",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Question);

                if (dr == DialogResult.No)
                {
                    return;
                }

                currentLayer.RemoveNode(NodeName);
                nodesTreeView.SelectedNode.Remove();
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void nodeNameTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if (nodesTreeView.SelectedNode != null)
                {
                    String NeededName = nodeNameTextBox.Text;

                    String NodeName = nodesTreeView.SelectedNode.Text;
                    Node selectedNode = currentLayer.GetNode(NodeName);
                    if (selectedNode == null) return;

                    if (currentLayer.GetNode(NeededName) != null) return;

                    nodesTreeView.SelectedNode.Text = NeededName;
                    selectedNode.setName(NeededName);
                    scene.Draw(graph,bufferedGraphics);
                }
            }
        }

        private void nodesTreeView_KeyUp(object sender, KeyEventArgs e)
        {
            if (nodesTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;
                Node selectedNode = currentLayer.GetNode(NodeName);
                if (selectedNode == null) return;
                spritePosXTextBox.Text = selectedNode.getPosX().ToString();
                spritePosYTextBox.Text = selectedNode.getPosY().ToString();
                nodeNameTextBox.Text = selectedNode.getName();
                checkBox1.Checked = selectedNode.isAligned();
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void nodesTreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            nodesTreeView.SelectedNode = e.Node;
            if (nodesTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;
                Node selectedNode = currentLayer.GetNode(NodeName);
                if (selectedNode == null) return;
                spritePosXTextBox.Text = selectedNode.getPosX().ToString();
                spritePosYTextBox.Text = selectedNode.getPosY().ToString();
                nodeNameTextBox.Text = selectedNode.getName();
                checkBox1.Checked = selectedNode.isAligned();
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void layerNameTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if (sceneTreeView.SelectedNode != null)
                {
                    String NeededName = layerNameTextBox.Text;

                    String NodeName = sceneTreeView.SelectedNode.Text;
                    Layer layer = scene.GetLayer(NodeName);

                    if (layer == null) return;

                    Layer layer1 = scene.GetLayer(NeededName);

                    if (layer1 != null) return;

                    sceneTreeView.SelectedNode.Text = NeededName;
                    layer.SetName(NeededName);
                    scene.Draw(graph,bufferedGraphics);
                }
            }

        }

        private void layerSizeXTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if (sceneTreeView.SelectedNode != null)
                {
                    String NeededName = layerNameTextBox.Text;

                    String NodeName = sceneTreeView.SelectedNode.Text;
                    Layer layer = scene.GetLayer(NodeName);

                    if (layer == null) return;

                    layer.SetSizeX(
                        Convert.ToInt32(layerSizeXTextBox.Text));
                    scene.Draw(graph,bufferedGraphics);
                }
            }
        }

        private void layerSizeYTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if (sceneTreeView.SelectedNode != null)
                {
                    String NeededName = layerNameTextBox.Text;

                    String NodeName = sceneTreeView.SelectedNode.Text;
                    Layer layer = scene.GetLayer(NodeName);

                    if (layer == null) return;

                    layer.SetSizeY(
                        Convert.ToInt32(layerSizeYTextBox.Text));
                    scene.Draw(graph,bufferedGraphics);
                }
            }
        }

        private void layerSizeXTextBox_Leave(object sender, EventArgs e)
        {
            if (sceneTreeView.SelectedNode != null)
            {
                String NeededName = layerNameTextBox.Text;

                String NodeName = sceneTreeView.SelectedNode.Text;
                Layer layer = scene.GetLayer(NodeName);

                if (layer == null) return;

                layer.SetSizeX(
                    Convert.ToInt32(layerSizeXTextBox.Text));
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void layerSizeYTextBox_Leave(object sender, EventArgs e)
        {
            if (sceneTreeView.SelectedNode != null)
            {
                String NeededName = layerNameTextBox.Text;

                String NodeName = sceneTreeView.SelectedNode.Text;
                Layer layer = scene.GetLayer(NodeName);

                if (layer == null) return;

                layer.SetSizeY(
                    Convert.ToInt32(layerSizeYTextBox.Text));
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                pictureBox1.Width = Convert.ToInt32(textBox1.Text);
                bufferedGraphics = BufferedGraphicsManager.Current.Allocate(
                pictureBox1.CreateGraphics(),
                new Rectangle(0, 0, pictureBox1.Width,
                pictureBox1.Height));
                
                graph = bufferedGraphics.Graphics;
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void textBox2_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                pictureBox1.Height = Convert.ToInt32(textBox2.Text);
                bufferedGraphics = BufferedGraphicsManager.Current.Allocate(
                pictureBox1.CreateGraphics(),
                new Rectangle(0, 0, pictureBox1.Width,
                pictureBox1.Height));
                
                graph = bufferedGraphics.Graphics;
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            TreeNode node = sceneTreeView.SelectedNode;

            if (node != null)
            {
                String NodeName = node.Text;

                DialogResult dr;
                dr = MessageBox.Show("Are you sure you wish to delete " + NodeName + "?",
                                "Confirmation",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Question);

                if (dr == DialogResult.No)
                {
                    return;
                }

                Layer item = scene.GetLayer(node.Text);

                if (item != null)
                {
                    scene.Remove(item);
                    node.Remove();
                }
                scene.Draw(graph,bufferedGraphics);
            }
        }

        private void loadSceneToolStripMenuItem_Click(object sender, EventArgs e)
        {
            scene.Load(ref resourceManager);

            sceneTreeView.Nodes.Clear();

            scene.FillTreeView(ref sceneTreeView);

            if (sceneTreeView.Nodes.Count > 0)
            {
                sceneTreeView.SelectedNode = sceneTreeView.Nodes[0];
            }

            sceneTreeView.Select();

            if (sceneTreeView.Nodes.Count > 0)
            {
                updateListBox(sceneTreeView.SelectedNode.Text);
            }

            scene.Draw(graph,bufferedGraphics);
        }

        private void scrollableCheckBox_CheckedChanged(object sender, EventArgs e)
        {
                if (sceneTreeView.SelectedNode != null)
                {
                    String NeededName = layerNameTextBox.Text;

                    String NodeName = sceneTreeView.SelectedNode.Text;
                    Layer layer = scene.GetLayer(NodeName);

                    if (layer == null) return;

                    sceneTreeView.SelectedNode.Text = NeededName;
                    layer.SetScroll(scrollableCheckBox.Checked);
                    scene.Draw(graph,bufferedGraphics);
                }
            
        }

        private void nodesTreeView_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyValue == (char)Keys.Up && Control.ModifierKeys == Keys.Shift)
            {
                e.Handled = true;

                TreeNode prevNode = nodesTreeView.SelectedNode.PrevNode;

                if (prevNode != null)
                {
                    String prevNodeName = prevNode.Text;
                    String nodeName = nodesTreeView.SelectedNode.Text;

                    int index = prevNode.Index;
                    TreeNode copy = (TreeNode)prevNode.Clone();
                    prevNode.Remove();
                    nodesTreeView.Nodes.Insert(index + 1, copy);
                    currentLayer.SwapNodes(prevNodeName, nodeName);
                    scene.Draw(graph,bufferedGraphics);
                }
            }

            if (e.KeyValue == (char)Keys.Down && Control.ModifierKeys == Keys.Shift)
            {
                e.Handled = true;

                TreeNode nextNode = nodesTreeView.SelectedNode.NextNode;

                if (nextNode != null)
                {
                    String prevNodeName = nextNode.Text;
                    String nodeName = nodesTreeView.SelectedNode.Text;

                    int index = nextNode.Index;
                    TreeNode copy = (TreeNode)nextNode.Clone();
                    nextNode.Remove();
                    nodesTreeView.Nodes.Insert(index - 1, copy);
                    currentLayer.SwapNodes(prevNodeName, nodeName);
                    scene.Draw(graph,bufferedGraphics);
                }
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (nodesTreeView.SelectedNode != null)
            {
                String NodeName = nodesTreeView.SelectedNode.Text;
                Node selectedNode = currentLayer.GetNode(NodeName);
                if (selectedNode == null) return;

                int index = Convert.ToInt32(spriteIndexTextBox.Text);

                Sprite sprite = (Sprite)selectedNode;
                if (sprite != null)
                {
                    
                    sprite.setCenterAlign(checkBox1.Checked);
                }
                scene.Draw(graph,bufferedGraphics);
            }
        }
    }
}