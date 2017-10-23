#include "DebugWindow.h"

#include "../Util.h"

void TW_CALL copyStringToClient(std::string& destination, const std::string& source)
{
	destination = source;
}

DebugWindow::DebugWindow(std::string windowID, std::string windowLabel)
{
#if defined(DEBUG) || defined(_DEBUG)
	m_windowID = windowID;
	m_windowLabel = windowLabel;
	m_window = TwNewBar(m_windowID.c_str());
	TwDefine((m_windowID + " label='" + m_windowLabel + "' iconified=true color='50 50 50' alpha=200 ").c_str());
#endif
}

DebugWindow::~DebugWindow()
{
	TwDeleteBar(m_window);
}

void DebugWindow::addVariableWithCallbacks(TwType varType, std::string varName, std::string componentName, std::string entityName, TwGetVarCallback getCallback, TwSetVarCallback setCallback, void* object, std::string additionalParams)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string varID = entityName + componentName + varName;
	std::string componentID = entityName + componentName;

	std::string spacelessVarID = removeSpacesFromString(varID);
	std::string spacelessComponentID = removeSpacesFromString(componentID);
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	TwAddVarCB(m_window, spacelessVarID.c_str(), varType, setCallback, getCallback, object, "");

	std::string description = " " + m_windowID + "/" + spacelessVarID + " group=" + spacelessComponentID + " label='" + varName + "' " + additionalParams;
	TwDefine(description.c_str());

	description = " " + m_windowID + "/" + spacelessComponentID + " group=" + spacelessEntityName + " label='" + componentName + "' opened=false ";
	TwDefine(description.c_str());
#endif
}

void DebugWindow::addButton(std::string buttonID, std::string buttonLabel, std::string group, TwButtonCallback callback, void* object, std::string additionalParams)
{
#if defined(DEBUG) || defined(_DEBUG)
	TwAddButton(m_window, buttonID.c_str(), callback, object, (" group=" + group + " label='" + buttonLabel + "' " + additionalParams).c_str());
#endif
}

void DebugWindow::addSeparator(std::string separatorID, std::string group, std::string additionalParams)
{
#if defined(DEBUG) || defined(_DEBUG)
	TwAddSeparator(m_window, separatorID.c_str(), (" group=" + group + " " + additionalParams).c_str());
#endif
}

std::string DebugWindow::getWindowID() const
{
	return m_windowID;
}

std::string DebugWindow::getWindowLabel() const
{
	return m_windowLabel;
}

TwBar* DebugWindow::getWindow() const
{
	return m_window;
}
