#define GL_SILENCE_DEPRECATION

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

struct Point2D {
    float x;
    float y;
    int slice;
    int stack;
    
    Point2D(float x, float y) {
        this->x = x;
        this->y = y;
        this->slice = -1;
        this->stack = -1;
    }
    
    Point2D(float x, float y, int slice, int stack) {
        this->x = x;
        this->y = y;
        this->slice = slice;
        this->stack = stack;
    }
    
    string toString() {
        return to_string(this->x) + " "
             + to_string(this->y)
             + "slice = " + to_string(this->slice);
    }
    
    
};

struct Point {
    float x;
    float y;
    float z;
    
    Point(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    string toString() {
        return to_string(this->x) + " "
        + to_string(this->y) + " "
        + to_string(this->z);
    }
    
    float norm() {
        return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    }
    
    Point normalize() {
        float norm = this->norm();
        
        return Point(-this->x / norm, -this->y / norm, -this->z / norm);
    }
    
};


struct Patch {
    vector<int> indices;
};


Point crossProduct(Point a, Point b) {
    return Point(a.y * b.z - a.z * b.y,
                 a.z * b.x - a.x * b.z,
                 a.x * b.y - a.y * b.x);
}

// plano
void outputPlaneVertices(float size, const char* fildes) {
    float aux = size / 2.0f;
    FILE* f = fopen(fildes, "w");
    
    fprintf(f, "%d\n", 6);
    
    float px1 = aux;
    float pz1 = aux;
    
    float px2 = aux;
    float pz2 = -aux;
    
    float px3 = -aux;
    float pz3 = -aux;
    
    float px4 = -aux;
    float pz4 = aux;
    
    fprintf(f, "%f %f %f\n", px1, 0.f, pz1);
    fprintf(f, "%f %f %f\n", px2, 0.f, pz2);
    fprintf(f, "%f %f %f\n", px3, 0.f, pz3);
    
    fprintf(f, "%f %f %f\n", px1, 0.f, pz1);
    fprintf(f, "%f %f %f\n", px3, 0.f, pz3);
    fprintf(f, "%f %f %f\n", px4, 0.f, pz4);
    
    // normais
    fprintf(f, "normals\n");
    
    fprintf(f, "%f %f %f\n", 0.f, 1.f, 0.f);
    fprintf(f, "%f %f %f\n", 0.f, 1.f, 0.f);
    fprintf(f, "%f %f %f\n", 0.f, 1.f, 0.f);
    
    fprintf(f, "%f %f %f\n", 0.f, 1.f, 0.f);
    fprintf(f, "%f %f %f\n", 0.f, 1.f, 0.f);
    fprintf(f, "%f %f %f\n", 0.f, 1.f, 0.f);
    
    // texturas
    fprintf(f, "textures\n");
    
    fprintf(f, "%f %f\n", 1.f, 0.f);
    fprintf(f, "%f %f\n", 1.f, 1.f);
    fprintf(f, "%f %f\n", 0.f, 1.f);
    
    fprintf(f, "%f %f\n", 1.f, 0.f);
    fprintf(f, "%f %f\n", 0.f, 1.f);
    fprintf(f, "%f %f\n", 0.f, 0.f);
    
    
    fclose(f);
}

bool parsePlane(int argc, char** argv) {
    if (argc != 4) {
        return false;
    }
    
    float size = atof(argv[2]);
    
    outputPlaneVertices(size, argv[3]);
    return true;
}

// Caixa
void outputBoxVertices(float x, float y, float z, const char* fildes, int divs = 1) {
    float step_x = x / divs;
    float step_y = y / divs;
    float step_z = z / divs;
    
    vector<Point> normals;
    
    // limites para centrar a caixa na origem
    float xc = x / 2;
    float zc = z / 2;
    
    float x_, y_, z_;
    int line, col;
    
    FILE* f = fopen(fildes, "w");
    
    fprintf(f, "%d\n", 36 * divs * divs);

    
    // planos z
    y_ = 0.0f;
    for(line = 0; line < divs; line++) {
        x_ = -xc;
        
        for(col = 0; col < divs; col++) {
            // frente (z = zc) - mão direita
            fprintf(f, "%f %f %f\n", x_, y_, zc);
            fprintf(f, "%f %f %f\n", x_ + step_x, y_, zc);
            fprintf(f, "%f %f %f\n", x_ + step_x, y_ + step_y, zc);
            
            fprintf(f, "%f %f %f\n", x_, y_, zc);
            fprintf(f, "%f %f %f\n", x_ + step_x, y_ + step_y, zc);
            fprintf(f, "%f %f %f\n", x_, y_ + step_y, zc);
            
            
            normals.push_back(Point(0,0,1));
            normals.push_back(Point(0,0,1));
            normals.push_back(Point(0,0,1));
            
            normals.push_back(Point(0,0,1));
            normals.push_back(Point(0,0,1));
            normals.push_back(Point(0,0,1));
            
            
            // trás (z = -zc) - mão esquerda
            fprintf(f, "%f %f %f\n", x_, y_, -zc);
            fprintf(f, "%f %f %f\n", x_ + step_x, y_ + step_y, -zc);
            fprintf(f, "%f %f %f\n", x_ + step_x, y_, -zc);
            
            fprintf(f, "%f %f %f\n", x_, y_, -zc);
            fprintf(f, "%f %f %f\n", x_, y_ + step_y, -zc);
            fprintf(f, "%f %f %f\n", x_ + step_x, y_ + step_y, -zc);
            
            
            normals.push_back(Point(0,0,-1));
            normals.push_back(Point(0,0,-1));
            normals.push_back(Point(0,0,-1));
            
            normals.push_back(Point(0,0,-1));
            normals.push_back(Point(0,0,-1));
            normals.push_back(Point(0,0,-1));
            
            
            x_ += step_x;
        }
        
        y_ += step_y;
    }
    
    // planos x
    y_ = 0.0f;
    for(line = 0; line < divs; line++) {
        z_ = zc;
        
        for(col = 0; col < divs; col++) {
            // direita (x = xc) - mão direita
            fprintf(f, "%f %f %f\n", xc, y_, z_);
            fprintf(f, "%f %f %f\n", xc, y_, z_ - step_z);
            fprintf(f, "%f %f %f\n", xc, y_ + step_y, z_ - step_z);
            
            fprintf(f, "%f %f %f\n", xc, y_, z_);
            fprintf(f, "%f %f %f\n", xc, y_ + step_y, z_ - step_z);
            fprintf(f, "%f %f %f\n", xc, y_ + step_y, z_);
            
            
            normals.push_back(Point(1,0,0));
            normals.push_back(Point(1,0,0));
            normals.push_back(Point(1,0,0));
            
            normals.push_back(Point(1,0,0));
            normals.push_back(Point(1,0,0));
            normals.push_back(Point(1,0,0));
            
            
            // esquerda (x = -xc) - mão esquerda
            fprintf(f, "%f %f %f\n", -xc, y_, z_);
            fprintf(f, "%f %f %f\n", -xc, y_ + step_y, z_ - step_z);
            fprintf(f, "%f %f %f\n", -xc, y_, z_ - step_z);
            
            fprintf(f, "%f %f %f\n", -xc, y_, z_);
            fprintf(f, "%f %f %f\n", -xc, y_ + step_y, z_);
            fprintf(f, "%f %f %f\n", -xc, y_ + step_y, z_ - step_z);
            
            
            normals.push_back(Point(-1,0,0));
            normals.push_back(Point(-1,0,0));
            normals.push_back(Point(-1,0,0));
            
            normals.push_back(Point(-1,0,0));
            normals.push_back(Point(-1,0,0));
            normals.push_back(Point(-1,0,0));
            
            
            z_ -= step_z;
        }
        
        y_ += step_y;
    }

    // planos y
    z_ = -zc;
    for(line = 0; line < divs; line++) {
        x_ = -xc;
        for(col = 0; col < divs; col++) {
            // cima (y = y) - mão direita
            fprintf(f, "%f %f %f\n", x_, y, z_ + step_z);
            fprintf(f, "%f %f %f\n", x_ + step_x, y, z_ + step_z);
            fprintf(f, "%f %f %f\n", x_ + step_x, y, z_);
            
            fprintf(f, "%f %f %f\n", x_, y, z_ + step_z);
            fprintf(f, "%f %f %f\n", x_ + step_x, y, z_);
            fprintf(f, "%f %f %f\n", x_, y, z_);
            
            
            normals.push_back(Point(0,1,0));
            normals.push_back(Point(0,1,0));
            normals.push_back(Point(0,1,0));
            
            normals.push_back(Point(0,1,0));
            normals.push_back(Point(0,1,0));
            normals.push_back(Point(0,1,0));
            
            
            // baixo (y = 0) - mão esquerda
            fprintf(f, "%f %f %f\n", x_, 0.f, z_ + step_z);
            fprintf(f, "%f %f %f\n", x_ + step_x, 0.f, z_);
            fprintf(f, "%f %f %f\n", x_ + step_x, 0.f, z_ + step_z);
            
            fprintf(f, "%f %f %f\n", x_, 0.f, z_ + step_z);
            fprintf(f, "%f %f %f\n", x_, 0.f, z_);
            fprintf(f, "%f %f %f\n", x_ + step_x, 0.f, z_);
            
            
            normals.push_back(Point(0,-1,0));
            normals.push_back(Point(0,-1,0));
            normals.push_back(Point(0,-1,0));
            
            normals.push_back(Point(0,-1,0));
            normals.push_back(Point(0,-1,0));
            normals.push_back(Point(0,-1,0));
            
            
            x_ += step_x;
        }
        
        z_ += step_z;
    }
    
    fprintf(f, "normals\n");
    for(Point p : normals) {
        fprintf(f, "%f %f %f\n", p.x, p.y, p.z);
    }
    
    fclose(f);
}

bool parseBox(int argc, char** argv) {
    
    if(argc == 6) {
        
        float x = atof(argv[2]);
        float y = atof(argv[3]);
        float z = atof(argv[4]);
        
        outputBoxVertices(x, y, z, argv[5]);
        return true;
    }
    else if(argc == 7) {
        
        float x = atof(argv[2]);
        float y = atof(argv[3]);
        float z = atof(argv[4]);
        int divs = atoi(argv[5]);
        
        outputBoxVertices(x, y, z, argv[6], divs);
        return true;
    }
    else {
        return false;
    }
}
// Cone
void outputConeVertices(float radius, float height, int slices, int stacks, const char* fildes) {
    
    float hdiff = height / stacks;
    float rdiff = radius / stacks;
    float step_angle = (2 * M_PI) / slices;
    float angle = 0.0f;
    
    vector<Point> normals;
    vector<Point2D> textures;
    
    FILE* f = fopen(fildes, "w");
    
    fprintf(f, "%d\n", slices * 3 + 6 * slices * stacks);
    // base
    int slice;
    for(slice = 0; slice < slices; slice++) {
        float x1 = radius * sin(angle);
        float z1 = radius * cos(angle);
        
        float x2 = radius * sin(angle + step_angle);
        float z2 = radius * cos(angle + step_angle);
        
        fprintf(f, "%f %f %f\n", x1, 0.f, z1);
        fprintf(f, "%f %f %f\n", 0.f, 0.f, 0.f);
        fprintf(f, "%f %f %f\n", x2, 0.f, z2);
        
        normals.push_back(Point(0,-1,0));
        normals.push_back(Point(0,-1,0));
        normals.push_back(Point(0,-1,0));
        
        angle += step_angle;
    }
    
    
    // face lateral
    height = 0;
    angle = 0.0f;
    int stack;
    for(stack = 0; stack < stacks; stack++) {
        for(slice = 0; slice < slices; slice++) {
            Point p1 = Point(radius * sin(angle),
                             height,
                             radius * cos(angle));
            
            Point p2 = Point(radius * sin(angle + step_angle),
                             height,
                             radius * cos(angle + step_angle));
                             
            Point p3 = Point((radius - rdiff) * sin(angle + step_angle),
                             height + hdiff,
                             (radius - rdiff) * cos(angle + step_angle));

            Point p4 = Point((radius - rdiff) * sin(angle),
                             height + hdiff,
                             (radius - rdiff) * cos(angle));

            
            fprintf(f, "%f %f %f\n", p1.x, p1.y, p1.z);
            fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
            fprintf(f, "%f %f %f\n", p4.x, p4.y, p4.z);
            
            fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
            fprintf(f, "%f %f %f\n", p4.x, p4.y, p4.z);
            fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
            
            Point vaux1_u = Point(p4.x - p1.x, p4.y - p1.y, p4.z - p1.z);
            Point vaux1_v = Point(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
            Point vaux1_p = crossProduct(vaux1_u, vaux1_v);
            
            Point vaux2_u = Point(p3.x - p2.x, p3.y - p2.y, p3.z - p2.z);
            Point vaux2_v = Point(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
            Point vaux2_p = crossProduct(vaux2_u, vaux2_v);
            
            normals.push_back(vaux1_p);
            normals.push_back(vaux2_p);
            normals.push_back(vaux1_p);
            
            normals.push_back(vaux2_p);
            normals.push_back(vaux1_p);
            normals.push_back(vaux2_p);
            
            angle -= step_angle;
        }
        
        height += hdiff;
        radius -= rdiff;
    }
    
    fprintf(f, "normals\n");
    for(Point p : normals) {
        fprintf(f, "%f %f %f\n", p.x, p.y, p.z);
    }
    
    fclose(f);
}


bool parseCone(int argc, char** argv) {
    if(argc != 7) {
        return false;
    }
    
    float radius = atof(argv[2]);
    float height = atof(argv[3]);
    int slices = atoi(argv[4]);
    int stacks = atoi(argv[5]);
    
    outputConeVertices(radius, height, slices, stacks, argv[6]);
    return true;
}

Point2D find_UV_coordinates(Point d, int slice, int stack) {
    return Point2D(0.5f + (atan2(d.z, d.x) / (2*M_PI)),
                   0.5f + (asin(d.y) / M_PI),
                   slice,stack);
}

// esfera
void outputSphereVertices(float radius, int slices, int stacks, const char* fildes) {
    
    float alpha_step = (2 * M_PI) / slices;
    float alpha = 0.0f;
    float beta_step  = M_PI / stacks;
    float beta = -M_PI / 2;
    int tmp_slice = slices;
    vector<Point> normals;
    vector<Point2D> textures;
    
    FILE* f = fopen(fildes, "w");
    fprintf(f, "%d\n", 6 * slices * (stacks - 1));
    
    //float textX = 0.f;
    //float textY = 0.f;
    //float text_angle = M_PI / slices;
    //float step_textX = 1 / slices;
    //float step_textY = 1 / stacks;
    
    // Base inferior
    int slice;
    for(slice = 0; slice < tmp_slice; slice++) {
        Point p1 = Point(radius * cos(beta + beta_step) * sin(alpha),
                         radius * sin(beta + beta_step),
                         radius * cos(beta + beta_step) * cos(alpha));
        
        Point p2 = Point(0.0f,
                         radius * sin(beta),
                         0.0f);
        
        Point p3 = Point(radius * cos(beta + beta_step) * sin(alpha + alpha_step),
                         radius * sin(beta + beta_step),
                         radius * cos(beta + beta_step) * cos(alpha + alpha_step));
        
        fprintf(f, "%f %f %f\n", p1.x, p1.y, p1.z);
        fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
        fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
        
        normals.push_back(Point(p1.x / radius, p1.y / radius, p1.z / radius));
        normals.push_back(Point(p2.x / radius, p2.y / radius, p2.z / radius));
        normals.push_back(Point(p3.x / radius, p3.y / radius, p3.z / radius));
        
        //textures.push_back(find_UV_coordinates(p1.normalize(),slice,0));
        //textures.push_back(find_UV_coordinates(p2.normalize(),slice,0));
        //textures.push_back(find_UV_coordinates(p3.normalize(),slice,0));
        
        textures.push_back(Point2D(alpha / (2 * M_PI), (beta + beta_step) / M_PI + 0.5f,slice,0));
        textures.push_back(Point2D(alpha / (2 * M_PI), beta / M_PI + 0.5f,slice,0));
        textures.push_back(Point2D((alpha + alpha_step) / (2 * M_PI), (beta + beta_step) / M_PI + 0.5f,slice,0));
        
        alpha += alpha_step;
    }
    
    beta += beta_step;
    
    // face lateral
    int stack;
    for(stack = 1; stack < stacks - 1; stack++) {
        for(slice = 0, alpha = 0.0f; slice < tmp_slice; slice++) {
            Point p1 = Point(radius * cos(beta) * sin(alpha),
                             radius * sin(beta),
                             radius * cos(beta) * cos(alpha));

            
            
            Point p2 = Point(radius * cos(beta) * sin(alpha + alpha_step),
                             radius * sin(beta),
                             radius * cos(beta) * cos(alpha + alpha_step));

            
            
            Point p3 = Point(radius * cos(beta + beta_step) * sin(alpha + alpha_step),
                             radius * sin(beta + beta_step),
                             radius * cos(beta + beta_step) * cos(alpha + alpha_step));
            
            
            
            Point p4 = Point(radius * cos(beta + beta_step) * sin(alpha),
                             radius * sin(beta + beta_step),
                             radius * cos(beta + beta_step) * cos(alpha));
            
            
            
            
            //fprintf(f, "slice %d\n", slice);
            fprintf(f, "%f %f %f\n", p1.x, p1.y, p1.z);
            fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
            fprintf(f, "%f %f %f\n", p4.x, p4.y, p4.z);
            
            fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
            fprintf(f, "%f %f %f\n", p4.x, p4.y, p4.z);
            fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
            
            normals.push_back(Point(p1.x / radius, p1.y / radius, p1.z / radius));
            normals.push_back(Point(p2.x / radius, p2.y / radius, p2.z / radius));
            normals.push_back(Point(p4.x / radius, p4.y / radius, p4.z / radius));
            
            normals.push_back(Point(p3.x / radius, p3.y / radius, p3.z / radius));
            normals.push_back(Point(p4.x / radius, p4.y / radius, p4.z / radius));
            normals.push_back(Point(p2.x / radius, p2.y / radius, p2.z / radius));
            
            textures.push_back(Point2D(alpha / (2 * M_PI), beta / M_PI + 0.5f, slice, stack));
            textures.push_back(Point2D((alpha + alpha_step) / (2 * M_PI), beta / M_PI + 0.5f, slice, stack));
            textures.push_back(Point2D(alpha / (2 * M_PI), (beta + beta_step) / M_PI + 0.5f, slice, stack));
            
            textures.push_back(Point2D((alpha + alpha_step) / (2 * M_PI), (beta + beta_step) / M_PI + 0.5f, slice, stack));
            textures.push_back(Point2D(alpha / (2 * M_PI), (beta + beta_step) / M_PI + 0.5f, slice, stack));
            textures.push_back(Point2D((alpha + alpha_step) / (2 * M_PI), beta / M_PI + 0.5f, slice, stack));
            
            alpha += alpha_step;
        }
        
        beta += beta_step;
    }
    
    alpha = 0.f;
    // Base superior
    for(slice = 0; slice < tmp_slice; slice++) {
        Point p1 = Point(radius * cos(beta) * sin(alpha),
                         radius * sin(beta),
                         radius * cos(beta) * cos(alpha));
        
        
        Point p2 = Point(0.0f,
                         radius * sin(beta + beta_step),
                         0.0f);
        

        
        Point p3 = Point(radius * cos(beta) * sin(alpha + alpha_step),
                         radius * sin(beta),
                         radius * cos(beta) * cos(alpha + alpha_step));
        
        fprintf(f, "%f %f %f\n", p1.x, p1.y, p1.z);
        fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
        fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
        
        normals.push_back(Point(p1.x / radius, p1.y / radius, p1.z / radius));
        normals.push_back(Point(p3.x / radius, p3.y / radius, p3.z / radius));
        normals.push_back(Point(p2.x / radius, p2.y / radius, p2.z / radius));

        
        textures.push_back(Point2D(alpha / (2 * M_PI), beta / M_PI + 0.5f,slice,stack));
        textures.push_back(Point2D((alpha + alpha_step) / (2 * M_PI), beta / M_PI + 0.5f,slice,stack));
        textures.push_back(Point2D(0.f, (beta + beta_step) / M_PI + 0.5f,slice,stack));
        
        alpha += alpha_step;
    }
    
    fprintf(f, "normals\n");
    for(Point p : normals) {
        fprintf(f, "%f %f %f\n", p.x, p.y, p.z);
    }
    
    fprintf(f, "textures\n");
    for(Point2D p : textures) {
        //fprintf(f, "%f %f stack = %d slice = %d\n", p.x, p.y, p.stack, p.slice);
        fprintf(f, "%f %f\n", p.x, p.y);
    }
    
    fclose(f);
}

bool parseSphere(int argc, char** argv) {
    
    if(argc != 6) {
        return false;
    }
    
    float radius = atof(argv[2]);
    int slices = atoi(argv[3]);
    int stacks = atoi(argv[4]);
    
    outputSphereVertices(radius, slices, stacks, argv[5]);
    return true;
}

// rings
void outputRingVertices(float radius, float width, int slices, const char* fildes) {
    
    float alpha = 0.0f;
    float alpha_step = (2 * M_PI) / slices;
    
    float r_ = radius - width;
    
    vector<Point> normals;
    vector<Point2D> textures;

    FILE* f = fopen(fildes, "w");
    fprintf(f, "%d\n", 6 * slices);
    
    
    int slice;
    for(slice = 0; slice < slices; slice++) {
        Point p1 = Point(r_ * sin(alpha),
                         0.f,
                         r_ * cos(alpha));
        
        Point p2 = Point(r_ * sin(alpha + alpha_step),
                         0.f,
                         r_ * cos(alpha + alpha_step));
        
        Point p3 = Point(radius * sin(alpha),
                         0.f,
                         radius * cos(alpha));
        
        Point p4 = Point(radius * sin(alpha + alpha_step),
                         0.f,
                         radius * cos(alpha + alpha_step));
        
        
        fprintf(f, "%f %f %f\n", p1.x, p1.y, p1.z);
        fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
        fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
        
        fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
        fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
        fprintf(f, "%f %f %f\n", p4.x, p4.y, p4.z);
        
        normals.push_back(Point(0,1,0));
        normals.push_back(Point(0,1,0));
        normals.push_back(Point(0,1,0));
        
        normals.push_back(Point(0,1,0));
        normals.push_back(Point(0,1,0));
        normals.push_back(Point(0,1,0));
        
        
        fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
        fprintf(f, "%f %f %f\n", p1.x, p1.y, p1.z);
        fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
        
        fprintf(f, "%f %f %f\n", p3.x, p3.y, p3.z);
        fprintf(f, "%f %f %f\n", p2.x, p2.y, p2.z);
        fprintf(f, "%f %f %f\n", p4.x, p4.y, p4.z);
        
        normals.push_back(Point(0,-1,0));
        normals.push_back(Point(0,-1,0));
        normals.push_back(Point(0,-1,0));

        normals.push_back(Point(0,-1,0));
        normals.push_back(Point(0,-1,0));
        normals.push_back(Point(0,-1,0));
        
        
        alpha += alpha_step;
    }
    
    fclose(f);
}

bool parseRing(int argc, char** argv) {
    
    if(argc != 6) {
        return false;
    }
    
    float radius = atof(argv[2]);
    float width = atof(argv[3]);
    int slices = atoi(argv[4]);
    
    outputRingVertices(radius, width, slices, argv[5]);
    return true;
}


// Bezier Patches
int factorial(int n) {
    
    int res = 1;
    for(int i = n; i > 1; i--) {
        res *= i;
    }
    
    return res;
}

float binomialCoeff(int n, int i) {
    
    float res;
    res = (float) factorial(n) / (float) (factorial(i) * factorial(n-i));
    
    return res;
}

float bernsteinPol(int n, int i, float t) {
    
    float res = binomialCoeff(n, i);
    res *= pow(t, i);
    res *= pow(1 - t, n - i);
    
    return res;
}



Point bezierPatch(vector<Point> points,
                    float u, float v, int n, int m) {
    
    Point pf = Point(0,0,0);
    
    int i, j;
    for(i = 0; i <= n; i++) {
        for(j = 0; j <= m; j++) {
            Point p = points[i * (n+1) + j];
            float bi = bernsteinPol(n, i, v);
            float bj = bernsteinPol(m, j, u);
            
            pf.x += bi * bj * p.x;
            pf.y += bi * bj * p.y;
            pf.z += bi * bj * p.z;
        }
    }
    
    return pf;
}


void outputBezierPatchVertices(vector<Point> points, int tessellation, int n, int m,
                         const char* fildes) {
    
    ofstream out_file;
    out_file.open(fildes, ios_base::app);
    
    if(out_file.fail()) {
        cerr << "Error opening file." << endl;
    }
    
    float f = 1.f / (float) tessellation;
    float u = 0.f;
    for(int i = 0; i < tessellation; i++) {
        float v = 0.f;
        float u_ = u + f;
        
        for(int j = 0; j < tessellation; j++) {
            float v_ = v + f;
            
            Point p1 = bezierPatch(points, u, v, n, m);
            Point p2 = bezierPatch(points, u_, v, n, m);
            Point p3 = bezierPatch(points, u, v_, n, m);
            Point p4 = bezierPatch(points, u_, v_, n, m);
            
            out_file << p1.toString() << endl;
            out_file << p2.toString() << endl;
            out_file << p4.toString() << endl;
            
            out_file << p1.toString() << endl;
            out_file << p4.toString() << endl;
            out_file << p3.toString() << endl;
            
            v += f;
            out_file.flush();
        }
        
        u += f;
    }
    
    
    out_file.close();
}


void generateBezierPatches(const char* src_file, int tessellation, const char* fildes) {
    
    /*************
     PARSING
     **************/
    
    ifstream in_file;
    in_file.open(src_file);
    
    if(in_file.fail()) {
        cerr << "Error opening file" << endl;
        exit(1);
    }
    
    // get number of patches
    string aux1;
    getline(in_file, aux1);
    int number_of_patches = stoi(aux1);
    
    // get patches indices
    // vector of patches
    vector<Patch> patches;
    char delimiter = ',';
    for(int i = 0; i < number_of_patches; i++) {
        Patch p;
        
        string indices;
        getline(in_file, indices);
        
        string token1;
        istringstream token_stream1(indices);
        
        while(getline(token_stream1, token1, delimiter)) {
            p.indices.push_back(stoi(token1));
        }
        
        if(p.indices.size() != 16) {
            cerr << "Número de elementos errado. Tem de ter 16 vértices." << endl;
        }
        
        patches.push_back(p);
    }
    
    if(patches.size() != number_of_patches) {
        cerr << "Número de patches incorreto." << endl;
    }
    
    // get number of control points
    string aux2;
    getline(in_file, aux2);
    int number_of_control_points = stoi(aux2);
    
    
    // get control points
    vector<Point> control_points;
    for(int i = 0; i < number_of_control_points; i++) {
        Point a = Point(0,0,0);
        
        string coordinates;
        getline(in_file, coordinates);
        
        string token2;
        istringstream token_stream2(coordinates);
        
        // x
        getline(token_stream2, token2, delimiter);
        a.x = stof(token2);
        
        // y
        getline(token_stream2, token2, delimiter);
        a.y = stof(token2);
        
        // z
        getline(token_stream2, token2, delimiter);
        a.z = stof(token2);
        
        control_points.push_back(a);
    }
    
    if(control_points.size() != number_of_control_points) {
        cerr << "Número de pontos de controlo errados." << endl;
    }
    
    
    /*************
     DRAWING
     **************/
    
    // write number of vertices
    ofstream out_file;
    out_file.open(fildes);
    out_file << tessellation * tessellation * 6 * number_of_patches << endl;
    out_file.flush();
    out_file.close();
    
    for(Patch p : patches) {
        // criar o vetor de pontos final do patch
        vector<Point> fpoints;
        for(int index : p.indices) {
            fpoints.push_back(control_points[index]);
        }
        
        // desenhar o patch
        outputBezierPatchVertices(fpoints, tessellation, 3, 3, fildes);
    }
}


bool parseBezierPatch(int argc, char** argv) {
    
    if(argc != 5) {
        return false;
    }
    
    int tessellation = atoi(argv[3]);
    
    generateBezierPatches(argv[2], tessellation, argv[4]);
    return true;
}


int main(int argc, char** argv) {

    if(strcmp(argv[1], "sphere") == 0) {
        parseSphere(argc, argv);
    }
    else if(strcmp(argv[1], "cone") == 0) {
        parseCone(argc, argv);
    }
    else if(strcmp(argv[1], "plane") == 0) {
        parsePlane(argc, argv);
    }
    else if(strcmp(argv[1], "box") == 0) {
        parseBox(argc, argv);
    }
    else if(strcmp(argv[1], "ring") == 0) {
        parseRing(argc, argv);
    }
    else if(strcmp(argv[1], "patch") == 0) {
        parseBezierPatch(argc, argv);
    }

    return 0;
}
