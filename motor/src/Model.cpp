#include "Model.h"
#include <fstream>
#include <string>
#include "utils.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <IL/il.h>

Model::Model(std::string filename)
{
	m_sFilename = filename;
	m_using_texture = false;

	m_has_diffuse_color = false;
	m_has_ambient_color = false;
	m_has_specular_color = false;
	m_has_emissive_color = false;

}

void Model::SetGroup(Group * group)
{
	m_group = group;
}

void Model::SetVBOs(GLuint vertices, GLuint normals, GLuint texcoords)
{
	m_vbo_vertex = vertices;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertex);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(TVertexData), m_vertices.data(), GL_STATIC_DRAW);

	if (!m_normals.empty())
	{
		m_vbo_normals = normals;
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(TNormalData), m_normals.data(), GL_STATIC_DRAW);
	}

	if (!m_texcoords.empty())
	{
		m_vbo_texcoords = texcoords;
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, m_texcoords.size() * sizeof(TTexCoordData), m_texcoords.data(), GL_STATIC_DRAW);
	}
}

bool Model::SetTexture(std::string texture_file)
{
	m_using_texture = true;
	ILuint ima;
	ilGenImages(1, &ima); 
	ilBindImage(ima);
	const char* tex = texture_file.c_str();
	ilLoadImage((ILstring)tex);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	unsigned char *imageData = ilGetData();

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	return true;
}

void Model::SetDiffuseColors(float red, float green, float blue, float alpha)
{
	m_has_diffuse_color = true;
	m_diffuse.r = red;
	m_diffuse.g = green;
	m_diffuse.b = blue;
	m_diffuse.a = alpha;
}

void Model::SetAmbientColors(float red, float green, float blue, float alpha)
{
	m_has_ambient_color = true;
	m_ambient.r = red;
	m_ambient.g = green;
	m_ambient.b = blue;
	m_ambient.a = alpha;
}

void Model::SetSpecularColors(float red, float green, float blue, float alpha)
{
	m_has_specular_color = true;
	m_specular.r = red;
	m_specular.g = green;
	m_specular.b = blue;
	m_specular.a = alpha;
}

void Model::SetEmissiveColors(float red, float green, float blue, float alpha)
{
	m_has_emissive_color = true;
	m_emissive.r = red;
	m_emissive.g = green;
	m_emissive.b = blue;
	m_emissive.a = alpha;
}

void matrixMultiplication(float N[4][4], float M[4][4], float Result[4][4])
{
	for (int I = 0; I < 4; ++I)
	{
		for (int J = 0; J < 4; ++J)
		{
			float SumElements = 0.0f;
			for (int K = 0; K < 4; ++K)
			{
				SumElements += M[I][K] * N[K][J];
			}
			Result[I][J] = SumElements;
		}
	}
}


void multMatrixVector(float m[4][4], float v[4], float res[4]) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[k][j];
		}
	}
}


void getCatmullRomPoint(float t, float *p0, float *p1, float *p2, float *p3, float *pos, float *deriv) {

	// catmull-rom matrix
	float m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f} };

	float quad = t * t;
	float cubo = t * t * t;
	
	float P[4][4] = { {p0[0], p0[1], p0[2], p0[3]},
		{p1[0], p1[1], p1[2], p1[3]} ,
		{p2[0], p2[1], p2[2], p2[3]} ,
		{p3[0], p3[1], p3[2], p3[3]}
	};
	
	
	float C[4];
	float T[4] = { cubo, quad, t, 1 };
	
	// C = m * T
	multMatrixVector(m, T, C);

	// pos = C * P
	multMatrixVector(P, C, pos);

	float D[4];
	float Tdash[4] = { 3 * quad,  2 * t, 1, 0 };

	// D = m * T'
	multMatrixVector(m, Tdash, D);

	// deriv = D * P 
	multMatrixVector(P, D, deriv);
}

// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, std::vector<Point> p, float *pos, float *deriv) {

	size_t POINT_COUNT = p.size();
	float t = gt * POINT_COUNT; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4];
	indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
	indices[1] = (indices[0] + 1) % POINT_COUNT;
	indices[2] = (indices[1] + 1) % POINT_COUNT;
	indices[3] = (indices[2] + 1) % POINT_COUNT;

	float* ps[4] = {
		p[indices[0]].toVector(),
		p[indices[1]].toVector(),
		p[indices[2]].toVector(),
		p[indices[3]].toVector(),
	};

	getCatmullRomPoint(t, ps[0], ps[1], ps[2], ps[3], pos, deriv);
}


void Model::Render(int g_time)
{
	glPushMatrix();
	std::vector<Group*> local_groups;

	for (Group* g = m_group; g != nullptr; g = g->GetParentGroup())
	{
		local_groups.push_back(g);
	}

	for(auto riter = local_groups.rbegin(); riter != local_groups.rend(); ++riter)
	{
		Group* g = *riter;

		std::vector<TGeoTransform*> geo_transforms = g->GetGeoTransforms();
		for(auto geot : geo_transforms)
		{
			switch (geot->type)
			{
			case GT_TRANSLATE:
			{
				TTranslate* trans = reinterpret_cast<TTranslate*>(geot);
				if (trans->translation_type == TTranslate::DYNAMIC_TRANSLATE)
				{
					float pct_t = (g_time / 1000.0f) / trans->time;
					if (pct_t > 1.0f)
					{
						g_time = fmod(g_time / 1000.0f, trans->time) * 1000;
						pct_t = (g_time / 1000.0f) / trans->time;
						//pct_t = 1.0f;
					}

					float pos[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
					float deriv[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
					getGlobalCatmullRomPoint(pct_t, trans->translation_points, pos, deriv);
					glTranslatef(pos[0], pos[1], pos[2]);
				}
				else
				{
					glTranslatef(trans->x, trans->y, trans->z);
				}
				break;
			}
			case GT_ROTATE:
			{
				TRotate* rot = reinterpret_cast<TRotate*>(geot);
				if (rot->time)
				{
					float pct_t = (g_time/1000.0f) / rot->time;
					if (pct_t > 1.0f)
					{
						g_time = fmod(g_time / 1000.0f, rot->time) * 1000;
						pct_t = (g_time / 1000.0f) / rot->time;
					}

					glRotatef(pct_t * 360, rot->x, rot->y, rot->z);
				}
				else
				{
					glRotatef(rot->angle, rot->x, rot->y, rot->z);
				}
				break;
			}
			case GT_SCALE:
			{
				TScale* scale = reinterpret_cast<TScale*>(geot);
				glScalef(scale->x, scale->y, scale->z);
				break;
			}
			}
		}
	}

	/*
	// Imediate mode
	glBegin(GL_TRIANGLES);
	for (auto vertex : m_vertices)
	{
		glVertex3f(vertex.x, vertex.y, vertex.z);
	}
	glEnd();
	*/

	//glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	//glMaterialfv(GL_FRONT, GL_EMISSION, white);

	// VBO mode

	if (m_has_diffuse_color)
	{
		float diffuse[4] = { m_diffuse.r,
			m_diffuse.g,
			m_diffuse.b,
			m_diffuse.a };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	}
	else
	{
		GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	}

	if (m_has_ambient_color)
	{
		float ambient[4] = { m_ambient.r,
			m_ambient.g,
			m_ambient.b,
			m_ambient.a };
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	}
	else
	{
		GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	}

	if (m_has_specular_color)
	{
		float specular[4] = { m_specular.r,
			m_specular.g,
			m_specular.b,
			m_specular.a };
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	}

	if (m_has_emissive_color)
	{
		float emissive[4] = { m_emissive.r,
			m_emissive.g,
			m_emissive.b,
			m_emissive.a };
		glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
	}

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertex);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	// Normals
	if (!m_normals.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		glNormalPointer(GL_FLOAT, 0, 0);
	}

	if (!m_texcoords.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoords);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}

	if (m_using_texture)
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}

	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glutPostRedisplay();

}

bool Model::Import()
{
	if (m_sFilename.empty())
		return false;

	std::ifstream vertex_file;
	vertex_file.open(m_sFilename.c_str(), std::ios::in);

	std::string vertex_number_str;
	int vertices;

	std::getline(vertex_file, vertex_number_str);
	vertices = std::stoi(vertex_number_str);

	std::string vertex_line;
	enum ELineType {
		VERTEX,
		NORMAL,
		TEXCOORD
	};

	ELineType current_line_type = VERTEX;
	while (std::getline(vertex_file, vertex_line))
	{
		if (vertex_line == "normals")
		{
			current_line_type = NORMAL;
			continue;
		}
		else if (vertex_line == "textures")
		{
			current_line_type = TEXCOORD;
			continue;
		}

		switch (current_line_type)
		{
		case VERTEX:
		{
			float x, y, z;
			std::vector<std::string> tokens = split(vertex_line, " ");
			if (tokens.size() != 3)
			{
				return true;
			}
			x = std::stof(tokens[0]);
			y = std::stof(tokens[1]);
			z = std::stof(tokens[2]);

			m_vertices.push_back(TVertexData{ x,y,z });
			break;
		}
		case NORMAL:
		{
			float x, y, z;
			std::vector<std::string> tokens = split(vertex_line, " ");
			if (tokens.size() != 3)
			{
				return true;
			}
			x = std::stof(tokens[0]);
			y = std::stof(tokens[1]);
			z = std::stof(tokens[2]);

			m_normals.push_back(TNormalData{ x,y,z });
			break;
		}
		case TEXCOORD:
		{
			float u,v;
			std::vector<std::string> tokens = split(vertex_line, " ");
			if (tokens.size() != 2)
			{
				return true;
			}
			u = std::stof(tokens[0]);
			v = std::stof(tokens[1]);

			m_texcoords.push_back(TTexCoordData{ u,v });
			break;
		}
		}
	}
	return true;
}
