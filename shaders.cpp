#include "shaders.h"
#include <string>

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
		if (i <= lightCount) {
			const SLight& light = driver->getDynamicLight(i);
			vector3df direction = (light.Position - nodepos);
			f32 dist = direction.getLength();
			binder[3] = dist;

			if (light.Type == ELT_POINT) {
				direction = direction.normalize();
			}
			else {
				direction = light.Direction;
				binder[3] = 1;
			}
			binder[0] = direction.X;
			binder[1] = direction.Y;
			binder[2] = direction.Z;

			SColorf dif = light.DiffuseColor;
			SColorf amb = light.AmbientColor;
			SColorf spec = light.SpecularColor;

			binder[4] = dif.r;
			binder[5] = dif.g;
			binder[6] = dif.b;
			binder[7] = dif.a;

			binder[8] = amb.r;
			binder[9] = amb.g;
			binder[10] = amb.b;
			binder[11] = amb.a;

			binder[12] = spec.r;
			binder[13] = spec.g;
			binder[14] = spec.b;
			binder[15] = spec.a;

			radcone.X = light.Radius;
			radcone.Y = light.InnerCone;
			radcone.Z = light.OuterCone;
		}
		else {
			binder[3] = -1;
		}
		std::string which = "LIGHTINFO_" + std::to_string(i);
		s32 matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, binder.pointer(), 16);
		which = "LIGHTRADCONE_" + std::to_string(i);
		matId = services->getPixelShaderConstantID(which.c_str());
		services->setPixelShaderConstant(matId, reinterpret_cast<f32*>(&radcone), 3);
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
