using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Text;
using System.Reflection;

namespace ExportDll
{
    enum ParserState
    {
        Normal,
        ClassDeclaration,
        Class,
        DeleteExportDependency,
        MethodDeclaration,
        MethodProperties,
        Method,
        DeleteExportAttribute,
    }
    class Program
    {
        static bool FindWithAttribute(MemberInfo mi, object obj)
        {
            object[] attrs = mi.GetCustomAttributes(typeof(ExportDllAttribute.ExportDllAttribute), false);
            if (attrs.Length > 0)
                return true;
            return false;
        }

        static int Main(string[] args)
        {
            try
            {
                if (args.Length < 1)
                {
                    Console.WriteLine("Parameter error!");
                    return 1;
                }
                bool debug = false;
                
                //Console.WriteLine("Args have {0} elements:", args.Length.ToString());
                //foreach(string arg in args)
                //    Console.WriteLine("\t'{0}'", arg);
                if (args.Length > 1)
                {
                    if (args[1] == "/Debug")
                        debug = true;
                }
                Console.WriteLine("Debug: {0}", debug);
                string filepath = args[0];
                string path = System.IO.Path.GetDirectoryName(filepath);
                if (path == string.Empty)
                {
                    Console.WriteLine("Full path needed!");
                    return 1;
                }
                string ext = System.IO.Path.GetExtension(filepath);
                if (ext != ".dll")
                {
                    Console.WriteLine("Target should be dll!");
                    return 1;
                }
                int exportscount = 0;
                Dictionary<string, Dictionary<string, KeyValuePair<string, string>>> dic = new Dictionary<string, Dictionary<string, KeyValuePair<string, string>>>();
                byte[] rawassembly;
                using (System.IO.FileStream fs = System.IO.File.OpenRead(filepath))
                {
                    System.IO.BinaryReader br = new System.IO.BinaryReader(fs);
                    rawassembly = br.ReadBytes((int)fs.Length);
                    br.Close();
                }
                Assembly assembly = Assembly.Load(rawassembly);
                Type[] types = assembly.GetTypes();
                foreach (Type type in types)
                {
                    MemberInfo[] mis = type.FindMembers(MemberTypes.All, BindingFlags.Public | BindingFlags.Static, new MemberFilter(FindWithAttribute), null);
                    {
                        foreach (MemberInfo mi in mis)
                        {
                            object[] attrs = mi.GetCustomAttributes(typeof(ExportDllAttribute.ExportDllAttribute), false);
                            if (attrs.Length > 0)
                            {
                                ExportDllAttribute.ExportDllAttribute attr = attrs[0] as ExportDllAttribute.ExportDllAttribute;
                                if(!dic.ContainsKey(type.FullName))
                                    dic[type.FullName] = new Dictionary<string, KeyValuePair<string, string>>();
                                dic[type.FullName][mi.Name] = new KeyValuePair<string, string>(attr.ExportName, attr.CallingConvention);
                                exportscount++;
                            }
                        }
                    }
                }
                if (exportscount > 0)
                {
                    int exportpos = 1;
                    string filename = System.IO.Path.GetFileNameWithoutExtension(filepath);
                    System.IO.Directory.SetCurrentDirectory(path);
                    Process proc = new Process();
                    string arguments = string.Format("/nobar{1}/out:{0}.il {0}.dll", filename, debug ? " /linenum " : " ");
                    Console.WriteLine("Deassebly file with arguments '{0}'", arguments);
                    System.Diagnostics.ProcessStartInfo info = new ProcessStartInfo(Properties.Settings.Default.ildasmpath, arguments);
                    info.UseShellExecute = false;
                    info.CreateNoWindow = false;
                    info.RedirectStandardOutput = true;
                    proc.StartInfo = info;
                    proc.Start();
                    proc.WaitForExit();
                    Console.WriteLine(proc.StandardOutput.ReadToEnd());
                    if (proc.ExitCode != 0)
                        return proc.ExitCode;
                    List<string> wholeilfile = new List<string>();
                    System.IO.StreamReader sr = System.IO.File.OpenText(System.IO.Path.Combine(path, filename + ".il"));
                    string methoddeclaration = "";
                    string methodname = "";
                    string classdeclaration = "";
                    string methodbefore = "";
                    string methodafter = "";
                    int methodpos = 0;
                    Stack<string> classnames = new Stack<string>();
                    List<string> externassembly = new List<string>();
                    ParserState state = ParserState.Normal;
                    while (!sr.EndOfStream)
                    {
                        string line = sr.ReadLine();
                        string trimedline = line.Trim();
                        bool addilne = true;
                        switch (state)
                        {
                            case ParserState.Normal:
                                if (trimedline.StartsWith(".corflags"))
                                {
                                    wholeilfile.Add(".corflags 0x00000002");
                                    wholeilfile.Add(string.Format(".vtfixup [{0}] int32 fromunmanaged at VT_01", exportscount));
                                    wholeilfile.Add(string.Format(".data VT_01 = int32[{0}]", exportscount));
                                    Console.WriteLine("Adding vtfixup.");
                                    addilne = false;
                                }
                                else if (trimedline.StartsWith(".class"))
                                {
                                    state = ParserState.ClassDeclaration;
                                    addilne = false;
                                    classdeclaration = trimedline;
                                }
                                else if (trimedline.StartsWith(".assembly extern ExportDllAttribute"))
                                {
                                    addilne = false;
                                    state = ParserState.DeleteExportDependency;
                                    Console.WriteLine("Deleting ExportDllAttribute dependency.");
                                }
                                break;
                            case ParserState.DeleteExportDependency:
                                if (trimedline.StartsWith("}"))
                                {
                                    state = ParserState.Normal;
                                }
                                addilne = false;
                                break;
                            case ParserState.ClassDeclaration:
                                if (trimedline.StartsWith("{"))
                                {
                                    state = ParserState.Class;
                                    string classname = "";
                                    System.Text.RegularExpressions.Regex reg = new System.Text.RegularExpressions.Regex(@".+\s+([^\s]+) extends \[.*");
                                    System.Text.RegularExpressions.Match m = reg.Match(classdeclaration);
                                    if (m.Groups.Count > 1)
                                        classname = m.Groups[1].Value;
                                    classname = classname.Replace("'", "");
                                    if (classnames.Count > 0)
                                        classname = classnames.Peek() + "+" + classname;
                                    classnames.Push(classname);
                                    Console.WriteLine("Found class: " + classname);
                                    wholeilfile.Add(classdeclaration);
                                }
                                else
                                {
                                    classdeclaration += " " + trimedline;
                                    addilne = false;
                                }
                                break;
                            case ParserState.Class:
                                if (trimedline.StartsWith(".class"))
                                {
                                    state = ParserState.ClassDeclaration;
                                    addilne = false;
                                    classdeclaration = trimedline;
                                }
                                else if (trimedline.StartsWith(".method"))
                                {
                                    if (dic.ContainsKey(classnames.Peek()))
                                    {
                                        methoddeclaration = trimedline;
                                        addilne = false;
                                        state = ParserState.MethodDeclaration;
                                    }
                                }
                                else if (trimedline.StartsWith("} // end of class"))
                                {
                                    classnames.Pop();
                                    if (classnames.Count > 0)
                                        state = ParserState.Class;
                                    else
                                        state = ParserState.Normal;
                                }
                                break;
                            case ParserState.MethodDeclaration:
                                if (trimedline.StartsWith("{"))
                                {
                                    System.Text.RegularExpressions.Regex reg = new System.Text.RegularExpressions.Regex(@"(?<before>[^\(]+\s+)(?<method>[^\(]+)(?<after>\(.*)");
                                    System.Text.RegularExpressions.Match m = reg.Match(methoddeclaration);
                                    if (m.Groups.Count > 3)
                                    {
                                        methodbefore = m.Groups["before"].Value;
                                        methodafter = m.Groups["after"].Value;
                                        methodname = m.Groups["method"].Value;
                                    }
                                    Console.WriteLine("Found method: " + methodname);
                                    if (dic[classnames.Peek()].ContainsKey(methodname))
                                    {
                                        methodpos = wholeilfile.Count;
                                        state = ParserState.MethodProperties;
                                    }
                                    else
                                    {
                                        wholeilfile.Add(methoddeclaration);
                                        state = ParserState.Method;
                                        methodpos = 0;
                                    }
                                }
                                else
                                {
                                    methoddeclaration += " " + trimedline;
                                    addilne = false;
                                }
                                break;
                            case ParserState.Method:
                                if (trimedline.StartsWith("} // end of method"))
                                {
                                    state = ParserState.Class;
                                }
                                break;
                            case ParserState.MethodProperties:
                                if (trimedline.StartsWith(".custom instance void [ExportDllAttribute]"))
                                {
                                    addilne = false;
                                    state = ParserState.DeleteExportAttribute;
                                }
                                else if (trimedline.StartsWith("// Code"))
                                {
                                    state = ParserState.Method;
                                    if (methodpos != 0)
                                        wholeilfile.Insert(methodpos, methoddeclaration);
                                }
                                break;
                            case ParserState.DeleteExportAttribute:
                                if (trimedline.StartsWith(".custum") || trimedline.StartsWith("// Code"))
                                {
                                    KeyValuePair<string, string> attr = dic[classnames.Peek()][methodname];
                                    methoddeclaration = methodbefore + "modopt([mscorlib]" + attr.Value + ") " + methodname + methodafter;
                                    Console.WriteLine("\tChanging calling convention: " + attr.Value);
                                    if (methodpos != 0)
                                        wholeilfile.Insert(methodpos, methoddeclaration);
                                    wholeilfile.Add(".vtentry 1 : " + exportpos.ToString());
                                    wholeilfile.Add(string.Format(".export [{0}] as {1}", exportpos, dic[classnames.Peek()][methodname].Key));
                                    Console.WriteLine("\tAdding .vtentry:{0} .export:{1}", exportpos, dic[classnames.Peek()][methodname].Key);
                                    exportpos++;
                                    state = ParserState.Method;
                                }
                                else
                                    addilne = false;
                                break;
                        }
                        if (addilne)
                            wholeilfile.Add(line);
                    }
                    sr.Close();
                    System.IO.StreamWriter sw = System.IO.File.CreateText(System.IO.Path.Combine(path, filename + ".il"));
                    foreach (string line in wholeilfile)
                    {
                        sw.WriteLine(line);
                    }
                    sw.Close();
                    string res = filename + ".res";
                    if (System.IO.File.Exists(filename + ".res"))
                        res = " /resource=" + res;
                    else
                        res = "";
                    proc = new Process();
                    arguments = string.Format("/nologo /quiet /out:{0}.dll {0}.il /DLL{1} {2}", filename, res, debug ? "/debug" : "/optimize");
                    Console.WriteLine("Compiling file with arguments '{0}'", arguments);
                    info = new ProcessStartInfo(Properties.Settings.Default.ilasmpath, arguments);
                    info.UseShellExecute = false;
                    info.CreateNoWindow = false;
                    info.RedirectStandardOutput = true;
                    proc.StartInfo = info;
                    proc.Start();
                    proc.WaitForExit();
                    Console.WriteLine(proc.StandardOutput.ReadToEnd());
                    if (proc.ExitCode != 0)
                        return proc.ExitCode;
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
                return -1;
            }
            return 0;
        }
    }
}
