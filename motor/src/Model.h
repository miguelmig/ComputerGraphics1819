#pragma once
#include <string>
#include <vector>
#include "Group.h"

#ifdef __APPLE__
#include <GLUT/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

class Model
{
	typedef struct SVertexData
	{
		float x, y, z;
	} TVertexData, TNormalData;

	typedef struct STexCoordData
	{
		float u, v;
	} TTexCoordData;

	typedef struct SColorData
	{
		float r, g, b, a;
	} TColorData;

public:
	Model(std::string file_name);
	void SetGroup(Group* group);
	void SetVBOs(GLuint vertices, GLuint normals, GLuint texcoords);
	bool SetTexture(std::string texture_file);

	void SetDiffuseColors(float red, float green, float blue, float alpha=1.0f);
	void SetAmbientColors(float red, float green, float blue, float alpha=1.0f);
	void SetSpecularColors(float red, float green, float blue, float alpha = 1.0f);
	void SetEmissiveColors(float red, float green, float blue, float alpha = 1.0f);

	
	virtual void Render(int global_time);
	virtual bool Import();


	
protected:
	std::string m_sFilename;
	std::vector<TVertexData> m_vertices;
	std::vector<TNormalData> m_normals;
	std::vector<TTexCoordData> m_texcoords;

	Group* m_group;

	GLuint m_vbo_vertex;
	GLuint m_vbo_normals;
	GLuint m_vbo_texcoords;

	unsigned int m_texture;
	bool m_using_texture;

	TColorData m_diffuse;
	bool m_has_diffuse_color;

	TColorData m_ambient;
	bool m_has_ambient_color;

	TColorData m_specular;
	bool m_has_specular_color;

	TColorData m_emissive;
	bool m_has_emissive_color;


};

