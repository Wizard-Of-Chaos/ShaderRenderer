#pragma once
#ifndef SHADERS_H
#define SHADERS_H
#include "irrsetup.h"

class BaedsLights : public ILightManager
{
	public:
		virtual void onNodePreRender(ISceneNode* node) { current = node; }
		ISceneNode* currentNode() { return current;}
	private:
		ISceneNode* current = nullptr;
};

class irrExampleCB : public IShaderConstantSetCallBack
{
	public:
		irrExampleCB(IVideoDriver* driver) : driver(driver) {}
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	private:
		void m_getMatrices();
		IVideoDriver* driver;
		matrix4 m_world;
		matrix4 m_view;
		matrix4 m_proj;
		matrix4 m_invtranspose;
		matrix4 m_inv;
		matrix4 m_trans;
};

class basicNormalsCb : public IShaderConstantSetCallBack
{
	public:
		virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);
	private:
		void m_getMatrices();

		matrix4 m_world;
		matrix4 m_view;
		matrix4 m_proj;
		matrix4 m_invtranspose;

		matrix4 lights[8];
		vector3df radcones[8];
};

#endif