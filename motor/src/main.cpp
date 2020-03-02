#define _USE_MATH_DEFINES
#include <math.h>
#include "stdlib.h"
#ifdef __APPLE__
#include <GLUT/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>
#endif

#include "tinyxml2.h"
#include <iostream>
#include <string>
#include <vector>
#include "Model.h"
#include "Group.h"
#include "Light.h"
#include <map>

#define ANG2RAD M_PI/180.0
using namespace tinyxml2;

int g_time = 0;

// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// XZ position of the camera
float x = 40, z = 20.0f;
// angle of rotation for the camera direction
float angle = 0;

GLfloat amb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specularLight[3] = { 0.0f, 0.0f, 0.0f };

float cam_alpha = 0.0f;
float cam_beta = M_PI / 8;
float cam_radius = 100.0f;
std::vector<Model> models;
std::vector<Group*> groups;
std::vector<Light*> lights;
std::map<int, std::string> textures;
#define MAX_GL_LIGHTS 8

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// set the camera
	glLoadIdentity();
	/*
	gluLookAt(x, 0.0f, z,
		x + lx, 0.0f, z + lz,
		0.0f, 1.0f, 0.0f);
	*/

	/*
	gluLookAt(0, 100.0, 50.0,
		0.0, 0.0f, 0.0,
		0.0f, 1.0f, 0.0f);
	*/
	
	GLfloat r_ = cam_radius * cos(cam_beta);
	GLfloat pz = r_ * cos(cam_alpha);
	GLfloat px = r_ * sin(cam_alpha);
	GLfloat py = cam_radius * sin(cam_beta);

	gluLookAt(px,py,pz, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	

	// put drawing instructions here
	g_time = glutGet(GLUT_ELAPSED_TIME);

	/*
	// draw axis
	glBegin(GL_LINES);
	// draw x axis
	glColor3f(1, 0, 0);
	glVertex3f(100, 0, 0);
	glVertex3f(-100, 0, 0);

	// draw y axis
	glColor3f(0, 1, 0);
	glVertex3f(0, 100, 0);
	glVertex3f(0, -100, 0);

	// draw z axis
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 100);
	glVertex3f(0, 0, -100);
	glEnd();
	glColor3f(1, 1, 1);

	*/

	// Set Lights Positions/Directions

	for (int i = 0; i < lights.size(); i++)
	{
		Light* l = lights[i];
		switch (l->GetLightType())
		{
		case LIGHT_POINT:
		case LIGHT_DIRECTIONAL:
		{
			TLightPos pos = l->GetLightPosition();
			GLfloat fpos[4] = { pos.x, pos.y, pos.z, pos.w };
			glLightfv(GL_LIGHT0 + i, GL_POSITION, fpos);
			break;
		}
		case LIGHT_SPOT:
		{
			TLightPos pos = l->GetLightPosition();
			GLfloat fpos[4] = { pos.x, pos.y, pos.z, pos.w };
			glLightfv(GL_LIGHT0 + i, GL_POSITION, fpos);
			TLightDirection dir = l->GetSpotDirection();
			float spot_cutoff = l->GetSpotCutoff();
			GLfloat fdir[3] = { dir.x, dir.y, dir.z };
			glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, fdir);
			glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, spot_cutoff);
			glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, 1);
			// spot stuff 
			break;
		}
		default:
			std::cout << "Invalid light type" << std::endl;
			break;
		}
	}

	// Render models
	for (Model m : models)
	{
		m.Render(g_time);
	}



	// End of frame
	glutSwapBuffers();
}

void processKeys(unsigned char key, int xx, int yy) 
{
	float fraction = 1.f;

	switch (key) {
	case 'a':
		angle -= 0.04f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case 'd':
		angle += 0.04f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case 'w':
		x += lx * fraction;
		z += lz * fraction;
		break;
	case 's':
		x -= lx * fraction;
		z -= lz * fraction;
		break;
	}

	glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) 
{

	// put code to process special keys in here
	int deltaTime = (glutGet(GLUT_ELAPSED_TIME) - g_time);
	float movement = 0.5 * sin(0.001 * M_PI * deltaTime);
	switch (key)
	{
	case GLUT_KEY_UP:
		if (cam_beta > 1.2f) {
			break;
		}
		cam_beta += movement;
		break;
	case GLUT_KEY_DOWN:
		if (cam_beta < -1.2f) {
			break;
		}
		cam_beta -= movement;
		break;
	case GLUT_KEY_LEFT:
		cam_alpha -= movement;
		break;
	case GLUT_KEY_RIGHT:
		cam_alpha += movement;
		break;
	case GLUT_KEY_PAGE_UP:
		cam_radius -= movement * 50;
		break;
	case GLUT_KEY_PAGE_DOWN:
		cam_radius += movement * 50;
		break;
	}

	glutPostRedisplay();

}


void processMouseMovement(int button, int state, int x, int y)
{

}

void usage(const char* name)
{
	std::cout << "Usage: ./" << name << " <input_file.xml>" << std::endl;
}

bool parse_point(XMLElement* e, Point& p)
{
	const char* sX = e->Attribute("X");
	const char* sY = e->Attribute("Y");
	const char* sZ = e->Attribute("Z");

	if (!sX || !sY || !sZ)
	{
		return false;
	}

	p.x = std::stof(sX);

	p.y = std::stof(sY);

	p.z = std::stof(sZ);
	return true;
}

bool parse_points(XMLElement* elem, std::vector<Point>& points)
{
	for (XMLElement* e = elem->FirstChildElement("point"); e != NULL; e = e->NextSiblingElement("point"))
	{
		Point p;
		if (!parse_point(e, p))
		{
			std::cout << "Error parsing point tag!" << std::endl;
			return false;
		}

		points.push_back(p);
	}

	if (points.size() < 4)
	{
		std::cout << "Not enough points!" << std::endl;
		return false;
	}

	return true;
}

TTranslate* parse_translate_tag(XMLElement* e)
{
	TTranslate* t = new TTranslate;
	const char* sX = e->Attribute("X");
	const char* sY = e->Attribute("Y");
	const char* sZ = e->Attribute("Z");

	const char* time = e->Attribute("time");

	if ((!sX && !sY && !sZ) && !time)
	{
		t->type = GT_INVALID;
		return t;
	}

	if (time && sX && sY && sZ)
	{
		t->type = GT_INVALID;
		return t;
	}

	if (time)
	{
		t->time = std::stof(time);
		t->translation_type = TTranslate::DYNAMIC_TRANSLATE;
		if (!parse_points(e, t->translation_points))
		{
			t->type = GT_INVALID;
		}
		return t;
	}

	if (sX)
		t->x = std::stof(sX);

	if(sY)
		t->y = std::stof(sY);

	if(sZ)
		t->z = std::stof(sZ);
	return t;
}

TRotate* parse_rotate_tag(XMLElement* e)
{
	TRotate* r = new TRotate;
	const char* sX = e->Attribute("axisX");
	const char* sY = e->Attribute("axisY");
	const char* sZ = e->Attribute("axisZ");

	const char* angle = e->Attribute("angle");
	const char* time = e->Attribute("time");

	if ((!sX && !sY && !sZ) || (!angle && !time))
	{
		r->type = GT_INVALID;
		return r;
	}

	if (angle && time)
	{
		r->type = GT_INVALID;
		return r;
	}

	if (sX)
		r->x = std::stof(sX);

	if (sY)
		r->y = std::stof(sY);

	if (sZ)
		r->z = std::stof(sZ);
	if (angle)
		r->angle = std::stof(angle);
	else
		r->time = std::stof(time);
	
	return r;
}

TScale* parse_scale_tag(XMLElement* e)
{
	TScale* s = new TScale;
	const char* sX = e->Attribute("X");
	const char* sY = e->Attribute("Y");
	const char* sZ = e->Attribute("Z");

	if (!sX && !sY && !sZ)
	{
		s->type = GT_INVALID;
		return s;
	}

	if (sX)
		s->x = std::stof(sX);

	if (sY)
		s->y = std::stof(sY);

	if (sZ)
		s->z = std::stof(sZ);
	return s;
}

bool parse_models_tag(XMLElement* element, Group* parent)
{
	for (XMLElement* e = element->FirstChildElement("model"); e != NULL; e = e->NextSiblingElement("model"))
	{
		std::string model_file_name = e->Attribute("file");
		Model m(model_file_name);
		if (!m.Import())
			return false;
		m.SetGroup(parent);

		const char* texture = e->Attribute("texture");
		if (texture)
		{
			//m.SetTexture(texture);
			textures[models.size()] = texture;
		}

		const char* diffR = e->Attribute("diffR");
		const char* diffG = e->Attribute("diffG");
		const char* diffB = e->Attribute("diffB");
		const char* diffA = e->Attribute("diffA");

		if (diffR)
		{
			if (!diffG || !diffB)
			{
				std::cout << "Invalid diffuse values";
				return false;
			}

			float fR = std::stof(std::string(diffR));
			float fG = std::stof(std::string(diffG));
			float fB = std::stof(std::string(diffB));

			float fA = 1.0f;
			if (diffA)
			{
				fA = std::stof(std::string(diffA));
			}
			m.SetDiffuseColors(fR, fG, fB, fA);
		}

		const char* ambientR = e->Attribute("ambR");
		const char* ambientG = e->Attribute("ambG");
		const char* ambientB = e->Attribute("ambB");
		const char* ambientA = e->Attribute("ambA");

		if (ambientR)
		{
			if (!ambientG || !ambientB)
			{
				std::cout << "Invalid ambient values";
				return false;
			}

			float fR = std::stof(std::string(ambientR));
			float fG = std::stof(std::string(ambientG));
			float fB = std::stof(std::string(ambientB));

			float fA = 1.0f;
			if (ambientA)
			{
				fA = std::stof(std::string(ambientA));
			}
			m.SetAmbientColors(fR, fG, fB, fA);
		}

		const char* specularR = e->Attribute("specR");
		const char* specularG = e->Attribute("specG");
		const char* specularB = e->Attribute("specB");
		const char* specularA = e->Attribute("specA");

		if (specularR)
		{
			if (!specularG || !specularB)
			{
				std::cout << "Invalid specular values";
				return false;
			}

			float fR = std::stof(std::string(specularR));
			float fG = std::stof(std::string(specularG));
			float fB = std::stof(std::string(specularB));

			float fA = 1.0f;
			if (specularA)
			{
				fA = std::stof(std::string(specularA));
			}
			m.SetSpecularColors(fR, fG, fB, fA);
		}

		const char* emissionR = e->Attribute("emissR");
		const char* emissionG = e->Attribute("emissG");
		const char* emissionB = e->Attribute("emissB");
		const char* emissionA = e->Attribute("emissA");

		if (emissionR)
		{
			if (!emissionG || !emissionB)
			{
				std::cout << "Invalid specular values";
				return false;
			}

			float fR = std::stof(std::string(emissionR));
			float fG = std::stof(std::string(emissionG));
			float fB = std::stof(std::string(emissionB));

			float fA = 1.0f;
			if (emissionA)
			{
				fA = std::stof(std::string(emissionA));
			}
			m.SetEmissiveColors(fR, fG, fB, fA);
		}

		models.push_back(m);
	}
	return true;
}

bool parse_group_tag(XMLElement* elem, Group* parent_group, Group* new_group)
{
	new_group->SetParentGroup(parent_group);
	for (XMLElement* e = elem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		std::string tag_name = e->Value();
		if (!tag_name.compare("translate"))
		{
			TTranslate* t = parse_translate_tag(e);
			if (t->type == GT_INVALID)
				return false;
			new_group->AppendGeoTransform(t);
		}
		else if (!tag_name.compare("rotate"))
		{
			TRotate* r = parse_rotate_tag(e);
			if (r->type == GT_INVALID)
				return false;
			new_group->AppendGeoTransform(r);
		}
		else if (!tag_name.compare("scale"))
		{
			TScale* s = parse_scale_tag(e);
			if (s->type == GT_INVALID)
				return false;
			new_group->AppendGeoTransform(s);
		}
		else if (!tag_name.compare("models"))
		{
			if(!parse_models_tag(e, new_group))
				return false;
		}
		else if (!tag_name.compare("group"))
		{
			Group* n = new Group;
			if (!parse_group_tag(e, new_group, n))
				return false;
			groups.push_back(n);
		}
	}
	return true;
}

bool parse_light_tag(XMLElement* e, Light* light)
{
	const char* light_type_str = e->Attribute("type");
	if (!light_type_str)
		return false;

	std::string light_type = light_type_str;
	if (light_type == "POINT")
	{
		// point light
		TLightPos pos;
		light->SetLightType(LIGHT_POINT);
		const char* sX = e->Attribute("posX");
		const char* sY = e->Attribute("posY");
		const char* sZ = e->Attribute("posZ");

		if (sX)
			pos.x = std::stof(sX);

		if (sY)
			pos.y = std::stof(sY);

		if (sZ)
			pos.z = std::stof(sZ);

		pos.w = 1.0f;
		light->SetLightPosition(pos);

	}
	else if (light_type == "DIRECTIONAL")
	{
		// directional light
		light->SetLightType(LIGHT_DIRECTIONAL);
		TLightPos dir;
		const char* sX = e->Attribute("dirX");
		const char* sY = e->Attribute("dirY");
		const char* sZ = e->Attribute("dirZ");

		if (sX)
			dir.x = std::stof(sX);

		if (sY)
			dir.y = std::stof(sY);

		if (sZ)
			dir.z = std::stof(sZ);

		dir.w = 0.0f;
		light->SetLightPosition(dir);

	}
	else if (light_type == "SPOT")
	{
		// spotlight
		light->SetLightType(LIGHT_SPOT);

		TLightPos pos;
		const char* sX = e->Attribute("posX");
		const char* sY = e->Attribute("posY");
		const char* sZ = e->Attribute("posZ");

		if (sX)
			pos.x = std::stof(sX);

		if (sY)
			pos.y = std::stof(sY);

		if (sZ)
			pos.z = std::stof(sZ);

		pos.w = 1.0f;
		light->SetLightPosition(pos);

		// spot parameters
		TLightPos dir;
		sX = e->Attribute("dirX");
		sY = e->Attribute("dirY");
		sZ = e->Attribute("dirZ");

		if (sX)
			dir.x = std::stof(sX);

		if (sY)
			dir.y = std::stof(sY);

		if (sZ)
			dir.z = std::stof(sZ);

		dir.w = 0.0f;
		light->SetSpotDirection(dir);

		const char* spotCutoff = e->Attribute("cutoff");
		if (!spotCutoff)
			return false;

		float angle = std::stof(spotCutoff);
		light->SetSpotCutoff(angle);
	}
	else
	{
		std::cout << "Invalid light type: " << light_type_str << std::endl;
		return false;
	}
	return true;
}

bool parse_lights_tag(XMLElement* elem)
{
	for (XMLElement* e = elem->FirstChildElement("light"); e != NULL; e = e->NextSiblingElement("light"))
	{
		// Found a light tag 
		Light* l = new Light;
		if (!parse_light_tag(e, l))
		{
			return false;
		}
		lights.push_back(l);
	}
	return true;
}

bool parse_xml_file(XMLDocument& xml_doc)
{
	tinyxml2::XMLNode* root = xml_doc.FirstChild();
	if (root == nullptr) return false;

	tinyxml2::XMLElement* element = xml_doc.FirstChildElement("scene");
	if (element == nullptr) return false;
	Group* rootGroup = groups[0];
	Group* currentGroup = rootGroup; // root group

	tinyxml2::XMLElement* lights_element = element->FirstChildElement("lights");
	if (lights_element != nullptr) {
		if (!parse_lights_tag(lights_element))
		{
			std::cout << "Error parsing lights tag" << std::endl;
			return false;
		}
	}

	for (XMLElement* e = element->FirstChildElement("group"); e != NULL; e = e->NextSiblingElement("group"))
	{
		// Found a group tag 
		Group* n = new Group;
		if (!parse_group_tag(e, currentGroup, n))
		{
			return false;
		}
		groups.push_back(n);
	}

	XMLElement* e = element->FirstChildElement("models");
	if (e != nullptr)
	{
		parse_models_tag(e, rootGroup);
	}
	return true;
}

void idleFunc(void)
{
	
}

void initLighting()
{
	GLfloat amb[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat diff[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[3] = { 0.0f, 0.0f, 0.0f };
	// light colors

	glEnable(GL_LIGHTING);

	for (int i = 0; i < lights.size() && i < MAX_GL_LIGHTS; i++)
	{
		glEnable(GL_LIGHT0 + i);
		//glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		//glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
		//glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	}

}

int main(int argc, char **argv) 
{
	if (argc == 1)
	{
		usage(argv[0]);
		return 0;
	}

	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);


	const char* xml_file_name = argv[1];

	// Root Group
	Group* root_group = new Group;
	groups.push_back(root_group); 

	tinyxml2::XMLDocument xml;
	int errorID = xml.LoadFile(xml_file_name);
	std::string errorStr = xml.ErrorStr();
	if (!parse_xml_file(xml))
	{
		std::cout << "Error parsing xml file!" << std::endl;
		return -1;
	}

	// put GLUT init here
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Motor");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error: %s\n", glewGetErrorString(err));
	}

	// assign vbos to each model
	size_t n = models.size();
	std::vector<GLuint> temp_vbos(3 * n);
	glGenBuffers(3 * n, temp_vbos.data());

	for (auto i = 0u; i < n; i++)
	{
		models[i].SetVBOs(temp_vbos[i * 3], temp_vbos[i * 3 + 1], temp_vbos[i * 3 + 2]);
	}

	// put callback registration here
	glutDisplayFunc(renderScene);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(changeSize);
	//glutIdleFunc(renderScene);

	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(processMouseMovement);


	// OpenGL settings 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);

	initLighting();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClearColor(0.0f,0.0f,0.0f,0.0f);

	// equivalente ao loadTexture para todos os modelos
	for (auto iter = textures.begin(); iter != textures.end(); ++iter)
	{ 
		models[iter->first].SetTexture(iter->second);
	}

	// enter GLUT's main loop
	glutMainLoop();
	
	for (auto group : groups)
	{
		delete group;
	}

	for (auto light : lights)
	{
		delete light;
	}
	return 1;
}

