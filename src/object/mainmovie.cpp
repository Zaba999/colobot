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

// mainmovie.cpp


#include <stdio.h>


#include "object/mainmovie.h"

#include "math/geometry.h"
#include "common/iman.h"
#include "object/motion/motionhuman.h"
#include "object/robotmain.h"




// Constructor of the application card.

CMainMovie::CMainMovie(CInstanceManager* iMan)
{
    m_iMan = iMan;
    m_iMan->AddInstance(CLASS_SHORT, this);

    m_interface = (CInterface*)m_iMan->SearchInstance(CLASS_INTERFACE);
    m_event     = (CEvent*)m_iMan->SearchInstance(CLASS_EVENT);
    m_engine    = (CD3DEngine*)m_iMan->SearchInstance(CLASS_ENGINE);
    m_main      = (CRobotMain*)m_iMan->SearchInstance(CLASS_MAIN);
    m_camera    = (CCamera*)m_iMan->SearchInstance(CLASS_CAMERA);
    m_sound     = (CSound*)m_iMan->SearchInstance(CLASS_SOUND);

    Flush();
}

// Destructor of the application card.

CMainMovie::~CMainMovie()
{
}


// Stops the current movie.

void CMainMovie::Flush()
{
    m_type = MM_NONE;
}


// Start of a film.

bool CMainMovie::Start(MainMovieType type, float time)
{
    Math::Matrix*   mat;
    Math::Vector    pos;
    CObject*    pObj;
    CMotion*    motion;

    m_type = type;
    m_speed = 1.0f/time;
    m_progress = 0.0f;

    if ( m_type == MM_SATCOMopen )
    {
        pObj = m_main->SearchHuman();
        if ( pObj == 0 )
        {
            m_type = MM_NONE;  // it's over!
            return true;
        }

        motion = pObj->RetMotion();
        if ( motion != 0 )
        {
            motion->SetAction(MHS_SATCOM, 0.5f);  // reads the SatCom
        }

        m_camera->RetCamera(m_initialEye, m_initialLookat);
        m_camera->SetType(CAMERA_SCRIPT);
        m_camera->SetSmooth(CS_HARD);
        m_camera->SetScriptEye(m_initialEye);
        m_camera->SetScriptLookat(m_initialLookat);
        m_camera->FixCamera();

        mat = pObj->RetWorldMatrix(0);
        m_finalLookat[0] = Math::Transform(*mat, Math::Vector( 1.6f, 1.0f, 1.2f));
        m_finalEye[0]    = Math::Transform(*mat, Math::Vector(-1.5f, 5.0f, 3.0f));
        m_finalLookat[1] = Math::Transform(*mat, Math::Vector( 1.6f, 1.0f, 1.2f));
        m_finalEye[1]    = Math::Transform(*mat, Math::Vector( 0.8f, 3.0f, 0.8f));
    }

    if ( m_type == MM_SATCOMclose )
    {
        pObj = m_main->SearchHuman();
        if ( pObj != 0 )
        {
            motion = pObj->RetMotion();
            if ( motion != 0 )
            {
                motion->SetAction(-1);  // finishes reading SatCom
            }
        }

        m_camera->SetType(CAMERA_BACK);
        m_type = MM_NONE;  // it's already over!
    }

    return true;
}

// Stop a current movie.

bool CMainMovie::Stop()
{
    CObject*    pObj;
    CMotion*    motion;

    if ( m_type == MM_SATCOMopen )
    {
        pObj = m_main->SearchHuman();
        if ( pObj != 0 )
        {
            motion = pObj->RetMotion();
            if ( motion != 0 )
            {
                motion->SetAction(-1);  // finishes reading SatCom
            }
        }
    }

    m_type = MM_NONE;
    return true;
}

// Indicates whether a film is in progress.

bool CMainMovie::IsExist()
{
    return (m_type != MM_NONE);
}


// Processing an event.

bool CMainMovie::EventProcess(const Event &event)
{
    Math::Vector    initialEye, initialLookat, finalEye, finalLookat, eye, lookat;
    float       progress;

    if ( m_type == MM_NONE )  return true;

    m_progress += event.rTime*m_speed;

    if ( m_type == MM_SATCOMopen )
    {
        if ( m_progress < 1.0f )
        {
            progress = 1.0f-powf(1.0f-m_progress, 3.0f);

            if ( progress < 0.6f )
            {
                progress = progress/0.6f;
                initialEye    = m_initialEye;
                initialLookat = m_initialLookat;
                finalEye      = m_finalEye[0];
                finalLookat   = m_finalLookat[0];
            }
            else
            {
                progress = (progress-0.6f)/0.3f;
                initialEye    = m_finalEye[0];
                initialLookat = m_finalLookat[0];
                finalEye      = m_finalEye[1];
                finalLookat   = m_finalLookat[1];
            }
            if ( progress > 1.0f )  progress = 1.0f;

            eye = (finalEye-initialEye)*progress+initialEye;
            lookat = (finalLookat-initialLookat)*progress+initialLookat;
            m_camera->SetScriptEye(eye);
            m_camera->SetScriptLookat(lookat);
//          m_camera->FixCamera();
        }
        else
        {
            m_stopType = m_type;
            Flush();
            return false;
        }
    }

    if ( m_type == MM_SATCOMclose )
    {
        if ( m_progress < 1.0f )
        {
        }
        else
        {
            m_stopType = m_type;
            Flush();
            return false;
        }
    }

    return true;
}


// Returns the type of the current movie.

MainMovieType CMainMovie::RetType()
{
    return m_type;
}

// Returns the type of movie stop.

MainMovieType CMainMovie::RetStopType()
{
    return m_stopType;
}


