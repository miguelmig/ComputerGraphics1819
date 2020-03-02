#pragma once
#include <string>
#include <queue>
enum EGeometricTransformation
{
	GT_TRANSLATE,
	GT_ROTATE,
	GT_SCALE,
	GT_INVALID,
};

class TGeoTransform
{
public:
	TGeoTransform() : x(0.0f), y(0.0f), z(0.0f), type(GT_INVALID) {}
	EGeometricTransformation type;
	float x, y, z;
};

struct Point
{
	float x, y, z;
	Point() : x(0), y(0), z(0) {
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		vec[3] = 1;
	}

	Point(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		vec[3] = 1;
	}

	float* toVector() {
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		vec[3] = 1;
		return vec;
	};

	float vec[4];
};

class TTranslate : public TGeoTransform
{
public:
	enum TranslateType
	{
		STATIC_TRANSLATE,
		DYNAMIC_TRANSLATE
	};

	TTranslate() { 
		type = GT_TRANSLATE; 
		translation_type = STATIC_TRANSLATE;
		time = 0.0f;
	};

	std::vector< Point > translation_points;
	float time;
	TranslateType translation_type;
};

class TRotate : public TGeoTransform
{
public:
	float angle;
	float time;
	TRotate() {
		type = GT_ROTATE;
		angle = 0.0f; 
		time = 0.0f;
	}
};

class TScale : public TGeoTransform
{
public:
	TScale() {
		type = GT_SCALE;
	};
};

class Group
{
public:
	Group();
	~Group();
	
	void SetParentGroup(Group* parent);
	void AppendGeoTransform(TGeoTransform* transform);
	size_t GetGeoTransformCount() const;
	std::vector<TGeoTransform*> GetGeoTransforms();
	Group* GetParentGroup() const;

protected:
	Group* m_pParentGroup;
	std::vector<TGeoTransform*> m_transformations;

};

