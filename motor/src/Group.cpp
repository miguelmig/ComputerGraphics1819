#include "Group.h"
#include <fstream>
#include <string>
#include "utils.h"
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

Group::Group() : m_pParentGroup(nullptr)
{

}

Group::~Group()
{
	for(auto transf : m_transformations)
	{
		delete transf;
	}
}

void Group::SetParentGroup(Group* parent)
{
	m_pParentGroup = parent;
}

size_t Group::GetGeoTransformCount() const
{
	return m_transformations.size();
}

void Group::AppendGeoTransform(TGeoTransform* transform)
{
	m_transformations.push_back(transform);
}

std::vector<TGeoTransform*> Group::GetGeoTransforms()
{
	std::vector<TGeoTransform*> ret(m_transformations);
	return ret;
}

Group * Group::GetParentGroup() const
{
	return m_pParentGroup;
}
