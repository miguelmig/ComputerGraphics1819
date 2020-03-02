#pragma once

enum ELightTypes
{
	LIGHT_INVALID,
	LIGHT_POINT,
	LIGHT_DIRECTIONAL,
	LIGHT_SPOT,
};

typedef struct SVec4
{
	float x, y, z;
	float w; // point or direction (?)

	SVec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
} TLightPos, TLightDirection;

class Light
{
public:
	void SetLightType(ELightTypes type);
	ELightTypes GetLightType() const;
	TLightPos GetLightPosition();
	void SetLightPosition(TLightPos pos);
	void SetSpotDirection(TLightDirection dir);
	TLightDirection GetSpotDirection();
	float GetSpotCutoff() const;
	void SetSpotCutoff(float angle);
private:
	ELightTypes m_lightType;
	TLightPos m_light_pos;
	TLightDirection m_spot_direction;
	float m_spot_cutoff;
};