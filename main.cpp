#include "shaders.h"
#include <iostream>

IrrlichtDevice* device = 0;
ISceneManager* smgr = 0;
IVideoDriver* driver = 0;

int main()
{
	device = createDevice(EDT_DIRECT3D9, dimension2du(1440, 810));
	if (!device) return 1;
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	path media = "media/";

	auto gpu = driver->getGPUProgrammingServices();
	auto cb = new irrExampleCB(driver);

	s32 shaderMaterial = 0;
	E_MATERIAL_TYPE shaderType;


	path vertexFile = "shaders/d3d9.hlsl";
	path pixelFile = "shaders/d3d9.hlsl";


	shaderMaterial = gpu->addHighLevelShaderMaterialFromFiles(
		vertexFile, "vertexMain", EVST_VS_2_0,
		pixelFile, "pixelMain", EPST_PS_2_0,
		cb, EMT_SOLID, 0);
	shaderType = (E_MATERIAL_TYPE)shaderMaterial;

	auto initmesh = smgr->getMesh("media/tux_final/tuxBACK.obj");
	auto mesh = smgr->getMeshManipulator()->createMeshWithTangents(initmesh);

	auto node = smgr->addMeshSceneNode(mesh);
	node->getMaterial(0).setTexture(0, driver->getTexture("media/tux_final/tuxBACK_Material_BaseColor.jpg"));
	node->getMaterial(1).setTexture(0, driver->getTexture("media/tux_final/tuxBACK_Material.001_BaseColor.jpg"));
	node->getMaterial(2).setTexture(0, driver->getTexture("media/tux_final/tuxBACK_Material.002_BaseColor.jpg"));
	node->getMaterial(0).setTexture(1, driver->getTexture("media/tux_final/tuxBACK_Material_Normal.jpg"));
	node->getMaterial(1).setTexture(1, driver->getTexture("media/tux_final/tuxBACK_Material.001_Normal.jpg"));
	node->getMaterial(2).setTexture(1, driver->getTexture("media/tux_final/tuxBACK_Material.002_Normal.jpg"));

	node->setMaterialType(EMT_PARALLAX_MAP_SOLID);
	node->setPosition(vector3df(0, 0, 0));
	node->setScale(vector3df(15, 15, 15));

	smgr->addTextSceneNode(device->getGUIEnvironment()->getBuiltInFont(),
		L"TUX!",
		video::SColor(255, 255, 255, 255), node);

	
	ISceneNodeAnimator* anim = smgr->createRotationAnimator(
		vector3df(0, 0, 0.2f));
	node->addAnimator(anim);
	anim->drop();
	
	auto light = smgr->addLightSceneNode();
	anim = smgr->createFlyCircleAnimator(vector3df(0, 0, 0), 150.f, .0004f);
	light->addAnimator(anim);
	anim->drop();

	auto bill = smgr->addBillboardSceneNode(light, dimension2df(20.f, 20.f));
	bill->setMaterialTexture(0, driver->getTexture(media + "particlewhite.bmp"));
	bill->setMaterialFlag(EMF_LIGHTING, false);
	bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

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