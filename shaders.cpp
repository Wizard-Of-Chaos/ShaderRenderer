#include "shaders.h"

void irrExampleCB::m_getMatrices()
{
	matrix4 world = driver->getTransform(ETS_WORLD);
	m_inv = world;
	m_inv.makeInverse();
	m_trans = world.getTransposed();

	m_worldviewproj = world * driver->getTransform(ETS_VIEW) * driver->getTransform(ETS_PROJECTION);
}


void irrExampleCB::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
	m_getMatrices();
	auto id = services->getVertexShaderConstantID("mWorldViewProj");
	services->setVertexShaderConstant(id, m_worldviewproj.pointer(), 16);

	id = services->getVertexShaderConstantID("mTransWorld");
	services->setVertexShaderConstant(id, m_trans.pointer(), 16);

	id = services->getVertexShaderConstantID("mInvWorld");
	services->setVertexShaderConstant(id, m_inv.pointer(), 16);

	id = services->getVertexShaderConstantID("mLightPos");
	vector3df pos = device->getSceneManager()->getActiveCamera()->getPosition();
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&pos), 3);

	id = services->getVertexShaderConstantID("mLightColor");
	SColorf color(0.f, 1.f, 1.f, 0.f);
	services->setVertexShaderConstant(id, reinterpret_cast<f32*>(&color), 4);

}
