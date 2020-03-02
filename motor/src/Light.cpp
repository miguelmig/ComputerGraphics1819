#include "Light.h"
#ifdef __APPLE__
#include <GLUT/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

void Light::SetLightType(ELightTypes type)
{
	m_lightType = type;
}

ELightTypes Light::GetLightType() const
{
	return m_lightType;
}

TLightPos Light::GetLightPosition()
{
	TLightPos ret = m_light_pos;
	if (m_lightType == LIGHT_DIRECTIONAL)
	{
		// negate direction
		ret.x = -ret.x;
		ret.y = -ret.y;
		ret.z = -ret.z;
	}

	return ret;
}

void Light::SetLightPosition(TLightPos pos)
{
	m_light_pos = pos;
	if (m_lightType == LIGHT_DIRECTIONAL)
	{
		m_light_pos.w = 0.0f;
	}
	else
	{
		m_light_pos.w = 1.0f;
	}
}

void Light::SetSpotDirection(TLightDirection dir)
{
	m_spot_direction = dir;
}

TLightDirection Light::GetSpotDirection()
{
	return m_spot_direction;
}

void Light::SetSpotCutoff(float angle)
{
	m_spot_cutoff = angle;
}

float Light::GetSpotCutoff() const
{
	return m_spot_cutoff;
}