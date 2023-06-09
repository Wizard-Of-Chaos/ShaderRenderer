// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_TEXT_SCENE_NODE_H_INCLUDED
#define IRR_I_TEXT_SCENE_NODE_H_INCLUDED

#include "ISceneNode.h"

namespace irr
{

namespace gui
{
	class IGUIFont;
}

namespace scene
{

//! A scene node for displaying 2d text at a position in three dimensional space
class ITextSceneNode : public ISceneNode
{
public:

	//! constructor
	ITextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
		const core::vector3df& position = core::vector3df(0,0,0))
			: ISceneNode(parent, mgr, id, position) {}

	//! sets the text string
	virtual void setText(const wchar_t* text) = 0;

	//! get the text string
	virtual const wchar_t* getText() const = 0;

	//! sets the color of the text
	virtual void setTextColor(video::SColor color) = 0;

	//! get the color of the text
	virtual video::SColor getTextColor() const = 0;

	//! set the font used to draw the text
	virtual void setFont(gui::IGUIFont* font) = 0;

	//! Get the font used to draw the text
	virtual gui::IGUIFont* getFont() const = 0;
};

} // end namespace scene
} // end namespace irr

#endif
