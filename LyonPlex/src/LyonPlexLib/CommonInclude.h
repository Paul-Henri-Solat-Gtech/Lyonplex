#pragma once

// THIS IS FOR BOTH PCH -> LIB & APP

//First Priority
#include <iostream>
#include <string>
#include <windows.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <stdexcept>
#include <vector>
#include <unordered_map>

//Lib DX12
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

//Namespace to shortcut variables
using namespace Microsoft::WRL;
using namespace DirectX;

//Second Priority
#include "d3dx12.h"

//CLASS OF LYONPLEX
#include "WindowPlex.h"
#include "GameManager.h"

