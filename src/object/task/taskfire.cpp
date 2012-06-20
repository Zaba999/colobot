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

// taskfire.cpp


#include <stdio.h>

#include "object/task/taskfire.h"

#include "graphics/common/particule.h"
#include "math/geometry.h"
#include "physics/physics.h"



const float ENERGY_FIRE 	= (0.25f/2.5f);	// energy consumed/shot
const float ENERGY_FIREr	= (0.25f/1.5f);	// energy consumed/ray
const float ENERGY_FIREi	= (0.10f/2.5f);	// energy consumed/organic


// Object's constructor.

CTaskFire::CTaskFire(CInstanceManager* iMan, CObject* object)
					 : CTask(iMan, object)
{
	m_soundChannel = -1;
}

// Object's destructor.

CTaskFire::~CTaskFire()
{
	if ( m_soundChannel != -1 )
	{
		m_sound->FlushEnvelope(m_soundChannel);
		m_sound->AddEnvelope(m_soundChannel, 0.0f, 1.0f, 1.0f, SOPER_STOP);
		m_soundChannel = -1;
	}
}


// Management of an event.

bool CTaskFire::EventProcess(const Event &event)
{
	CObject*	power;
	CPhysics*	physics;
	Math::Matrix*	mat;
	Math::Vector	pos, speed, dir, vib;
	ObjectType	type;
	Math::Point		dim;
	float		energy, fire;
	int			i, channel;

	if ( m_engine->RetPause() )  return true;
	if ( event.event != EVENT_FRAME )  return true;
	if ( m_bError )  return false;

	m_time += event.rTime;
	m_lastSound -= event.rTime;
	m_progress += event.rTime*m_speed;

	power = m_object->RetPower();
	if ( power != 0 )
	{
		energy = power->RetEnergy();
			 if ( m_bOrganic )  fire = ENERGY_FIREi;
		else if ( m_bRay     )  fire = ENERGY_FIREr;
		else                    fire = ENERGY_FIRE;
		energy -= event.rTime*fire/power->RetCapacity();
		power->SetEnergy(energy);
	}

	if ( m_lastParticule+0.05f <= m_time )
	{
		m_lastParticule = m_time;

		if ( m_bOrganic )
		{
			mat = m_object->RetWorldMatrix(1);  // insect-cannon

			for ( i=0 ; i<6 ; i++ )
			{
				pos = Math::Vector(0.0f, 2.5f, 0.0f);
				pos = Math::Transform(*mat, pos);

				speed = Math::Vector(200.0f, 0.0f, 0.0f);

				physics = m_object->RetPhysics();
				if ( physics != 0 )
				{
					speed += physics->RetLinMotion(MO_REASPEED);
				}

				speed.x += (Math::Rand()-0.5f)*10.0f;
				speed.y += (Math::Rand()-0.5f)*20.0f;
				speed.z += (Math::Rand()-0.5f)*30.0f;
				speed = Math::Transform(*mat, speed);
				speed -= pos;

				dim.x = Math::Rand()*0.5f+0.5f;
				dim.y = dim.x;

				channel = m_particule->CreateParticule(pos, speed, dim, PARTIGUN4, 0.8f, 0.0f, 0.0f);
				m_particule->SetObjectFather(channel, m_object);
			}
		}
		else if ( m_bRay )
		{
			mat = m_object->RetWorldMatrix(2);  // cannon

			for ( i=0 ; i<4 ; i++ )
			{
				pos = Math::Vector(4.0f, 0.0f, 0.0f);
				pos.y += (rand()%3-1)*1.5f;
				pos.z += (rand()%3-1)*1.5f;
				pos = Math::Transform(*mat, pos);

				speed = Math::Vector(200.0f, 0.0f, 0.0f);
				speed.x += (Math::Rand()-0.5f)*6.0f;
				speed.y += (Math::Rand()-0.5f)*12.0f;
				speed.z += (Math::Rand()-0.5f)*12.0f;
				speed = Math::Transform(*mat, speed);
				speed -= pos;

				dim.x = 1.0f;
				dim.y = dim.x;
				channel = m_particule->CreateTrack(pos, speed, dim, PARTITRACK11,
												   2.0f, 200.0f, 0.5f, 1.0f);
				m_particule->SetObjectFather(channel, m_object);

				speed = Math::Vector(5.0f, 0.0f, 0.0f);
				speed.x += (Math::Rand()-0.5f)*1.0f;
				speed.y += (Math::Rand()-0.5f)*2.0f;
				speed.z += (Math::Rand()-0.5f)*2.0f;
				speed = Math::Transform(*mat, speed);
				speed -= pos;
				speed.y += 5.0f;

				dim.x = 2.0f;
				dim.y = dim.x;
				m_particule->CreateParticule(pos, speed, dim, PARTISMOKE2, 2.0f, 0.0f, 0.5f);
			}
		}
		else
		{
			type = m_object->RetType();

			if ( type == OBJECT_MOBILErc )
			{
				mat = m_object->RetWorldMatrix(2);  // cannon
			}
			else
			{
				mat = m_object->RetWorldMatrix(1);  // cannon
			}

			for ( i=0 ; i<3 ; i++ )
			{
				if ( type == OBJECT_MOBILErc )
				{
					pos = Math::Vector(0.0f, 0.0f, 0.0f);
				}
				else
				{
					pos = Math::Vector(3.0f, 1.0f, 0.0f);
				}
				pos.y += (Math::Rand()-0.5f)*1.0f;
				pos.z += (Math::Rand()-0.5f)*1.0f;
				pos = Math::Transform(*mat, pos);

				speed = Math::Vector(200.0f, 0.0f, 0.0f);

				physics = m_object->RetPhysics();
				if ( physics != 0 )
				{
					speed += physics->RetLinMotion(MO_REASPEED);
				}

				speed.x += (Math::Rand()-0.5f)*3.0f;
				speed.y += (Math::Rand()-0.5f)*6.0f;
				speed.z += (Math::Rand()-0.5f)*6.0f;
				speed = Math::Transform(*mat, speed);
				speed -= pos;

				dim.x = Math::Rand()*0.7f+0.7f;
				dim.y = dim.x;

				channel = m_particule->CreateParticule(pos, speed, dim, PARTIGUN1, 0.8f, 0.0f, 0.0f);
				m_particule->SetObjectFather(channel, m_object);
			}

			if ( type != OBJECT_MOBILErc &&
				 m_progress > 0.3f )
			{
				pos = Math::Vector(-1.0f, 1.0f, 0.0f);
				pos.y += (Math::Rand()-0.5f)*0.4f;
				pos.z += (Math::Rand()-0.5f)*0.4f;
				pos = Math::Transform(*mat, pos);

				speed = Math::Vector(-4.0f, 0.0f, 0.0f);
				speed.x += (Math::Rand()-0.5f)*2.0f;
				speed.y += (Math::Rand()-0.2f)*4.0f;
				speed.z += (Math::Rand()-0.5f)*4.0f;
				speed = Math::Transform(*mat, speed);
				speed -= pos;

				dim.x = Math::Rand()*1.2f+1.2f;
				dim.y = dim.x;

				m_particule->CreateParticule(pos, speed, dim, PARTICRASH, 2.0f, 0.0f, 0.0f);
//?				m_particule->CreateParticule(pos, speed, dim, PARTISMOKE2, 4.0f, 0.0f, 0.0f);
			}
		}

		dir = Math::Vector(0.0f, 0.0f, 0.0f);
		if ( m_progress < 0.1f )
		{
			dir.z = (Math::PI*0.04f)*(m_progress*10.0f);
		}
		else if ( m_progress < 0.9f )
		{
			dir.z = (Math::PI*0.04f);
		}
		else
		{
			dir.z = (Math::PI*0.04f)*(1.0f-(m_progress-0.9f)*10.0f);
		}
		m_object->SetInclinaison(dir);

		vib.x = (Math::Rand()-0.5f)*0.01f;
		vib.y = (Math::Rand()-0.5f)*0.02f;
		vib.z = (Math::Rand()-0.5f)*0.02f;
		m_object->SetCirVibration(vib);

		vib.x = (Math::Rand()-0.5f)*0.20f;
		vib.y = (Math::Rand()-0.5f)*0.05f;
		vib.z = (Math::Rand()-0.5f)*0.20f;
		m_object->SetLinVibration(vib);
	}

	if ( m_bRay && m_lastSound <= 0.0f )
	{
		m_lastSound = Math::Rand()*0.4f+0.4f;
		m_sound->Play(SOUND_FIREp, m_object->RetPosition(0));
	}

	return true;
}


// Assigns the goal was achieved.

Error CTaskFire::Start(float delay)
{
	CObject*	power;
	Math::Vector	pos, goal, speed;
	float		energy, fire;
	ObjectType	type;

	m_bError = true;  // operation impossible

	type = m_object->RetType();
	if ( type != OBJECT_MOBILEfc &&
		 type != OBJECT_MOBILEtc &&
		 type != OBJECT_MOBILEwc &&
		 type != OBJECT_MOBILEic &&
		 type != OBJECT_MOBILEfi &&
		 type != OBJECT_MOBILEti &&
		 type != OBJECT_MOBILEwi &&
		 type != OBJECT_MOBILEii &&
		 type != OBJECT_MOBILErc )  return ERR_FIRE_VEH;

//?	if ( !m_physics->RetLand() )  return ERR_FIRE_FLY;

	speed = m_physics->RetMotorSpeed();
//?	if ( speed.x != 0.0f ||
//?		 speed.z != 0.0f )  return ERR_FIRE_MOTOR;

	m_bRay = (type == OBJECT_MOBILErc);

	m_bOrganic = false;
	if ( type == OBJECT_MOBILEfi ||
		 type == OBJECT_MOBILEti ||
		 type == OBJECT_MOBILEwi ||
		 type == OBJECT_MOBILEii )
	{
		m_bOrganic = true;
	}

	if ( delay == 0.0f )
	{
		if ( m_bRay )  delay = 1.2f;
		else           delay = 2.0f;
	}
	m_delay = delay;

	power = m_object->RetPower();
	if ( power == 0 )  return ERR_FIRE_ENERGY;
	energy = power->RetEnergy();
	     if ( m_bOrganic )  fire = m_delay*ENERGY_FIREi;
	else if ( m_bRay     )  fire = m_delay*ENERGY_FIREr;
	else                    fire = m_delay*ENERGY_FIRE;
	if ( energy < fire/power->RetCapacity()+0.05f )  return ERR_FIRE_ENERGY;
	
	m_speed = 1.0f/m_delay;
	m_progress = 0.0f;
	m_time = 0.0f;
	m_lastParticule = 0.0f;
	m_lastSound = 0.0f;
	m_bError = false;  // ok

//?	m_camera->StartCentering(m_object, Math::PI*0.15f, 99.9f, 0.0f, 1.0f);

	if ( m_bOrganic )
	{
		m_soundChannel = m_sound->Play(SOUND_FIREi, m_object->RetPosition(0), 1.0f, 1.0f, true);
		if ( m_soundChannel != -1 )
		{
			m_sound->AddEnvelope(m_soundChannel, 1.0f, 1.0f, m_delay, SOPER_CONTINUE);
			m_sound->AddEnvelope(m_soundChannel, 0.0f, 1.0f, 0.5f, SOPER_STOP);
		}
	}
	else if ( m_bRay )
	{
	}
	else
	{
		m_soundChannel = m_sound->Play(SOUND_FIRE, m_object->RetPosition(0), 1.0f, 1.0f, true);
		if ( m_soundChannel != -1 )
		{
			m_sound->AddEnvelope(m_soundChannel, 1.0f, 1.0f, m_delay, SOPER_CONTINUE);
			m_sound->AddEnvelope(m_soundChannel, 0.0f, 1.0f, 0.5f, SOPER_STOP);
		}
	}

	return ERR_OK;
}

// Indicates whether the action is finished.

Error CTaskFire::IsEnded()
{
	if ( m_engine->RetPause() )  return ERR_CONTINUE;
	if ( m_bError )  return ERR_STOP;
	if ( m_progress < 1.0f )  return ERR_CONTINUE;

	Abort();
	return ERR_STOP;
}

// Suddenly ends the current action.

bool CTaskFire::Abort()
{
	m_object->SetInclinaison(Math::Vector(0.0f, 0.0f, 0.0f));
	m_object->SetCirVibration(Math::Vector(0.0f, 0.0f, 0.0f));
	m_object->SetLinVibration(Math::Vector(0.0f, 0.0f, 0.0f));

	if ( m_soundChannel != -1 )
	{
		m_sound->FlushEnvelope(m_soundChannel);
		m_sound->AddEnvelope(m_soundChannel, 0.0f, 1.0f, 1.0f, SOPER_STOP);
		m_soundChannel = -1;
	}

//?	m_camera->StopCentering(m_object, 1.0f);
	return true;
}

