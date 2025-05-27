#pragma once

#include "GraphicsDevice.h"
#include "CommandManager.h"
#include "DescriptorManager.h"

struct IRender 
{
	virtual bool Init(HWND windowHandle, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager) = 0;
	virtual void Resize(int w, int h) = 0;
	virtual void CreatePipeline() = 0;
	virtual void RecordCommands() = 0; 
};