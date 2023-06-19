#pragma once
#ifndef IRRSETUP_H
#define IRRSETUP_H

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern IrrlichtDevice* device;
class BaedsLights;
extern BaedsLights* lmgr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#endif