#include "Util.h"

#include "Debug/Debug.h"
#include "Scene/SceneManager.h"

#include <shobjidl.h> 

// Taken from http://www.cplusplus.com/forum/windows/169960/ for the file dialog
// and https://stackoverflow.com/questions/875249/how-to-get-current-directory for getting .exe directory
std::string Util::loadFileDialog(HWND hWnd, const char* fileTypeFilter)
{
	char filename[MAX_PATH] = {};

	OPENFILENAME ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = fileTypeFilter;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = filename;
	ofn.lpstrTitle = "Load scene";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	char currentPath[MAX_PATH];
	GetModuleFileName(NULL, currentPath, MAX_PATH);

	std::string currentPathStr = std::string(currentPath);
	std::string::size_type pos = currentPathStr.find_last_of("\\/");
	std::string currentDirStr = currentPathStr.substr(0, pos) + "\\Scenes";

	ofn.lpstrInitialDir = currentDirStr.c_str();

	if (!GetOpenFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE: Debug::warning("Failed to load open file dialog: CDERR_DIALOGFAILURE");   break;
		case CDERR_FINDRESFAILURE: Debug::warning("Failed to load open file dialog: CDERR_FINDRESFAILURE");  break;
		case CDERR_INITIALIZATION: Debug::warning("Failed to load open file dialog: CDERR_INITIALIZATION");  break;
		case CDERR_LOADRESFAILURE: Debug::warning("Failed to load open file dialog: CDERR_LOADRESFAILURE");  break;
		case CDERR_LOADSTRFAILURE: Debug::warning("Failed to load open file dialog: CDERR_LOADSTRFAILURE");  break;
		case CDERR_LOCKRESFAILURE: Debug::warning("Failed to load open file dialog: CDERR_LOCKRESFAILURE");  break;
		case CDERR_MEMALLOCFAILURE: Debug::warning("Failed to load open file dialog: CDERR_MEMALLOCFAILURE"); break;
		case CDERR_MEMLOCKFAILURE: Debug::warning("Failed to load open file dialog: CDERR_MEMLOCKFAILURE");  break;
		case CDERR_NOHINSTANCE: Debug::warning("Failed to load open file dialog: CDERR_NOHINSTANCE");     break;
		case CDERR_NOHOOK: Debug::warning("Failed to load open file dialog: CDERR_NOHOOK");          break;
		case CDERR_NOTEMPLATE: Debug::warning("Failed to load open file dialog: CDERR_NOTEMPLATE");      break;
		case CDERR_STRUCTSIZE: Debug::warning("Failed to load open file dialog: CDERR_STRUCTSIZE");      break;
		case FNERR_BUFFERTOOSMALL: Debug::warning("Failed to load open file dialog: FNERR_BUFFERTOOSMALL");  break;
		case FNERR_INVALIDFILENAME: Debug::warning("Failed to load open file dialog: FNERR_INVALIDFILENAME"); break;
		case FNERR_SUBCLASSFAILURE: Debug::warning("Failed to load open file dialog: FNERR_SUBCLASSFAILURE"); break;
		default: break;
		}
	}

	return filename;
}

std::string Util::saveFileDialog(HWND hWnd, const char* fileTypeFilter)
{
	char filename[MAX_PATH] = {};

	OPENFILENAME ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = fileTypeFilter;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = filename;
	ofn.lpstrTitle = "Save scene";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	char currentPath[MAX_PATH];
	GetModuleFileName(NULL, currentPath, MAX_PATH);

	std::string currentPathStr = std::string(currentPath);
	std::string::size_type pos = currentPathStr.find_last_of("\\/");
	std::string currentDirStr = currentPathStr.substr(0, pos) + "\\Scenes";

	ofn.lpstrInitialDir = currentDirStr.c_str();

	if (!GetSaveFileName(&ofn))
	{
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE: Debug::warning("Failed to load open file dialog: CDERR_DIALOGFAILURE");   break;
		case CDERR_FINDRESFAILURE: Debug::warning("Failed to load open file dialog: CDERR_FINDRESFAILURE");  break;
		case CDERR_INITIALIZATION: Debug::warning("Failed to load open file dialog: CDERR_INITIALIZATION");  break;
		case CDERR_LOADRESFAILURE: Debug::warning("Failed to load open file dialog: CDERR_LOADRESFAILURE");  break;
		case CDERR_LOADSTRFAILURE: Debug::warning("Failed to load open file dialog: CDERR_LOADSTRFAILURE");  break;
		case CDERR_LOCKRESFAILURE: Debug::warning("Failed to load open file dialog: CDERR_LOCKRESFAILURE");  break;
		case CDERR_MEMALLOCFAILURE: Debug::warning("Failed to load open file dialog: CDERR_MEMALLOCFAILURE"); break;
		case CDERR_MEMLOCKFAILURE: Debug::warning("Failed to load open file dialog: CDERR_MEMLOCKFAILURE");  break;
		case CDERR_NOHINSTANCE: Debug::warning("Failed to load open file dialog: CDERR_NOHINSTANCE");     break;
		case CDERR_NOHOOK: Debug::warning("Failed to load open file dialog: CDERR_NOHOOK");          break;
		case CDERR_NOTEMPLATE: Debug::warning("Failed to load open file dialog: CDERR_NOTEMPLATE");      break;
		case CDERR_STRUCTSIZE: Debug::warning("Failed to load open file dialog: CDERR_STRUCTSIZE");      break;
		case FNERR_BUFFERTOOSMALL: Debug::warning("Failed to load open file dialog: FNERR_BUFFERTOOSMALL");  break;
		case FNERR_INVALIDFILENAME: Debug::warning("Failed to load open file dialog: FNERR_INVALIDFILENAME"); break;
		case FNERR_SUBCLASSFAILURE: Debug::warning("Failed to load open file dialog: FNERR_SUBCLASSFAILURE"); break;
		default: break;
		}
	}

	return filename;
}
