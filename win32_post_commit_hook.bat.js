// this script is a local post-commit hook script.

var objArgs,num;

objArgs = WScript.Arguments;
num = objArgs.length;
if (num != 7)
{
    WScript.Echo("Usage: [CScript | WScript] PostCommit.js path/to/bat path/to/pathsfile depth path/to/messagefile revision error path/to/CWD ");
    WScript.Quit(1);
}

var repoPath = objArgs(0).substring( 0, objArgs(0).lastIndexOf('\\') );
var changelogPath = repoPath + "\\changelog.txt";
var svnTool = repoPath + "\\Toolset\\svn\\bin\\svn.exe";

var fs = new ActiveXObject("Scripting.FileSystemObject");

var errorFilePath = objArgs(5);
var errorFile = fs.GetFile( errorFilePath );
if( errorFile.Size == 0 )
{
	var changelogFile = fs.OpenTextFile( changelogPath, 8, true );
	var messageFilePath = objArgs(3);
	var message = readFile( messageFilePath );
	var revision = objArgs(4);
	date = new Date();
	changelogFile.WriteLine();
	changelogFile.WriteLine( "[" + date.toGMTString() + "] revision: " + revision );
	changelogFile.WriteLine( message );
	changelogFile.Close();
	
	var WshShell = new ActiveXObject("WScript.Shell");
	var execString = svnTool + " ci -m \"changelog update\" " + changelogPath;
	var oExec = WshShell.Exec( execString );
	while (oExec.Status == 0)
	{
		WScript.Sleep(100);
	}
	
	var changesFile = fs.OpenTextFile( repoPath + "\\build\\changes.txt", 8, true );
	changesFile.WriteLine( message );
	changesFile.Close();
}


WScript.Quit(0);

function readFile(path)
{
	var retString = "";
	var fs = new ActiveXObject("Scripting.FileSystemObject");
	if (fs.FileExists(path))
	{
		var a = fs.OpenTextFile(path, 1, false);
		while (!a.AtEndOfStream)
		{
			var line = a.ReadLine();
			retString += line;
		}
		a.Close();
	}
	return retString;
}

function readPaths(path)
{
	var retPaths = new Array();
	var fs = new ActiveXObject("Scripting.FileSystemObject");
	if (fs.FileExists(path))
	{
		var a = fs.OpenTextFile(path, 1, false);
		var i = 0;
		while (!a.AtEndOfStream)
		{
			var line = a.ReadLine();
			retPaths[i] = line;
			i = i + 1;
		}
		a.Close();
	}
	return retPaths;
	
}
