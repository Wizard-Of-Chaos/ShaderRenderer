#pragma once
#ifndef SHADERS_H
#define SHADERS_H
#include "irrsetup.h"

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

#endif