#include "shaders.h"

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
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&pos), 3);

	id = services->getVertexShaderConstantID("LIGHT_COLOR");
	SColorf color = device->getVideoDriver()->getDynamicLight(0).DiffuseColor;
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&color), 4);

	id = services->getVertexShaderConstantID("CAMERA_VIEW");
	vector3df camVec = device->getSceneManager()->getActiveCamera()->getAbsoluteTransformation().getRotationDegrees().rotationToDirection(vector3df(0, 0, 1));
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&camVec), 3);

}
