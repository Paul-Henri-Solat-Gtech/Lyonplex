#pragma once

//Include for both pch
#include "CommonInclude.h"

//Don't touch (only for sampleProject)
#include "resource.h"
#include "AllScenes.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DEBUG_NEW
#endif
