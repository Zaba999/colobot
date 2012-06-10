// * This file is part of the COLOBOT source code
// * Copyright (C) 2001-2008, Daniel ROUX & EPSITEC SA, www.epsitec.ch
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program. If not, see  http://www.gnu.org/licenses/.

// control.h

#pragma once


#include "graphics/common/text.h"
#include "common/struct.h"
#include "common/event.h"


class CInstanceManager;
class CEvent;
class CD3DEngine;
class CRobotMain;
class CParticule;
class CSound;


#define STATE_ENABLE		(1<<0)	// active
#define STATE_CHECK		(1<<1)	// pressed
#define STATE_HILIGHT		(1<<2)	// overflown by mouse
#define STATE_PRESS		(1<<3)	// pressed by mouse
#define STATE_VISIBLE		(1<<4)	// visible
#define STATE_DEAD		(1<<5)	// inaccessible (x)
#define STATE_DEFAULT		(1<<6)	// actuated by RETURN
#define STATE_OKAY		(1<<7)	// green point at the bottom right
#define STATE_SHADOW		(1<<8)	// shadow
#define STATE_GLINT		(1<<9)	// dynamic reflection
#define STATE_CARD		(1<<10)	// tab
#define STATE_EXTEND		(1<<11)	// extended mode
#define STATE_SIMPLY		(1<<12)	// undecorated
#define STATE_FRAME		(1<<13)	// framework highlighting
#define STATE_WARNING		(1<<14)	// framework hatched yellow / black
#define STATE_VALUE		(1<<15)	// displays the value
#define STATE_RUN		(1<<16)	// running program



class CControl
{
public:
	CControl(CInstanceManager* iMan);
	virtual ~CControl();

	virtual bool	Create(FPOINT pos, FPOINT dim, int icon, EventMsg eventMsg);

	virtual bool	EventProcess(const Event &event);

	virtual void	SetPos(FPOINT pos);
	virtual FPOINT	RetPos();
	virtual void	SetDim(FPOINT dim);
	virtual FPOINT	RetDim();
	virtual bool	SetState(int state, bool bState);
	virtual bool	SetState(int state);
	virtual bool	ClearState(int state);
	virtual bool	TestState(int state);
	virtual	int		RetState();
	virtual void	SetIcon(int icon);
	virtual int		RetIcon();
	virtual void	SetName(char* name, bool bTooltip=true);
	virtual char*	RetName();
	virtual void	SetJustif(int mode);
	virtual int		RetJustif();
	virtual	void	SetFontSize(float size);
	virtual	float	RetFontSize();
	virtual	void	SetFontStretch(float stretch);
	virtual	float	RetFontStretch();
	virtual void	SetFontType(FontType font);
	virtual FontType RetFontType();
	virtual bool	SetTooltip(char* name);
	virtual bool	GetTooltip(FPOINT pos, char* name);
	virtual void	SetFocus(bool bFocus);
	virtual bool	RetFocus();

	virtual EventMsg RetEventMsg();

	virtual void	Draw();

protected:
			void	GlintDelete();
			void	GlintCreate(FPOINT ref, bool bLeft=true, bool bUp=true);
			void	GlintFrame(const Event &event);
			void	DrawPart(int icon, float zoom, float ex);
			void	DrawIcon(FPOINT pos, FPOINT dim, FPOINT uv1, FPOINT uv2, float ex=0.0f);
			void	DrawIcon(FPOINT pos, FPOINT dim, FPOINT uv1, FPOINT uv2, FPOINT corner, float ex);
			void	DrawWarning(FPOINT pos, FPOINT dim);
			void	DrawShadow(FPOINT pos, FPOINT dim, float deep=1.0f);
	virtual	bool	Detect(FPOINT pos);

protected:
	CInstanceManager* m_iMan;
	CD3DEngine*	m_engine;
	CEvent*		m_event;
	CRobotMain*	m_main;
	CParticule*	m_particule;
	CSound*		m_sound;

	FPOINT		m_pos;			// corner upper / left
	FPOINT		m_dim;			// dimensions
	int		m_icon;
	EventMsg	m_eventMsg;		// message to send when clicking
	int		m_state;		// states (STATE_ *)
	float		m_fontSize;		// size of the button name
	float		m_fontStretch;		// stretch of the font
	FontType	m_fontType;		// type of font
	int		m_justif;		// type of justification (-1,0,1)
	char		m_name[100];		// name of the button
	char		m_tooltip[100];		// name of tooltip
	bool		m_bFocus;
	bool		m_bCapture;

	bool		m_bGlint;
	FPOINT		m_glintCorner1;
	FPOINT		m_glintCorner2;
	float		m_glintProgress;
	FPOINT		m_glintMouse;
};


