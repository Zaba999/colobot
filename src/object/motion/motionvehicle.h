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

// motionvehicle.h

#pragma once


#include "object/motion/motion.h"


class CInstanceManager;
class CEngine;
class CLight;
class CParticule;
class CTerrain;
class CCamera;
class CBrain;
class CPhysics;
class CObject;


class CMotionVehicle : public CMotion
{
public:
	CMotionVehicle(CInstanceManager* iMan, CObject* object);
	~CMotionVehicle();

	void		DeleteObject(bool bAll=false);
	bool		Create(D3DVECTOR pos, float angle, ObjectType type, float power);
	bool		EventProcess(const Event &event);

	bool		RetTraceDown();
	void		SetTraceDown(bool bDown);
	int			RetTraceColor();
	void		SetTraceColor(int color);
	float		RetTraceWidth();
	void		SetTraceWidth(float width);

protected:
	void		CreatePhysics(ObjectType type);
	bool		EventFrame(const Event &event);
	bool		EventFrameFly(const Event &event);
	bool		EventFrameInsect(const Event &event);
	bool		EventFrameCanoni(const Event &event);
	void		UpdateTrackMapping(float left, float right, ObjectType type);

protected:
	float		m_wheelTurn[4];
	float		m_flyPaw[3];
	float		m_posTrackLeft;
	float		m_posTrackRight;
	int			m_partiReactor;
	float		m_armTimeAbs;
	float		m_armMember;
	float		m_canonTime;
	float		m_lastTimeCanon;
	D3DVECTOR	m_wheelLastPos;
	D3DVECTOR	m_wheelLastAngle;
	D3DVECTOR	m_posKey;
	bool		m_bFlyFix;
	bool		m_bTraceDown;
	int			m_traceColor;
	float		m_traceWidth;
};

