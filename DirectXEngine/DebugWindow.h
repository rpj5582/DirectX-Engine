#pragma once

#include "AntTweakBar\AntTweakBar.h"

#include <string>

class DebugWindow
{
public:
	DebugWindow(std::string windowID, std::string windowLabel);
	virtual ~DebugWindow();

	template<typename T>
	void addVariable(T* var, TwType varType, std::string varName, std::string componentName, std::string entityName, std::string additionalParams = "", bool readonly = false);
	void addVariableWithCallbacks(TwType varType, std::string varName, std::string componentName, std::string entityName, TwGetVarCallback getCallback, TwSetVarCallback setCallback, void* object, std::string additionalParams = "");
	void addButton(std::string buttonID, std::string buttonLabel, std::string group, TwButtonCallback callback, void* object, std::string additionalParams = "");
	void addSeparator(std::string separatorID, std::string group, std::string additionalParams = "");

	std::string getWindowID() const;
	std::string getWindowLabel() const;
	TwBar* getWindow() const;

protected:
	std::string m_windowID;
	std::string m_windowLabel;

	TwBar* m_window;
};

template<typename T>
inline void DebugWindow::addVariable(T* var, TwType varType, std::string varName, std::string componentName, std::string entityName, std::string additionalParams, bool readonly)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string varID = entityName + componentName + varName;
	std::string componentID = entityName + componentName;

	std::string spacelessVarID = removeSpacesFromString(varID);
	std::string spacelessComponentID = removeSpacesFromString(componentID);
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	if (readonly)
	{
		TwAddVarRO(m_window, spacelessVarID.c_str(), varType, var, "");

	}
	else
	{
		int success = TwAddVarRW(m_window, spacelessVarID.c_str(), varType, var, "");
		if (!success)
		{
			Debug::warning(TwGetLastError());
		}
	}

	std::string description = " " + m_windowID + "/" + spacelessVarID + " group=" + spacelessComponentID + " label='" + varName + "'" + additionalParams;
	TwDefine(description.c_str());

	description = " " + m_windowID + "/" + spacelessComponentID + " group=" + spacelessEntityName + " label='" + componentName + "' opened=false ";
	TwDefine(description.c_str());
#endif
}

void TW_CALL copyStringToClient(std::string& destination, const std::string& source);
