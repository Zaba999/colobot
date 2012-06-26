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

// taskbuild.cpp


#include <stdio.h>

#include "object/task/taskbuild.h"

#include "common/iman.h"
#include "old/light.h"
#include "old/terrain.h"
#include "old/water.h"
#include "math/geometry.h"
#include "object/auto/auto.h"
#include "object/motion/motionhuman.h"
#include "object/robotmain.h"
#include "physics/physics.h"
#include "ui/displaytext.h"




// Object's constructor.

CTaskBuild::CTaskBuild(CInstanceManager* iMan, CObject* object)
                       : CTask(iMan, object)
{
    int     i;

    m_type = OBJECT_DERRICK;
    m_time = 0.0f;
    m_soundChannel = -1;

    for ( i=0 ; i<TBMAXLIGHT ; i++ )
    {
        m_lightRank[i] = -1;
    }
}

// Object's destructor.

CTaskBuild::~CTaskBuild()
{
    int     i;

    if ( m_soundChannel != -1 )
    {
        m_sound->FlushEnvelope(m_soundChannel);
        m_sound->AddEnvelope(m_soundChannel, 0.0f, 1.0f, 1.0f, SOPER_STOP);
        m_soundChannel = -1;
    }

    for ( i=0 ; i<TBMAXLIGHT ; i++ )
    {
        if ( m_lightRank[i] == -1 )  continue;
        m_light->DeleteLight(m_lightRank[i]);
    }
}


// Creates a building.

bool CTaskBuild::CreateBuilding(Math::Vector pos, float angle)
{
    m_building = new CObject(m_iMan);
    if ( !m_building->CreateBuilding(pos, angle, 0.0f, m_type, 0.0f) )
    {
        delete m_building;
        m_building = 0;
        return false;
    }
    m_building->UpdateMapping();
    m_building->SetLock(true);  // not yet usable

    if ( m_type == OBJECT_DERRICK  )  m_buildingHeight = 35.0f;
    if ( m_type == OBJECT_FACTORY  )  m_buildingHeight = 28.0f;
    if ( m_type == OBJECT_REPAIR   )  m_buildingHeight = 30.0f;
    if ( m_type == OBJECT_STATION  )  m_buildingHeight = 13.0f;
    if ( m_type == OBJECT_CONVERT  )  m_buildingHeight = 20.0f;
    if ( m_type == OBJECT_TOWER    )  m_buildingHeight = 30.0f;
    if ( m_type == OBJECT_RESEARCH )  m_buildingHeight = 22.0f;
    if ( m_type == OBJECT_RADAR    )  m_buildingHeight = 19.0f;
    if ( m_type == OBJECT_ENERGY   )  m_buildingHeight = 20.0f;
    if ( m_type == OBJECT_LABO     )  m_buildingHeight = 16.0f;
    if ( m_type == OBJECT_NUCLEAR  )  m_buildingHeight = 40.0f;
    if ( m_type == OBJECT_PARA     )  m_buildingHeight = 68.0f;
    if ( m_type == OBJECT_INFO     )  m_buildingHeight = 19.0f;
    m_buildingHeight *= 0.25f;

    m_buildingPos = m_building->RetPosition(0);
    m_buildingPos.y -= m_buildingHeight;
    m_building->SetPosition(0, m_buildingPos);
    return true;
}

// Creates lights for the effects.

void CTaskBuild::CreateLight()
{
    D3DLIGHT7       light;
    D3DCOLORVALUE   color;
    Math::Vector        center, pos, dir;
    Math::Point         c, p;
    float           angle;
    int             i;

    if ( !m_engine->RetLightMode() )  return;

    center = m_metal->RetPosition(0);

    angle = 0;
    for ( i=0 ; i<TBMAXLIGHT ; i++ )
    {
        m_lightRank[i] = m_light->CreateLight();
        if ( m_lightRank[i] == -1 )  continue;

        c.x = center.x;
        c.y = center.z;
        p.x = center.x+40.0f;
        p.y = center.z;
        p = Math::RotatePoint(c, angle, p);
        pos.x = p.x;
        pos.z = p.y;
        pos.y = center.y+40.0f;
        dir = center-pos;

        ZeroMemory( &light, sizeof(light) );
        light.dltType       = D3DLIGHT_SPOT;
        light.dcvDiffuse.r  = 0.0f;
        light.dcvDiffuse.g  = 0.0f;
        light.dcvDiffuse.b  = 0.0f;  // white (invisible)
        light.dvPosition.x  = pos.x;
        light.dvPosition.y  = pos.y;
        light.dvPosition.z  = pos.z;
        light.dvDirection.x = dir.x;
        light.dvDirection.y = dir.y;
        light.dvDirection.z = dir.z;
        light.dvRange = D3DLIGHT_RANGE_MAX;
        light.dvFalloff = 1.0f;
        light.dvAttenuation0 = 1.0f;
        light.dvAttenuation1 = 0.0f;
        light.dvAttenuation2 = 0.0f;
        light.dvTheta = 0.0f;
        light.dvPhi = Math::PI/4.0f;
        m_light->SetLight(m_lightRank[i], light);

        color.r = -1.0f;
        color.g = -1.0f;
        color.b = -0.5f;  // violet
        color.a =  0.0f;
        m_light->SetLightColor(m_lightRank[i], color);
        m_light->SetLightColorSpeed(m_lightRank[i], 1.0f/((1.0f/m_speed)*0.25f));

        angle += (Math::PI*2.0f)/TBMAXLIGHT;
    }

    m_bBlack = false;
}

// Switches the lights from black to white.

void CTaskBuild::BlackLight()
{
    D3DCOLORVALUE   color;
    int             i;

    for ( i=0 ; i<TBMAXLIGHT ; i++ )
    {
        if ( m_lightRank[i] == -1 )  continue;

        color.r = 0.0f;
        color.g = 0.0f;
        color.b = 0.0f;  // white (invisible)
        color.a = 0.0f;
        m_light->SetLightColor(m_lightRank[i], color);
        m_light->SetLightColorSpeed(m_lightRank[i], 1.0f/((1.0f/m_speed)*0.75f));
    }

    m_bBlack = true;
}

// Management of an event.

bool CTaskBuild::EventProcess(const Event &event)
{
    Math::Matrix*       mat;
    Math::Vector        pos, dir, speed;
    Math::Point         dim;
    float           a, g, cirSpeed, dist, linSpeed;

    if ( m_engine->RetPause() )  return true;
    if ( event.event != EVENT_FRAME )  return true;
    if ( m_bError )  return false;

    m_time += event.rTime;

    m_progress += event.rTime*m_speed;  // other advance

    if ( m_phase == TBP_TURN )  // preliminary rotation?
    {
        a = m_object->RetAngleY(0);
        g = m_angleY;
        cirSpeed = Math::Direction(a, g)*1.0f;
        if ( cirSpeed >  1.0f )  cirSpeed =  1.0f;
        if ( cirSpeed < -1.0f )  cirSpeed = -1.0f;

        m_physics->SetMotorSpeedZ(cirSpeed);  // turns left/right
        return true;
    }

    if ( m_phase == TBP_MOVE )  // preliminary forward/backward?
    {
        dist = Math::Distance(m_object->RetPosition(0), m_metal->RetPosition(0));
        linSpeed = 0.0f;
        if ( dist > 30.0f )  linSpeed =  1.0f;
        if ( dist < 30.0f )  linSpeed = -1.0f;
        m_physics->SetMotorSpeedX(linSpeed);  // forward/backward
        return true;
    }

    if ( m_phase == TBP_RECEDE )  // terminal back?
    {
        m_physics->SetMotorSpeedX(-1.0f);  // back
        return true;
    }

    if ( m_phase == TBP_TAKE )  // takes gun?
    {
        return true;
    }

    if ( m_phase == TBP_PREP )  // prepares?
    {
        return true;
    }

    if ( m_phase == TBP_TERM )  // ends?
    {
        return true;
    }

    if ( !m_bBuild )  // building to build?
    {
        m_bBuild = true;

        pos = m_metal->RetPosition(0);
        a   = m_object->RetAngleY(0);
        if ( !CreateBuilding(pos, a+Math::PI) )
        {
            m_metal->SetLock(false);  // usable again
            m_motion->SetAction(-1);
            m_object->SetObjectParent(14, 0);
            m_object->SetPosition(14, Math::Vector(-1.5f, 0.3f, -1.35f));
            m_object->SetAngleZ(14, Math::PI);
            m_camera->FlushEffect();
            Abort();
            m_bError = true;
            m_displayText->DisplayError(ERR_TOOMANY, m_object->RetPosition(0));
            return false;
        }
        CreateLight();
    }

    pos = m_buildingPos;
    pos.y += m_buildingHeight*m_progress;
    m_building->SetPosition(0, pos);  // the building rises

    m_building->SetZoom(0, m_progress*0.75f+0.25f);
    m_metal->SetZoom(0, 1.0f-m_progress);

    a = (2.0f-2.0f*m_progress);
    if ( a > 1.0f )  a = 1.0f;
    dir.x = (Math::Rand()-0.5f)*a*0.1f;
    dir.z = (Math::Rand()-0.5f)*a*0.1f;
    dir.y = (Math::Rand()-0.5f)*a*0.1f;
    m_building->SetCirVibration(dir);

    if ( !m_bBlack && m_progress >= 0.25f )
    {
        BlackLight();
    }

    if ( m_lastParticule+m_engine->ParticuleAdapt(0.05f) <= m_time )
    {
        m_lastParticule = m_time;

        pos = m_metal->RetPosition(0);
        speed.x = (Math::Rand()-0.5f)*20.0f;
        speed.z = (Math::Rand()-0.5f)*20.0f;
        speed.y = Math::Rand()*10.0f;
        dim.x = Math::Rand()*6.0f+4.0f;
        dim.y = dim.x;
        m_particule->CreateParticule(pos, speed, dim, PARTIFIRE);

        pos = Math::Vector(0.0f, 0.5f, 0.0f);
        mat = m_object->RetWorldMatrix(14);
        pos = Transform(*mat, pos);
        speed = m_metal->RetPosition(0);
        speed.x += (Math::Rand()-0.5f)*5.0f;
        speed.z += (Math::Rand()-0.5f)*5.0f;
        speed -= pos;
        dim.x = 2.0f;
        dim.y = dim.x;
        m_particule->CreateParticule(pos, speed, dim, PARTIFIREZ);

        if ( Math::Rand() < 0.3f )
        {
            m_sound->Play(SOUND_BUILD, m_object->RetPosition(0), 0.5f, 1.0f*Math::Rand()*1.5f);
        }
    }

    return true;
}


// Assigns the goal was achieved.

Error CTaskBuild::Start(ObjectType type)
{
    Math::Vector    pos, speed, pv, pm;
    Error       err;
    float       iAngle, oAngle;

    m_type = type;
    m_lastParticule = 0.0f;
    m_progress = 0.0f;

    iAngle = m_object->RetAngleY(0);
    iAngle = Math::NormAngle(iAngle);  // 0..2*Math::PI
    oAngle = iAngle;

    m_bError = true;  // operation impossible

    pos = m_object->RetPosition(0);
    if ( pos.y < m_water->RetLevel() )  return ERR_BUILD_WATER;

    if ( !m_physics->RetLand() )  return ERR_BUILD_FLY;

    speed = m_physics->RetMotorSpeed();
    if ( speed.x != 0.0f ||
         speed.z != 0.0f )  return ERR_BUILD_MOTOR;

    if ( m_object->RetFret() != 0 )  return ERR_MANIP_BUSY;

    m_metal = SearchMetalObject(oAngle, 2.0f, 100.0f, Math::PI*0.25f, err);
    if ( err == ERR_BUILD_METALNEAR && m_metal != 0 )
    {
        err = FlatFloor();
        if ( err != ERR_OK )  return err;
        return ERR_BUILD_METALNEAR;
    }
    if ( err != ERR_OK )  return err;

    err = FlatFloor();
    if ( err != ERR_OK )  return err;

    m_metal->SetLock(true);  // not usable
    m_camera->StartCentering(m_object, Math::PI*0.15f, 99.9f, 0.0f, 1.0f);
        
    m_phase = TBP_TURN;  // rotation necessary preliminary
    m_angleY = oAngle;  // angle was reached

    pv = m_object->RetPosition(0);
    pv.y += 8.3f;
    pm = m_metal->RetPosition(0);
    m_angleZ = Math::RotateAngle(Math::DistanceProjected(pv, pm), fabs(pv.y-pm.y));

    m_physics->SetFreeze(true);  // it does not move

    m_bBuild = false;  // not yet built
    m_bError = false;  // ok
    return ERR_OK;
}

// Indicates whether the action is finished.

Error CTaskBuild::IsEnded()
{
    CAuto*      automat;
    float       angle, dist, time;

    if ( m_engine->RetPause() )  return ERR_CONTINUE;
    if ( m_bError )  return ERR_STOP;

    if ( m_phase == TBP_TURN )  // preliminary rotation?
    {
        angle = m_object->RetAngleY(0);
        angle = Math::NormAngle(angle);  // 0..2*Math::PI

        if ( Math::TestAngle(angle, m_angleY-Math::PI*0.01f, m_angleY+Math::PI*0.01f) )
        {
            m_physics->SetMotorSpeedZ(0.0f);

            dist = Math::Distance(m_object->RetPosition(0), m_metal->RetPosition(0));
            if ( dist > 30.0f )
            {
                time = m_physics->RetLinTimeLength(dist-30.0f, 1.0f);
                m_speed = 1.0f/time;
            }
            else
            {
                time = m_physics->RetLinTimeLength(30.0f-dist, -1.0f);
                m_speed = 1.0f/time;
            }
            m_phase = TBP_MOVE;
            m_progress = 0.0f;
        }
        return ERR_CONTINUE;
    }

    if ( m_phase == TBP_MOVE )  // preliminary forward/backward?
    {
        dist = Math::Distance(m_object->RetPosition(0), m_metal->RetPosition(0));

        if ( dist >= 25.0f && dist <= 35.0f )
        {
            m_physics->SetMotorSpeedX(0.0f);
            m_motion->SetAction(MHS_GUN);  // takes gun

            m_phase = TBP_TAKE;
            m_speed = 1.0f/1.0f;
            m_progress = 0.0f;
        }
        else
        {
            if ( m_progress > 1.0f )  // timeout?
            {
                m_metal->SetLock(false);  // usable again
                if ( dist < 30.0f )  return ERR_BUILD_METALNEAR;
                else                 return ERR_BUILD_METALAWAY;
            }
        }
        return ERR_CONTINUE;
    }

    if ( m_phase == TBP_TAKE )  // takes gun
    {
        if ( m_progress < 1.0f )  return ERR_CONTINUE;

        m_motion->SetAction(MHS_FIRE);  // shooting position
        m_object->SetObjectParent(14, 4);
        m_object->SetPosition(14, Math::Vector(0.6f, 0.1f, 0.3f));
        m_object->SetAngleZ(14, 0.0f);

        m_phase = TBP_PREP;
        m_speed = 1.0f/1.0f;
        m_progress = 0.0f;
    }

    if ( m_phase == TBP_PREP )  // prepares?
    {
        if ( m_progress < 1.0f )  return ERR_CONTINUE;

        m_soundChannel = m_sound->Play(SOUND_TREMBLE, m_object->RetPosition(0), 0.0f, 1.0f, true);
        m_sound->AddEnvelope(m_soundChannel, 0.7f, 1.0f, 1.0f, SOPER_CONTINUE);
        m_sound->AddEnvelope(m_soundChannel, 0.7f, 1.5f, 7.0f, SOPER_CONTINUE);
        m_sound->AddEnvelope(m_soundChannel, 0.0f, 1.5f, 2.0f, SOPER_STOP);

        m_camera->StartEffect(CE_VIBRATION, m_metal->RetPosition(0), 1.0f);

        m_phase = TBP_BUILD;
        m_speed = 1.0f/10.f;  // duration of 10s
        m_progress = 0.0f;
    }

    if ( m_phase == TBP_BUILD )  // construction?
    {
        if ( m_progress < 1.0f )  return ERR_CONTINUE;

        DeleteMark(m_metal->RetPosition(0), 20.0f);

        m_metal->DeleteObject();  // removes the metal
        delete m_metal;
        m_metal = 0;

        m_building->SetZoom(0, 1.0f);
        m_building->SetCirVibration(Math::Vector(0.0f, 0.0f, 0.0f));
        m_building->SetLock(false);  // building usable
        m_main->CreateShortcuts();
        m_displayText->DisplayError(INFO_BUILD, m_buildingPos, 10.0f, 50.0f);

        automat = m_building->RetAuto();
        if ( automat != 0 )
        {
            automat->Init();
        }

        m_motion->SetAction(MHS_GUN);  // hands gun
        m_phase = TBP_TERM;
        m_speed = 1.0f/1.0f;
        m_progress = 0.0f;
    }

    if ( m_phase == TBP_TERM )  // rotation terminale ?
    {
        if ( m_progress < 1.0f )  return ERR_CONTINUE;

        m_motion->SetAction(-1);
        m_object->SetObjectParent(14, 0);
        m_object->SetPosition(14, Math::Vector(-1.5f, 0.3f, -1.35f));
        m_object->SetAngleZ(14, Math::PI);

        if ( m_type == OBJECT_FACTORY  ||
             m_type == OBJECT_RESEARCH ||
             m_type == OBJECT_NUCLEAR  )
        {

            m_phase = TBP_RECEDE;
            m_speed = 1.0f/1.5f;
            m_progress = 0.0f;
        }
    }

    if ( m_phase == TBP_RECEDE )  // back?
    {
        if ( m_progress < 1.0f )  return ERR_CONTINUE;

        m_physics->SetMotorSpeedX(0.0f);
    }

    Abort();
    return ERR_STOP;
}

// Suddenly ends the current action.

bool CTaskBuild::Abort()
{
    if ( m_soundChannel != -1 )
    {
        m_sound->FlushEnvelope(m_soundChannel);
        m_sound->AddEnvelope(m_soundChannel, 0.0f, 1.0f, 1.0f, SOPER_STOP);
        m_soundChannel = -1;
    }

    m_camera->StopCentering(m_object, 2.0f);
    m_physics->SetFreeze(false);  // is moving again
    return true;
}


// Checks whether the terrain is fairly flat
// and if there is not too close to another object.

Error CTaskBuild::FlatFloor()
{
    CObject     *pObj;
    ObjectType  type;
    Math::Vector    center, pos, oPos, bPos;
    Math::Point     c, p;
    float       radius, max, oRadius, bRadius, angle, dist;
    int         i, j;
    bool        bLittleFlat, bBase;

    radius = 0.0f;
    if ( m_type == OBJECT_DERRICK  )  radius =  5.0f;
    if ( m_type == OBJECT_FACTORY  )  radius = 15.0f;
    if ( m_type == OBJECT_REPAIR   )  radius = 12.0f;
    if ( m_type == OBJECT_STATION  )  radius = 12.0f;
    if ( m_type == OBJECT_CONVERT  )  radius = 12.0f;
    if ( m_type == OBJECT_TOWER    )  radius =  7.0f;
    if ( m_type == OBJECT_RESEARCH )  radius = 10.0f;
    if ( m_type == OBJECT_RADAR    )  radius =  5.0f;
    if ( m_type == OBJECT_ENERGY   )  radius =  8.0f;
    if ( m_type == OBJECT_LABO     )  radius = 12.0f;
    if ( m_type == OBJECT_NUCLEAR  )  radius = 20.0f;
    if ( m_type == OBJECT_PARA     )  radius = 20.0f;
    if ( m_type == OBJECT_INFO     )  radius =  5.0f;
    if ( radius == 0.0f )  return ERR_GENERIC;

    center = m_metal->RetPosition(0);
    angle = m_terrain->RetFineSlope(center);
    bLittleFlat = ( angle < FLATLIMIT );

    max = m_terrain->RetFlatZoneRadius(center, radius);
    if ( max < radius )  // area too small?
    {
        if ( bLittleFlat )
        {
            m_main->SetShowLimit(1, PARTILIMIT3, m_metal, center, max, 10.0f);
        }
        return bLittleFlat?ERR_BUILD_FLATLIT:ERR_BUILD_FLAT;
    }

    max = 100000.0f;
    bBase = false;
    for ( i=0 ; i<1000000 ; i++ )
    {
        pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
        if ( pObj == 0 )  break;

        if ( !pObj->RetActif() )  continue;  // inactive?
        if ( pObj->RetTruck() != 0 )  continue;  // object transported?
        if ( pObj == m_metal )  continue;
        if ( pObj == m_object )  continue;

        type = pObj->RetType();
        if ( type == OBJECT_BASE )
        {
            oPos = pObj->RetPosition(0);
            dist = Math::Distance(center, oPos)-80.0f;
            if ( dist < max )
            {
                max = dist;
                bPos = oPos;
                bRadius = oRadius;
                bBase = true;
            }
        }
        else
        {
            j = 0;
            while ( pObj->GetCrashSphere(j++, oPos, oRadius) )
            {
                dist = Math::Distance(center, oPos)-oRadius;
                if ( dist < max )
                {
                    max = dist;
                    bPos = oPos;
                    bRadius = oRadius;
                    bBase = false;
                }
            }
        }
    }
    if ( max < radius )
    {
        m_main->SetShowLimit(1, PARTILIMIT2, m_metal, center, max, 10.0f);
        if ( bRadius < 2.0f )  bRadius = 2.0f;
        m_main->SetShowLimit(2, PARTILIMIT3, m_metal, bPos, bRadius, 10.0f);
        return bBase?ERR_BUILD_BASE:ERR_BUILD_BUSY;
    }

    max = 100000.0f;
    for ( i=0 ; i<1000000 ; i++ )
    {
        pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
        if ( pObj == 0 )  break;

        if ( !pObj->RetActif() )  continue;  // inactive?
        if ( pObj->RetTruck() != 0 )  continue;  // object transported?
        if ( pObj == m_metal )  continue;
        if ( pObj == m_object )  continue;

        type = pObj->RetType();
        if ( type == OBJECT_DERRICK  ||
             type == OBJECT_FACTORY  ||
             type == OBJECT_STATION  ||
             type == OBJECT_CONVERT  ||
             type == OBJECT_REPAIR   ||
             type == OBJECT_TOWER    ||
             type == OBJECT_RESEARCH ||
             type == OBJECT_RADAR    ||
             type == OBJECT_ENERGY   ||
             type == OBJECT_LABO     ||
             type == OBJECT_NUCLEAR  ||
             type == OBJECT_START    ||
             type == OBJECT_END      ||
             type == OBJECT_INFO     ||
             type == OBJECT_PARA     ||
             type == OBJECT_SAFE     ||
             type == OBJECT_HUSTON   )  // building?
        {
            j = 0;
            while ( pObj->GetCrashSphere(j++, oPos, oRadius) )
            {
                dist = Math::Distance(center, oPos)-oRadius;
                if ( dist < max )
                {
                    max = dist;
                    bPos = oPos;
                    bRadius = oRadius;
                }
            }
        }
    }
    if ( max-BUILDMARGIN < radius )
    {
        m_main->SetShowLimit(1, PARTILIMIT2, m_metal, center, max-BUILDMARGIN, 10.0f);
        m_main->SetShowLimit(2, PARTILIMIT3, m_metal, bPos, bRadius+BUILDMARGIN, 10.0f);
        return bBase?ERR_BUILD_BASE:ERR_BUILD_NARROW;
    }

    return ERR_OK;
}

// Seeks the nearest metal object.

CObject* CTaskBuild::SearchMetalObject(float &angle, float dMin, float dMax,
                                       float aLimit, Error &err)
{
    CObject     *pObj, *pBest;
    Math::Vector    iPos, oPos;
    ObjectType  type;
    float       min, iAngle, a, aa, aBest, distance, magic;
    int         i;
    bool        bMetal;

    iPos   = m_object->RetPosition(0);
    iAngle = m_object->RetAngleY(0);
    iAngle = Math::NormAngle(iAngle);  // 0..2*Math::PI

    min = 1000000.0f;
    pBest = 0;
    bMetal = false;
    for ( i=0 ; i<1000000 ; i++ )
    {
        pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
        if ( pObj == 0 )  break;

        if ( !pObj->RetActif() )  continue;  // objet inactive?
        if ( pObj->RetTruck() != 0 )  continue;  // object transported?

        type = pObj->RetType();
        if ( type != OBJECT_METAL )  continue;

        bMetal = true;  // metal exists

        oPos = pObj->RetPosition(0);
        distance = Math::Distance(oPos, iPos);
        a = Math::RotateAngle(oPos.x-iPos.x, iPos.z-oPos.z);  // CW!

        if ( distance > dMax )  continue;
        if ( !Math::TestAngle(a, iAngle-aLimit, iAngle+aLimit) )  continue;

        if ( distance < dMin )
        {
            err = ERR_BUILD_METALNEAR;  // too close
            return pObj;
        }

        aa = fabs(a-iAngle);
        if ( aa > Math::PI )  aa = Math::PI*2.0f-aa;
        magic = distance*aa;

        if ( magic < min )
        {
            min = magic;
            aBest = a;
            pBest = pObj;
        }
    }

    if ( pBest == 0 )
    {
        if ( bMetal )  err = ERR_BUILD_METALAWAY;  // too far
        else           err = ERR_BUILD_METALINEX;  // non-existent
    }
    else
    {
        angle = aBest;
        err = ERR_OK;
    }
    return pBest;
}

// Destroys all the close marks.

void CTaskBuild::DeleteMark(Math::Vector pos, float radius)
{
    CObject*    pObj;
    Math::Vector    oPos;
    ObjectType  type;
    float       distance;
    int         i;

    for ( i=0 ; i<1000000 ; i++ )
    {
        pObj = (CObject*)m_iMan->SearchInstance(CLASS_OBJECT, i);
        if ( pObj == 0 )  break;

        type = pObj->RetType();
        if ( type != OBJECT_MARKSTONE   &&
             type != OBJECT_MARKURANIUM &&
             type != OBJECT_MARKKEYa    &&
             type != OBJECT_MARKKEYb    &&
             type != OBJECT_MARKKEYc    &&
             type != OBJECT_MARKKEYd    &&
             type != OBJECT_MARKPOWER   )  continue;

        oPos = pObj->RetPosition(0);
        distance = Math::Distance(oPos, pos);
        if ( distance <= radius )
        {
            pObj->DeleteObject();  // removes the mark
            delete pObj;
            i --;
        }
    }
}

