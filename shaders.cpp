#include "shaders.h"
#include <string>
#include <iostream>

void irrExampleCB::m_getMatrices()
{
	m_world = driver->getTransform(ETS_WORLD);
	m_view = driver->getTransform(ETS_VIEW);
	m_proj = driver->getTransform(ETS_PROJECTION);
	m_inv = m_world;
	m_inv.makeInverse();
	m_trans = m_world.getTransposed();
	m_invtranspose = m_inv.getTransposed();
}


void irrExampleCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	m_getMatrices();
	auto id = services->getVertexShaderConstantID("WORLD");
	services->setVertexShaderConstant(id, m_world.pointer(), 16);

	id = services->getVertexShaderConstantID("VIEW");
	services->setVertexShaderConstant(id, m_view.pointer(), 16);

	id = services->getVertexShaderConstantID("PROJ");
	services->setVertexShaderConstant(id, m_proj.pointer(), 16);

	id = services->getVertexShaderConstantID("TRANS");
	services->setVertexShaderConstant(id, m_trans.pointer(), 16);

	id = services->getVertexShaderConstantID("INV");
	services->setVertexShaderConstant(id, m_inv.pointer(), 16);

	id = services->getVertexShaderConstantID("INV_TRANSPOSE");
	services->setVertexShaderConstant(id, m_invtranspose.pointer(), 16);

	SLight light;
	if (driver->getDynamicLightCount() > 0) light = driver->getDynamicLight(0);
	id = services->getVertexShaderConstantID("LIGHT_POS");
	vector3df pos = light.Position;
	//vector3df pos = device->getSceneManager()->getActiveCamera()->getAbsolutePosition();
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&pos), 3);

	id = services->getPixelShaderConstantID("LIGHT_DIFFUSE_COLOR");
	SColorf color = device->getVideoDriver()->getDynamicLight(0).DiffuseColor;
	services->setPixelShaderConstant(id, reinterpret_cast<f32*>(&color), 4);

	id = services->getPixelShaderConstantID("LIGHT_AMBIENT_COLOR");
	color = device->getVideoDriver()->getDynamicLight(0).AmbientColor;
	services->setPixelShaderConstant(id, reinterpret_cast<f32*>(&color), 4);
	
	id = services->getPixelShaderConstantID("LIGHT_SPECULAR_COLOR");
	color = device->getVideoDriver()->getDynamicLight(0).SpecularColor;
	services->setPixelShaderConstant(id, reinterpret_cast<f32*>(&color), 4);

	id = services->getVertexShaderConstantID("CAMERA_VIEW");
	vector3df camVec = device->getSceneManager()->getActiveCamera()->getAbsoluteTransformation().getRotationDegrees().rotationToDirection(vector3df(0, 0, 1));
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&camVec), 3);
}

void basicNormalsCb::m_getMatrices()
{
	auto driver = device->getVideoDriver();
	m_world = driver->getTransform(ETS_WORLD);
	m_view = driver->getTransform(ETS_VIEW);
	m_proj = driver->getTransform(ETS_PROJECTION);
	m_invtranspose = m_world;
	m_invtranspose.makeInverse();
	m_invtranspose = m_invtranspose.getTransposed();
}

void basicNormalsCb::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	m_getMatrices();
	auto id = services->getVertexShaderConstantID("WORLD");
	services->setVertexShaderConstant(id, m_world.pointer(), 16);
	id = services->getVertexShaderConstantID("VIEW");
	services->setVertexShaderConstant(id, m_view.pointer(), 16);
	id = services->getVertexShaderConstantID("PROJ");
	services->setVertexShaderConstant(id, m_proj.pointer(), 16);
	id = services->getVertexShaderConstantID("INV_TRANSPOSE");
	services->setVertexShaderConstant(id, m_invtranspose.pointer(), 16);
	id = services->getPixelShaderConstantID("CAMERA_VIEW");
	auto cam = device->getSceneManager()->getActiveCamera();
	vector3df camPos = cam->getAbsolutePosition();
	vector3df camVec = cam->getAbsoluteTransformation().getRotationDegrees().rotationToDirection(vector3df(0, 0, 1));
	f32 dist = cam->getFarValue();

	services->setPixelShaderConstant(id, reinterpret_cast<f32*>(&camVec), 3);
	
	id = services->getVertexShaderConstantID("CAMERA_POS");
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&camPos), 16);
	id = services->getVertexShaderConstantID("RENDER_DISTANCE");
	services->setVertexShaderConstant(id, &dist, 1);


	auto driver = device->getVideoDriver();
	vector3df nodepos(0, 0, 0);
	if(lmgr->currentNode()) nodepos = lmgr->currentNode()->getAbsolutePosition();
	s32 lightCount = driver->getDynamicLightCount();

	for (u32 i = 0; i < 8; ++i) {
		matrix4& binder = lights[i];
		vector3df& radcone = radcones[i];
		vector3df direction(0, 0, 0);
		vector3df position(0, 0, 0);
		SColorf diff, amb, spec;
		bool valid = false;
		if (i < lightCount) {
			valid = true;
			const SLight& light = driver->getDynamicLight(i);

			if (light.Type == ELT_POINT) {
				direction = (light.Position - nodepos);
				direction = direction.normalize();
			}
			else {
				direction = light.Direction;
				binder[3] = 1;
			}
			position = light.Position;

			diff = light.DiffuseColor;
			amb = light.AmbientColor;
			spec = light.SpecularColor;

			radcone.X = light.Radius;
			radcone.Y = light.InnerCone;
			radcone.Z = light.OuterCone;
		}
		else {
			valid = false;
		}
		int cast = valid;
		std::string which = "LIGHTVALID_" + std::to_string(i);
		s32 matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, &cast, 1);
		which = "LIGHTRADCONE_" + std::to_string(i);
		matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, reinterpret_cast<f32*>(&radcone), 3);
		which = "LIGHTDIR_" + std::to_string(i);
		matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, reinterpret_cast<f32*>(&direction), 3);
		which = "LIGHTPOS_" + std::to_string(i);
		matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, reinterpret_cast<f32*>(&position), 3);
		which = "LIGHTDIFF_" + std::to_string(i);
		matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, reinterpret_cast<f32*>(&diff), 4);
		which = "LIGHTAMB_" + std::to_string(i);
		matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, reinterpret_cast<f32*>(&amb), 4);
		which = "LIGHTSPEC_" + std::to_string(i);
		matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, reinterpret_cast<f32*>(&spec), 4);

	}
	
	id = services->getPixelShaderConstantID("HAS_FOG");
	int isFog = m_material->FogEnable;
	services->setPixelShaderConstant(id, &isFog, 1);
	if (m_material->FogEnable) {
		SColor col;
		f32 min, max, density;
		bool range, pix;
		E_FOG_TYPE type;
		driver->getFog(col, type, min, max, density, pix, range);
		SColorf colf(col);
		id = services->getPixelShaderConstantID("FOG_COLOR");
		services->setPixelShaderConstant(id, reinterpret_cast<f32*>(&colf), 4);
		id = services->getPixelShaderConstantID("FOG_MIN");
		services->setPixelShaderConstant(id, &min, 4);
		id = services->getPixelShaderConstantID("FOG_MAX");
		services->setPixelShaderConstant(id, &max, 4);
	}
}
