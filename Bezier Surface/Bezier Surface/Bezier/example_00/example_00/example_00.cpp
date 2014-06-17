// CS184 Simple OpenGL Example
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string.h>
#include <stdio.h>
#include <cstdlib>
//#include "Vector.h"
//#include "Patch.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif
#include <time.h>
#include <math.h>
#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif
#define PI 3.14159265
using namespace std;
/*
 * OGL01Shape3D.cpp: 3D Shapes
 */
float sqr(float x){
	return x*x;
}
//****************************************************
// Some Classes
//****************************************************
class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};
class Vector{
public:
	float x,y,z;
	Vector(){};
	Vector(float x, float y, float z){
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector normalize() {
		float length = sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
        float nx = this->x / length;
        float ny = this->y / length;
        float nz = this->z / length;
        Vector retvect = Vector(nx, ny, nz);
        return retvect;
    }
	Vector operator * (float a){
		Vector temp;
		temp.x = a*x;
		temp.y = a*y;
		temp.z = a*z;
		return (temp);
	}
	Vector operator + (Vector v){
		Vector temp;
		temp.x = x + v.x;
		temp.y = y + v.y;
		temp.z = z + v.z;
		return (temp);
	}
	Vector operator - (Vector v){
		Vector temp;
		temp.x = x - v.x;
		temp.y = y - v.y;
		temp.z = z - v.z;
		return (temp);
	}
	float operator * (Vector v){
		return x*v.x + y*v.y + z*v.z;
	}
	Vector cross(Vector nvect) {
		float nx = -(this->y * nvect.z) + (this->z * nvect.y);
		float ny = -(this->z * nvect.x) + (this->x * nvect.z);
		float nz = -(this->x * nvect.y) + (this->y * nvect.x);
		Vector retvect = Vector(nx, ny, nz);
    return retvect;
	}
};
class Point{
public:
	float x,y,z;
	float h;
	Point() {};
	Point(float x, float y, float z){
		this->x = x;
		this->y = y;
		this->z = z;
		this->h = h;
	}
	Point operator * (float a){
		Point temp;
		temp.x = x*a;
		temp.y = y*a;
		temp.z = z*a;
		return (temp);
	}
	Point operator + (Point p){
		Point temp;
		temp.x = x + p.x;
		temp.y = y + p.y;
		temp.z = z + p.z;
		return (temp);
	}
	Vector operator - (Point p){
		Vector temp;
		temp.x = x - p.x;
		temp.y = y - p.y;
		temp.z = z - p.z;
		return (temp);
	}
	float distant(Point p){
		return sqrt(sqr(this->x - p.x) + sqr(this->y - p.y) + sqr(this->z - p.z));
	}
};
class Patch{
	Point patch[4][4];
	Point transpatch[4][4];
public:
	Patch( Point p[][4]){
		for(int j = 0; j < 4; j++){
			for(int i = 0; i < 4; i++){
				patch[i][j] = p[i][j];
			}
		}
		for(int j = 0; j < 4; j++){
			for(int i = 0; i < 4; i++){
				transpatch[i][j] = patch[j][i];
			}
		}
	}
	Point getpoint(int i,int j){
		return patch[i][j];
	}
	Point* getrow(int i){
		Point* temp = patch[i];
		return temp;
	}
	Point* getcol(int i){
		Point* temp = transpatch[i];
		return temp;
	}
};
class LocalGeo{
public:
	Point point;
	Vector normal;
	float u;
	float v;
	Patch* originalpatch;
	LocalGeo(){};
	LocalGeo( Point p, Vector n, float iu, float iv, Patch *patch){
		point = p;
		normal = n;
		u = iu;
		v = iv;
		originalpatch = patch;
	}
};
class Triangle{
public:
	LocalGeo p0;
	LocalGeo p1;
	LocalGeo p2;
	Triangle(){};
	Triangle(LocalGeo point0, LocalGeo point1, LocalGeo point2){
		p0 = point0;
		p1 = point1;
		p2 = point2;
	}
};
/* Global variables */
char title[] = "3D Shapes";
float epsilon = 0.001;
float rotAngley = 0;
float rotAnglex = 0;
int transx = 0;
int transy = 0;
int zoom = 0;
int smoothorflat = 0;
int wireframe = 0;
vector<float> loadfloats;
vector<Patch> loadpatches;
string filename;
int adaptoruni = 0;
float subdivparam = 0.0;
//****************************************************
// Bezier
//****************************************************
void bezcurveinterp(Point* curve, float u, Point* point, Vector* dev){
	Point A = curve[0]*(1.0-u) + curve[1]*u;
	Point B = curve[1]*(1.0-u) + curve[2]*u;
	Point C = curve[2]*(1.0-u) + curve[3]*u;
	Point D = A*(1.0-u) + B*u;
	Point E = B*(1.0-u) + C*u;
	Point p = D*(1.0-u) + E*u;
	Vector dPdu = (E-D)*3.0;
	*point = p;
	*dev = dPdu;
}
void bezpatchinterp(Patch patch, float u, float v, Point* point, Vector* normal){
	Point vcurve[4];
	Vector dPdv;
	Point ucurve[4];
	Vector dPdu;
	Point p;
	//getting the row curves
	bezcurveinterp(patch.getrow(0), u, &vcurve[0], &dPdv);
	bezcurveinterp(patch.getrow(1), u, &vcurve[1], &dPdv);
	bezcurveinterp(patch.getrow(2), u, &vcurve[2], &dPdv);
	bezcurveinterp(patch.getrow(3), u, &vcurve[3], &dPdv);
	//getting the collumn curves
	bezcurveinterp(patch.getcol(0), v, &ucurve[0], &dPdu);
	bezcurveinterp(patch.getcol(1), v, &ucurve[1], &dPdu);
	bezcurveinterp(patch.getcol(2), v, &ucurve[2], &dPdu);
	bezcurveinterp(patch.getcol(3), v, &ucurve[3], &dPdu);
	//getting the point and normal
	bezcurveinterp(vcurve, v, &p, &dPdv);
	bezcurveinterp(ucurve, u, &p, &dPdu);
	Vector n = (dPdv.cross(dPdu)).normalize();
	*point = p;
	*normal = n;
}
//function that does uniform subdivision
void uniformsubdivide(Patch patch, float step){
	int numdiv = ((1 + epsilon)/step);
	float u = 0;
	float v = 0;
	vector<LocalGeo> localgeolist;
	for(int iu = 0; iu <= numdiv; iu++){
		u = iu*step;
		for(int iv = 0; iv <= numdiv; iv++){
			v = iv*step;
			Point point;
			Vector normal;
			bezpatchinterp(patch,u,v,&point,&normal);
			localgeolist.insert(localgeolist.begin(),LocalGeo(point,normal,u,v,&patch));
		}
	}
	int edgesize = sqrt(localgeolist.size());
	for(int j = 0; j < edgesize - 1; j++){
		for(int i = 0; i < edgesize - 1; i++){
			glBegin(GL_QUADS);
			glNormal3f(localgeolist[i + j*edgesize].normal.x,localgeolist[i + j*edgesize].normal.y,localgeolist[i + j*edgesize].normal.z);
			glVertex3f(localgeolist[i + j*edgesize].point.x,localgeolist[i + j*edgesize].point.y,localgeolist[i + j*edgesize].point.z);
			glNormal3f(localgeolist[i + 1 + j*edgesize].normal.x,localgeolist[i + 1 + j*edgesize].normal.y,localgeolist[i + 1 + j*edgesize].normal.z);
			glVertex3f(localgeolist[i + 1 + j*edgesize].point.x,localgeolist[i + 1 + j*edgesize].point.y,localgeolist[i + 1 + j*edgesize].point.z);
			glNormal3f(localgeolist[i + 1 + (j + 1)*edgesize].normal.x,localgeolist[i + 1 + (j + 1)*edgesize].normal.y,localgeolist[i + 1 + (j + 1)*edgesize].normal.z);
			glVertex3f(localgeolist[i + 1 + (j + 1)*edgesize].point.x,localgeolist[i + 1 + (j + 1)*edgesize].point.y,localgeolist[i + 1 + (j + 1)*edgesize].point.z);
			glNormal3f(localgeolist[i + (j + 1)*edgesize].normal.x,localgeolist[i + (j + 1)*edgesize].normal.y,localgeolist[i + (j + 1)*edgesize].normal.z);
			glVertex3f(localgeolist[i + (j + 1)*edgesize].point.x,localgeolist[i + (j + 1)*edgesize].point.y,localgeolist[i + (j + 1)*edgesize].point.z);
			glEnd();
		}
	}
}
//function that does center test
int adaptivetest(LocalGeo p0, LocalGeo p1, float th){
	Point online = Point((p0.point.x + p1.point.x)/2, (p0.point.y + p1.point.y)/2, (p0.point.z + p1.point.z)/2);
	Point oncurve;
	Vector normal;
	bezpatchinterp(*(p0.originalpatch),(p0.u + p1.u)/2,(p0.v + p1.v)/2,&oncurve,&normal);
	float d = oncurve.distant(online);
	if(d > th){
	    return 1;
	}else{
		return 0;
	}
}
//function that does adaptive subdivision
void adaptivesubdivide(Patch patch, float th){
	float step = 0.25;
	int numdiv = ((1 + epsilon)/step);
	float u = 0;
	float v = 0;
	vector<LocalGeo> localgeolist;
	for(int iu = 0; iu <= numdiv; iu++){
		u = iu*step;
		for(int iv = 0; iv <= numdiv; iv++){
			v = iv*step;
			Point point;
			Vector normal;
			bezpatchinterp(patch,u,v,&point,&normal);
			localgeolist.insert(localgeolist.begin(),LocalGeo(point,normal,u,v,&patch));
		}
	}
	vector<Triangle> trianglelist;
	int edgesize = sqrt(localgeolist.size());
	for(int j = 0; j < edgesize - 1; j++){
		for(int i = 0; i < edgesize - 1; i++){
			float midu = (localgeolist[i + j*edgesize].u + localgeolist[i  + 1 + j*edgesize].u + localgeolist[i + (j + 1)*edgesize].u + localgeolist[i + 1 + (j + 1)*edgesize].u)/4;
			float midv = (localgeolist[i + j*edgesize].v + localgeolist[i  + 1 + j*edgesize].v + localgeolist[i + (j + 1)*edgesize].v + localgeolist[i + 1 + (j + 1)*edgesize].v)/4;
			Point midpoint;
			Vector midnorm;
			bezpatchinterp(patch,midu,midv,&midpoint,&midnorm);
			float x1 = localgeolist[i + j*edgesize].point.x +  localgeolist[i + 1 + (j + 1)*edgesize].point.x;
			float y1 = localgeolist[i + j*edgesize].point.y +  localgeolist[i + 1 + (j + 1)*edgesize].point.y;
			float z1 = localgeolist[i + j*edgesize].point.z +  localgeolist[i + 1 + (j + 1)*edgesize].point.z;
			Point diagonal1 = Point(x1,y1,z1);
			float x2 = localgeolist[i + 1 + j*edgesize].point.x +  localgeolist[i + (j + 1)*edgesize].point.x;
			float y2 = localgeolist[i + 1 + j*edgesize].point.y +  localgeolist[i + (j + 1)*edgesize].point.y;
			float z2 = localgeolist[i + 1 + j*edgesize].point.z +  localgeolist[i + (j + 1)*edgesize].point.z;
			Point diagonal2 = Point(x2,y2,z2);
			float d1 = midpoint.distant(diagonal1);
			float d2 = midpoint.distant(diagonal2);
			if(d1 > d2){
				trianglelist.push_back(Triangle(localgeolist[i + j*edgesize],localgeolist[i + 1 + j*edgesize],localgeolist[i + (j + 1)*edgesize]));
				trianglelist.push_back(Triangle(localgeolist[i + (j + 1)*edgesize],localgeolist[i + 1 + j*edgesize],localgeolist[i + 1 + (j + 1)*edgesize]));
			}else{
				trianglelist.push_back(Triangle(localgeolist[i + j*edgesize],localgeolist[i + 1 + j*edgesize],localgeolist[i + 1 + (j + 1)*edgesize]));
				trianglelist.push_back(Triangle(localgeolist[i + j*edgesize],localgeolist[i + 1 + (j + 1)*edgesize],localgeolist[i + (j + 1)*edgesize]));
			}
		}
	}
	while(trianglelist.size() != 0){
		Triangle curr = trianglelist.back();
		int e1 = adaptivetest(curr.p0,curr.p1,th);
		int e2 = adaptivetest(curr.p1,curr.p2,th);
		int e3 = adaptivetest(curr.p0,curr.p2,th);
		if(e1 == 1 && e2 == 1 && e3 == 1){
			Point p1,p2,p3;
			Vector n1,n2,n3;
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2, &p1,&n1);
			LocalGeo l1 = LocalGeo(p1,n1,(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2,curr.p0.originalpatch);
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2, &p2,&n2);
			LocalGeo l2 = LocalGeo(p2,n2,(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2,curr.p0.originalpatch);
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2, &p3,&n3);
			LocalGeo l3 = LocalGeo(p3,n3,(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2,curr.p0.originalpatch);
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,l3,l1));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p1,l1,l2));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p2,l2,l3));
			trianglelist.insert(trianglelist.begin(),Triangle(l1,l3,l2));
			trianglelist.pop_back();
			continue;
		}else if(e1 == 1 && e2 == 0 && e3 == 1){
			Point p1,p3;
			Vector n1,n3;
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2, &p1,&n1);
			LocalGeo l1 = LocalGeo(p1,n1,(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2,curr.p0.originalpatch);
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2, &p3,&n3);
			LocalGeo l3 = LocalGeo(p3,n3,(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2,curr.p0.originalpatch);
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,l3,l1));
			trianglelist.insert(trianglelist.begin(),Triangle(l1,curr.p2,curr.p1));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p2,l1,l3));
			trianglelist.pop_back();
			continue;
		}else if(e1 == 0 && e2 == 1 && e3 == 1){
			Point p2,p3;
			Vector n2,n3;
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2, &p2,&n2);
			LocalGeo l2 = LocalGeo(p2,n2,(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2,curr.p0.originalpatch);
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2, &p3,&n3);
			LocalGeo l3 = LocalGeo(p3,n3,(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2,curr.p0.originalpatch);
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p2,l2,l3));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p1,l3,l2));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,curr.p1,l3));
			trianglelist.pop_back();
			continue;
		}else if(e1 == 1 && e2 == 1 && e3 == 0){
			Point p1,p2;
			Vector n1,n2;
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2, &p1,&n1);
			LocalGeo l1 = LocalGeo(p1,n1,(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2,curr.p0.originalpatch);
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2, &p2,&n2);
			LocalGeo l2 = LocalGeo(p2,n2,(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2,curr.p0.originalpatch);
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p1,l1,l2));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,l2,l1));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,curr.p2,l2));
			trianglelist.pop_back();
			continue;
		}else if(e1 == 0 && e2 == 0 && e3 == 1){
			Point p3;
			Vector n3;
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2, &p3,&n3);
			LocalGeo l3 = LocalGeo(p3,n3,(curr.p2.u + curr.p0.u)/2,(curr.p2.v + curr.p0.v)/2,curr.p0.originalpatch);
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,l3,curr.p1));
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p1,l3,curr.p2));
			trianglelist.pop_back();
			continue;
		}else if(e1 == 0 && e2 == 1 && e3 == 0){
			Point p2;
			Vector n2;
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2, &p2,&n2);
			LocalGeo l2 = LocalGeo(p2,n2,(curr.p1.u + curr.p2.u)/2,(curr.p1.v + curr.p2.v)/2,curr.p0.originalpatch);
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,l2,curr.p1));
			trianglelist.insert(trianglelist.begin(),Triangle(l2,curr.p0,curr.p2));
			trianglelist.pop_back();
			continue;
		}else if(e1 == 1 && e2 == 0 && e3 == 0){
			Point p1;
			Vector n1;
			bezpatchinterp(*(curr.p0.originalpatch),(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2, &p1,&n1);
			LocalGeo l1 = LocalGeo(p1,n1,(curr.p0.u + curr.p1.u)/2,(curr.p0.v + curr.p1.v)/2,curr.p0.originalpatch);
			trianglelist.insert(trianglelist.begin(),Triangle(curr.p0,curr.p2,l1));
			trianglelist.insert(trianglelist.begin(),Triangle(l1,curr.p2,curr.p1));
			trianglelist.pop_back();
			continue;
		}else{
			glBegin(GL_TRIANGLES);
			glNormal3f(curr.p0.normal.x,curr.p0.normal.y,curr.p0.normal.z);
			glVertex3f(curr.p0.point.x,curr.p0.point.y,curr.p0.point.z);
			glNormal3f(curr.p1.normal.x,curr.p1.normal.y,curr.p1.normal.z);
			glVertex3f(curr.p1.point.x,curr.p1.point.y,curr.p1.point.z);
			glNormal3f(curr.p2.normal.x,curr.p2.normal.y,curr.p2.normal.z);
			glVertex3f(curr.p2.point.x,curr.p2.point.y,curr.p2.point.z);
			glEnd();
			trianglelist.pop_back();
			continue;
		}
		
	}	
	
}
/* Initialize OpenGL Graphics */
void initGL() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   //Lightning model
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
      glEnable(GL_LIGHTING);
      //Seting up the light component
      glEnable(GL_LIGHT0);
      GLfloat ambientlight0[] = {0.1,0.1,0.1,1.0};
      GLfloat diffuselight0[] = {0.2,0.2,0.2,1.0};
      GLfloat specularlight0[] = {0.8,0.8,0.8,1.0};
      glLightfv(GL_LIGHT0, GL_AMBIENT,ambientlight0);
      glLightfv(GL_LIGHT0, GL_DIFFUSE,diffuselight0);
      glLightfv(GL_LIGHT0, GL_SPECULAR,specularlight0);
      //Seting up the light position
      GLfloat positionlight0[] = {50.0,50.0,50.0,1.0};
      glLightfv(GL_LIGHT0, GL_POSITION, positionlight0);
      //Seting up the light component
      glEnable(GL_LIGHT1);
      GLfloat ambientlight1[] = {0.1,0.1,0.1,1.0};
      GLfloat diffuselight1[] = {0.2,0.2,0.2,1.0};
      GLfloat specularlight1[] = {0.8,0.8,0.8,1.0};
      glLightfv(GL_LIGHT1, GL_AMBIENT,ambientlight1);
      glLightfv(GL_LIGHT1, GL_DIFFUSE,diffuselight1);
      glLightfv(GL_LIGHT1, GL_SPECULAR,specularlight1);
      //Seting up the light position
      GLfloat positionlight1[] = {0.5,0.5,40,1.0};
      glLightfv(GL_LIGHT1, GL_POSITION, positionlight1);
      //Seting up the light component
      glEnable(GL_LIGHT2);
      GLfloat ambientlight2[] = {0.1,0.1,0.1,1.0};
      GLfloat diffuselight2[] = {0.2,0.2,0.2,1.0};
      GLfloat specularlight2[] = {0.8,0.8,0.8,1.0};
      glLightfv(GL_LIGHT2, GL_AMBIENT,ambientlight2);
      glLightfv(GL_LIGHT2, GL_DIFFUSE,diffuselight2);
      glLightfv(GL_LIGHT2, GL_SPECULAR,specularlight2);
      //Seting up the light position
      GLfloat positionlight2[] = {0.8,0.8,0.2,1.0};
      glLightfv(GL_LIGHT2, GL_POSITION, positionlight2);
      glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
      glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
      glShadeModel(GL_SMOOTH);   // Enable smooth shading
      glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

/* Handler for window-repaint event. Called back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
   // Render a color-cube consisting of 6 quads with different colors
   glLoadIdentity();                 // Reset the model-view matrix
  // glTranslatef(-1.0f, 0.5f, -8.0f);  // Move right and into the screen
   glTranslatef(0, 0.5f, zoom);
   glTranslatef(transx, 0.5f, -7);
   glTranslatef(0.0f, transy, -7);
   glRotatef(rotAngley,0.0,1.0,0.0);
   glRotatef(rotAnglex,1.0,0.0,0.0);
   GLfloat ambient[] = {0.1,0.1,0.0,1.0};
   GLfloat diffuse[] = {0.9,0.9,0.0,1.0};
   GLfloat specular[] = {0.5,0.5,0.0,1.0};
    if (wireframe % 2 == 0) {
   		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   	   }else {
   		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   	   }
   	   if (smoothorflat % 2 == 0) {
   		glEnable(GL_FLAT);
   		glShadeModel(GL_FLAT);
   	   } else {
   		glEnable(GL_SMOOTH);
   		glShadeModel(GL_SMOOTH);
   	   }
      glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
      glMaterialf(GL_FRONT,GL_SHININESS,128.0);
      glLightfv(GL_LIGHT0,GL_AMBIENT, specular);

      
      for (int x = 0; x < loadpatches.size(); x++) {
    	  
		  if (adaptoruni == 1) {
			  adaptivesubdivide(loadpatches[x], subdivparam);
			}
		  uniformsubdivide(loadpatches[x],subdivparam);
	  }





   glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);

   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 500.0f);
}


//****************************************************
// Keyboard Input
//****************************************************
void keyboard(unsigned char key, int x, int y)
{
//zoom in
    if (key == 61)
        {
            zoom+= 1;
        }
    //zoom out
    if (key == 45)
        {
            zoom-=1;
        }
    //space bar exit
    if (key == 32)
        {
            exit(0);
        }
    if (key == 's'){
    	smoothorflat +=1;
    }
    if (key == 'w'){
    	wireframe += 1;
    }
    glutPostRedisplay();
}
void SpecialKeys(int key, int x, int y)
{
	int mod = glutGetModifiers();
    switch (key)
	{
		case GLUT_KEY_LEFT:
			if (mod == GLUT_ACTIVE_SHIFT) {
				transx-=1;
			} else {
			rotAngley +=10;
			}
			break;
		case GLUT_KEY_RIGHT:
			if (mod == GLUT_ACTIVE_SHIFT) {
				transx+=1;
			} else {
			rotAngley -=10;
			}
			break;
		case GLUT_KEY_UP:
			if (mod == GLUT_ACTIVE_SHIFT) {
				transy+=1;
			} else {
			rotAnglex -=10;
			}
			break;
		case GLUT_KEY_DOWN:
			if (mod == GLUT_ACTIVE_SHIFT) {
				transy-=1;
			} else {
			rotAnglex +=10;
			}
			break;
	}
    glutPostRedisplay();
}
//****************************************************
// Reading files
//****************************************************
/* Function for loading the scene */
vector< float > loadScene(std::string file) {
    vector<float> vertices;
    std::ifstream inpfile(file.c_str());
    if(!inpfile.is_open()) {
        std::cout << "Unable to open file" << std::endl;
    } else {
        std::string line;
        while(inpfile.good()) {
            std::vector<std::string> splitline;
            std::string buf;
            std::getline(inpfile,line);
            std::stringstream ss(line);
            while (ss >> buf) {
                splitline.push_back(buf);
            }
            //Ignore blank lines
            if(splitline.size() == 0) {
                continue;
            }
            //Ignore comments
            if(splitline[0][0] == '#') {
                continue;
            }
            if (splitline.size() == 1) {
                vertices.push_back(atof(splitline[0].c_str()));
            } else {
                for (int i = 0; i < (int)splitline.size(); i++) {
                    vertices.push_back(atof(splitline[i].c_str()));
                }
            }
        }
    }
    return vertices;
}
/* Function for loading patches */
vector <Patch> buildPatches(vector <float> data) {
    float numofpatches = data[0];
    vector <Patch> patches;
    vector <Point> vertices;
    for (int i = 1; i < data.size(); i +=3){
        vertices.push_back(Point(data[i], data[i+1], data[i+2]));
    }
    int step = 0;
    for (int j = 0; j < numofpatches; j++) {
    	Point test[4][4];
    	for (int x = 0; x < 4; x++) {
    		for (int y = 0; y < 4; y++) {
    			test[x][y] = vertices[step];
    			step++;
    		}
    	}
    	Patch temp = Patch(test);
    	patches.push_back(temp);
    }
    return patches;
}
/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
	if (argc == 4) {
		filename = argv[1];
		subdivparam = atof(argv[2]);
		//use adaptive subdivsion
		adaptoruni = 1;
	} else {
		filename = argv[1];
		subdivparam = atof(argv[2]);
		//use uniform subdivision
	}
	loadfloats = loadScene(filename);
	loadpatches = buildPatches(loadfloats);
    glutInit(&argc, argv);            // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(640, 480);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutCreateWindow(title);          // Create window with the given title
    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);       // Register callback handler for window re-size event
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(keyboard);
    initGL();                       // Our own OpenGL initialization
    glutMainLoop();                 // Enter the infinite event-processing loop
    return 0;
}
