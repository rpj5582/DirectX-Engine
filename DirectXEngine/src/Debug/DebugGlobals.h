#pragma once

#include "AntTweakBar/AntTweakBar.h"

void TW_CALL copyStringToClient(std::string& destination, const std::string& source);

void TW_CALL getEntityEnabledDebugEditor(void* value, void* clientData);

void TW_CALL setEntityEnabledDebugEditor(const void* value, void* clientData);

void TW_CALL addChildEntityDebugEditor(void* clientData);

void TW_CALL removeChildEntityDebugEditor(void* clientData);

void TW_CALL addEntityDebugEditor(void* clientData);

void TW_CALL removeEntityDebugEditor(void* clientData);

void TW_CALL addComponentDebugEditor(void* clientData);

void TW_CALL removeComponentDebugEditor(void* clientData);

void TW_CALL addTextureDebugEditor(void* clientData);

void TW_CALL addMaterialDebugEditor(void* clientData);

void TW_CALL addMeshDebugEditor(void* clientData);

void TW_CALL addFontDebugEditor(void* clientData);

void TW_CALL addSamplerDebugEditor(void* clientData);

void TW_CALL addVertexShaderDebugEditor(void* clientData);

void TW_CALL addPixelShaderDebugEditor(void* clientData);

void TW_CALL removeTextureDebugEditor(void* clientData);

void TW_CALL removeMaterialDebugEditor(void* clientData);

void TW_CALL removeMeshDebugEditor(void* clientData);

void TW_CALL removeFontDebugEditor(void* clientData);

void TW_CALL removeSamplerDebugEditor(void* clientData);

void TW_CALL removeVertexShaderDebugEditor(void* clientData);

void TW_CALL removePixelShaderDebugEditor(void* clientData);

void TW_CALL newSceneDebugEditor(void* clientData);

void TW_CALL loadSceneDebugEditor(void* clientData);

void TW_CALL saveSceneDebugEditor(void* clientData);

void TW_CALL getMainCameraSceneDebugEditor(void* value, void* clientData);

void TW_CALL setMainCameraSceneDebugEditor(const void* value, void* clientData);
