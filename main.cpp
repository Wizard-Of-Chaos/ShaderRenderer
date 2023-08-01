#include "shaders.h"
#include <iostream>

IrrlichtDevice* device = 0;
ISceneManager* smgr = 0;
IVideoDriver* driver = 0;
BaedsLights* lmgr = 0;

void setLight(SColorf col, vector3df rot, f32 speed)
{
	auto light = smgr->addLightSceneNode(0, vector3df(0,0,0), col);
	SLight& data = light->getLightData();
	data.Radius = 210.f;
	data.AmbientColor.a = 0.f;
	auto anim = smgr->createFlyCircleAnimator(vector3df(0, 0, 0), 200.f, speed, rot);
	light->addAnimator(anim);
	anim->drop();

	auto bill = smgr->addBillboardSceneNode(light, dimension2df(20.f, 20.f));
	bill->setMaterialTexture(0, driver->getTexture("media/particlewhite.bmp"));
	bill->setMaterialFlag(EMF_LIGHTING, false);
	bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
}

int main()
{
	device = createDevice(EDT_DIRECT3D9, dimension2du(1440, 810));
	if (!device) return 1;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	path media = "media/";
	path shader = "shaders/";

	auto gpu = driver->getGPUProgrammingServices();
	auto cb = new basicNormalsCb();

	lmgr = new BaedsLights();
	smgr->setLightManager(lmgr);

	s32 shaderMaterial = 0;

	E_MATERIAL_TYPE baseType = EMT_SOLID;
	stringc vertex = "";
	stringc pixel = "";
	bool isLight = true;
	bool useShader = true;

	IrrXMLReader* xml = createIrrXMLReader("config.xml");
	if (!xml) {
		device->getLogger()->log(L"Config file missing, replacing with defaults");
		auto out = device->getFileSystem()->createXMLWriter("config.xml");
		out->writeXMLHeader();
		out->writeElement(L"shader", true, L"vertex", L"default.hlsl", L"pixel", L"default.hlsl", L"basemat", L"solid");
		out->writeLineBreak();
		out->writeElement(L"toggles", true, L"light", L"on");
		out->writeLineBreak();
		out->drop();
		xml = createIrrXMLReader("config.xml");
	}
	while (xml->read()) {
		const stringw which = xml->getNodeName();
		if (which == L"shader") {
			vertex = xml->getAttributeValueSafe("vertex");
			pixel = xml->getAttributeValueSafe("pixel");
			stringc base = xml->getAttributeValueSafe("basemat");
			if (base == "transparent_alpha") baseType = EMT_TRANSPARENT_ALPHA_CHANNEL;
			else if (base == "transparent_vertex") baseType = EMT_TRANSPARENT_VERTEX_ALPHA;
			else if (base == "solid") baseType = EMT_SOLID;
			else if (base == "parallax") baseType = EMT_PARALLAX_MAP_SOLID;
		}
		if (which == L"toggles") {
			stringc l = xml->getAttributeValueSafe("light");
			stringc s = xml->getAttributeValueSafe("shader");
			isLight = (l == "yes");
			useShader = (s == "yes");
		}
	}

	delete xml;

	if (!isLight) device->getLogger()->log(L"Light is OFF");
	if (!useShader) device->getLogger()->log(L"Shader is OFF -- what? Why?");

	path vertexFile = shader + vertex;
	path pixelFile = shader + pixel;

	if (useShader) {
		shaderMaterial = gpu->addHighLevelShaderMaterialFromFiles(
			vertexFile, "vertexMain", EVST_VS_3_0,
			pixelFile, "pixelMain", EPST_PS_3_0,
			cb, baseType, 0);
	}
	auto initmesh = smgr->getMesh("media/tux_final/tuxBACK.obj");
	auto mesh = smgr->getMeshManipulator()->createMeshWithTangents(initmesh);
	smgr->getMeshCache()->removeMesh(initmesh);

	auto node = smgr->addMeshSceneNode(mesh);
	node->getMaterial(0).setTexture(0, driver->getTexture("media/tux_final/tuxBACK_Material_BaseColor.jpg"));
	node->getMaterial(1).setTexture(0, driver->getTexture("media/tux_final/tuxBACK_Material.001_BaseColor.jpg"));
	node->getMaterial(2).setTexture(0, driver->getTexture("media/tux_final/tuxBACK_Material.002_BaseColor.jpg"));
	node->getMaterial(0).setTexture(1, driver->getTexture("media/tux_final/tuxBACK_Material_Normal.jpg"));
	node->getMaterial(1).setTexture(1, driver->getTexture("media/tux_final/tuxBACK_Material.001_Normal.jpg"));
	node->getMaterial(2).setTexture(1, driver->getTexture("media/tux_final/tuxBACK_Material.002_Normal.jpg"));

	if (useShader) node->setMaterialType((E_MATERIAL_TYPE)shaderMaterial);
	else node->setMaterialType(baseType);
	node->setMaterialFlag(EMF_FOG_ENABLE, true);
	driver->setFog(SColor(0, 100, 200, 50), EFT_FOG_LINEAR, 250, 500);

	node->setPosition(vector3df(0, 0, 0));
	node->setScale(vector3df(15, 15, 15));

	smgr->addTextSceneNode(device->getGUIEnvironment()->getBuiltInFont(),
		L"Normal maps included, even if not used.",
		video::SColor(255, 255, 255, 255), node);


	ISceneNodeAnimator* anim = smgr->createRotationAnimator(
		vector3df(0, 0, 0.2f));
	node->addAnimator(anim);
	anim->drop();
	/*
	anim = smgr->createFlyStraightAnimator(
		vector3df(0, 0, 400), vector3df(0, 0, -400), 10000, true, true);
	node->addAnimator(anim);
	anim->drop();
	//*/
	
	if (isLight) {
		setLight(SColorf(1, 1, 1), vector3df(0, 1, 0), .001f);
		///*
		setLight(SColorf(1, 0, 0), vector3df(1, 0, 0), .0004f);
		setLight(SColorf(0, 1, 0), vector3df(0, 0, 1), .0015f);
		setLight(SColorf(0, 0, 1), vector3df(1, 0, 1), .002f);
		setLight(SColorf(1, 0, 1), vector3df(1, 1, 0), .005f);
		setLight(SColorf(.5, .5, 1), vector3df(1, 0.5, 0), .0025f);
		setLight(SColorf(0, .5, .2), vector3df(1, 1, 1), .0001f);
		setLight(SColorf(.3, .1, .7), vector3df(0, 1, 1), .0008f);
		//*/
	}
	else {
		node->setMaterialFlag(EMF_LIGHTING, false);
	}

	driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);
	smgr->addSkyBoxSceneNode(
		driver->getTexture(media + "irrlicht2_up.jpg"),
		driver->getTexture(media + "irrlicht2_dn.jpg"),
		driver->getTexture(media + "irrlicht2_lf.jpg"),
		driver->getTexture(media + "irrlicht2_rt.jpg"),
		driver->getTexture(media + "irrlicht2_ft.jpg"),
		driver->getTexture(media + "irrlicht2_bk.jpg"));
	driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS();
	cam->setPosition(vector3df(-100, 50, 100));
	cam->setTarget(vector3df(0, 0, 0));
	device->getCursorControl()->setVisible(false);

	s32 lastFPS = -1;
	while (device->run()) if (device->isWindowActive())
	{
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255, 0, 0, 0));
		smgr->drawAll();
		driver->endScene();
		int fps = driver->getFPS();
		if (lastFPS != fps) {
			core::stringw str = L"DRIVER [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;
			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}
	device->drop();
	return 0;
}