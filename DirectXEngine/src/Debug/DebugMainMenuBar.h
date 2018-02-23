#pragma once
#include "IDebugEditor.h"

#include "DebugEntityList.h"
#include "DebugComponentList.h"
#include "DebugAssetList.h"

enum ClosedState
{
	NOT_CLOSED,
	TRYING_TO_CLOSE,
	CLOSED
};

class DebugMainMenuBar : public IDebugEditor
{
public:
	DebugMainMenuBar(DebugEntityList& entityList, DebugComponentList& componentList, DebugAssetList& assetList);
	~DebugMainMenuBar();

	float getHeight() const;

	bool shouldDrawEntityList() const;
	bool shouldDrawComponentList() const;
	bool shouldDrawAssetList() const;
	bool shouldDrawConsole() const;

	bool chooseNew();
	bool chooseLoad();
	bool chooseQuit();

	void openNewSaveChangesPopup();
	void openLoadSaveChangesPopup();
	void openQuitSaveChangesPopup();

	ClosedState getAppClosedState() const;
	void setAppClosedState(ClosedState state);

	void draw() override;

private:
	void handleNewSaveChanges();
	void handleLoadSaveChanges();
	void handleQuitSaveChanges();

	DebugEntityList& m_entityList;
	DebugComponentList& m_componentList;
	DebugAssetList& m_assetList;

	bool m_drawEntityList;
	bool m_drawComponentList;
	bool m_drawAssetList;
	bool m_drawConsole;

	float m_height;

	ClosedState m_closedState;
};