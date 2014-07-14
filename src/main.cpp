#include<GL/glut.h>
#include<string.h>
#include<stdio.h>
#include<cmath>
#include<GL/gl.h>
#include<GL/glu.h>
#include<unistd.h>

#include "Utility.h"

GLfloat red_light[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat yellow_light[] = { 1.0, 0.0, 1.0, 1.0 };
GLfloat blue_light[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat green_light[] = { 0.0, 1.0, 0.0, 1.0 };

float circularLight[100][100];
float plyVertex[40000][5];
float plyVertexNormal[40000][4];
int plyFace[80000][4],c;
char fileName[25];
float nrml[80000][3];
float scale = 4.0;
int plyVertexCount,plyfaceCount,flag2=0,perPolygonFlag=0;
float tx=0.0,ty=0.0,tz=0.0;
int discoFlag = 0;
int circularLightFlag = 0, discoLightingFlag = 0;
int rotationFlag = 0;
int clockRotationFlag = 0;
float quaternionX=1,quaternionY=0,quaternionZ=1;


typedef struct { float x, y, z; } vector;
typedef struct { float w, x, y, z; } quaternion;

int mouse_pressed;
int callback_set;
float previous_time, current_time;
float mouse_x, mouse_y;

vector downwards;
vector model_position;
quaternion model_orientation;


vector make_vector(float x, float y, float z) {
  return (vector){x, y, z};
}

float vector_x(vector v) { return v.x; }
float vector_y(vector v) { return v.y; }
float vector_z(vector v) { return v.z; }

vector vector_add(vector a, vector b) {
  return (vector){a.x + b.x, a.y + b.y, a.z + b.z};
}

vector vector_scale(float constant, vector v) {
  return (vector){constant*v.x, constant*v.y, constant*v.z};
}

float vector_norm(vector v) {
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vector vector_normalize(vector v) {
  return vector_scale(1/vector_norm(v), v);
}

float vector_dot_product(vector a, vector b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

vector vector_cross_product(vector a, vector b) {
  return (vector){a.y*b.z - a.z*b.y, a.x*b.z - a.z*b.x, a.x*b.y - a.y*b.x};
}

quaternion make_quaternion(float w, float x, float y, float z) {
  return (quaternion){w, x, y, z};
}

quaternion make_rotation_quaternion_from_axis_and_angle(vector axis, float angle) {
  return (quaternion){cos(angle/2.0),
                      vector_x(axis)*sin(angle/2.0),
                      vector_y(axis)*sin(angle/2.0),
                      vector_z(axis)*sin(angle/2.0)};
}

quaternion quaternion_multiply(quaternion a, quaternion b) {
  return (quaternion){a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
                      a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
                      a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
                      a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w};
}

quaternion quaternion_conjugate(quaternion q) {
  return (quaternion){q.w, -q.x, -q.y, -q.z};
}

vector quaternion_rotate_vector(quaternion q, vector v) {
  quaternion vq;
  
  vq = make_quaternion(0, vector_x(v), vector_y(v), vector_z(v));
  vq = quaternion_multiply(q, vq);
  vq = quaternion_multiply(vq, quaternion_conjugate(q));
  return make_vector(vq.x, vq.y, vq.z);
}

void quaternion_fill_opengl_rotation_matrix(quaternion q, GLfloat * m) {
  vector x,   y,   z;
  vector x_t, y_t, z_t;
  
  x = make_vector(1, 0, 0);
  y = make_vector(0, 1, 0);
  z = make_vector(0, 0, 1);
  
  x_t = quaternion_rotate_vector(q, x);
  y_t = quaternion_rotate_vector(q, y);
  z_t = quaternion_rotate_vector(q, z);

  m[ 0] = vector_dot_product(x, x_t);
  m[ 1] = vector_dot_product(y, x_t);
  m[ 2] = vector_dot_product(z, x_t);
  m[ 3] = 0;
  
  m[ 4] = vector_dot_product(x, y_t);
  m[ 5] = vector_dot_product(y, y_t);
  m[ 6] = vector_dot_product(z, y_t);
  m[ 7] = 0;
  
  m[ 8] = vector_dot_product(x, z_t);
  m[ 9] = vector_dot_product(y, z_t);
  m[10] = vector_dot_product(z, z_t);
  m[11] = 0;

  m[12] = 0;
  m[13] = 0;
  m[14] = 0;
  m[15] = 1;
}

void animate(float dt) {
	
	 vector model_movement;
	 vector rotation_axis;
	 quaternion rotation;
	 float speed;
	  
	 if(dt < 0.0001) return;
	 if(mouse_x*mouse_x + mouse_y*mouse_y < 0.0001) return;

	 model_movement = vector_scale(dt*3, make_vector(mouse_x, mouse_y, 0));
	 model_position = vector_add(model_position, model_movement);
	  
	 speed = vector_norm(model_movement);
	  
	 if(clockRotationFlag == 1)  // clockwise rotation
		model_movement.x = -model_movement.x;
	 else  // anti-clock wise
		model_movement.x = model_movement.x; 
	
	downwards = make_vector(quaternionX, quaternionY, quaternionZ);

	
	  rotation_axis = vector_normalize(vector_cross_product(downwards, model_movement));
	  rotation = make_rotation_quaternion_from_axis_and_angle(rotation_axis, speed*1.481);
	  model_orientation = quaternion_multiply(rotation, model_orientation);
}

void timer(int ignored) {
	
	current_time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
	animate(current_time - previous_time);
	previous_time = current_time;
	glutTimerFunc(20, timer, -1);
		  
	if(rotationFlag == 1)
		glutPostRedisplay();
		
}

void visibility(int state) {
	
  if(state == GLUT_VISIBLE && !callback_set){
    callback_set = 1;
    previous_time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    timer(-1);
  }
}

void init_lighting(void) 
{
	
	if(flag2==0)
	{
  	    glDisable(GL_LIGHT1);
   		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   		GLfloat mat_shininess[] = { 50.0 };
   		GLfloat now[] ={0.5};
   		GLfloat light_position[] = { 1.0, 1.0, 1.0, -50.0 };
	    glClearColor (0.0, 0.0, 0.0, 0.0);
		glShadeModel (GL_SMOOTH);
   		GLfloat cyan[] = {1.0f, 1.0f, 1.0f, 1.f};
   		glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
   		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  	    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   		glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,now);
   		glEnable(GL_LIGHTING);
   		glEnable(GL_LIGHT0);
   		glEnable(GL_DEPTH_TEST);
   		glEnable(GL_NORMALIZE); 
  	}
 	else
  	{
   		glDisable(GL_LIGHT0);
   		GLfloat light0Position[] = {1.0, 1.0, 1.0, 0.0}; 
		glLightfv(GL_LIGHT1, GL_POSITION, light0Position);
   		GLfloat ambient0[]={1.0, 1.0, 1.0, 1.0}; 
  		GLfloat diffuse0[]={1.0, 1.0, 1.0, 1.0};
  		GLfloat specular0[]={1.0,1.0, 1.0, 1.0};
   		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient0);
   		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse0);
  		glLightfv(GL_LIGHT1, GL_SPECULAR, specular0);
  		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient0);
  		glEnable(GL_LIGHTING);
  		glEnable(GL_LIGHT1);
  		glEnable(GL_DEPTH_TEST);
  		glEnable(GL_NORMALIZE);
  	 }

}
void keyPressed(unsigned char key, int x, int y) {  
  
		if(key == 119) // translate up ; key = 'w'
		{ 				
			ty += 0.005;					
		}  
		if(key == 97)  // translate right; key = 'd'
		{
			tx -= 0.005;							
		}
		if(key == 115)  // traslate down; key = 's'
		{			
			ty -= 0.005;		
		}
		if(key == 100)  // translate left; key = 'd'
		{
			tx += 0.005;	
		}
		if(key == 120) // z - direction backward.
		{
			tz -= 0.0005;
		}
		if(key == 122)
		{
			tz += 0.0005;
		}
		if(key == 114)  // rotate anti-clock wise about z-axis; key: 'r'
		{
			rotationFlag = ( rotationFlag == 1) ? 0 : 1;
		}
		if(key == 116)  // rotation clock wise; key : 't'
		{
			clockRotationFlag = ( clockRotationFlag == 1) ? 0 : 1;			
		}
		if(key == 105)  // zoom in: key 'i' 
		{
			scale += 0.5;								
		}
		if(key == 111)  // zoom out: key 'o'
		{
			scale -= .5;									
		}
		if(key == 27 ) // esc. to exit ..
		{
			exit(0);
		}
		if(key== 'm')
		{	
			if(flag2==0)
				flag2=1;
			else
				flag2=0;
		}
		
		if(key == 'p')  // lighting
		{
			if(perPolygonFlag==0)
				perPolygonFlag=1;
			else
				perPolygonFlag=0;
		}
		if( key == 99) // circular mode
		{
			circularLightFlag = 1;
			discoLightingFlag = 0;
		}
		if( key == 'v') // disco mode
		{
			circularLightFlag = 0;
			discoLightingFlag = 1;
		}
		if( key == 98) // resents disco and circular mode.
		{
			circularLightFlag = 0;
			discoLightingFlag = 0;		
		}
		if( key == '1')
		{
			quaternionX += 0.3;
		}
		if( key == '2')
		{
			quaternionX -= 0.3;
		}
		if( key == '3')
		{
			quaternionY += 0.3;
		}
		if( key == '4')
		{
			quaternionY -= 0.3;
		}
		if( key == '5')
		{
			quaternionZ += 0.3;
		}
		if(key == '6')
		{
			quaternionZ -= 0.3;	
		}
			
		
		glutPostRedisplay();
		
	
}

void mouseHandler(int btn,int state,int x,int y)
{
	if(x < 300 && y > 450 ) // bottom left
	{
		glTranslatef(100.0, 100.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light);
	}
	if(x > 300 && x <600 && y > 450 ) // bottom
	{
		glTranslatef(0.0, 100.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow_light);
	}
	if(x > 600 && y > 450 )  // bottom right
	{
		glTranslatef(-100.0, 100.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow_light);
	}
	if(x < 300 && y > 150 && y<450 )  // left
	{
		glTranslatef(100.0, 0.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, blue_light);
	}
	if(x < 300 && y < 150 )  // top left
	{
		glTranslatef(100.0, -100.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, blue_light);
	}
	if(x > 300 && x < 600 && y < 150 )  // top center
	{
		glTranslatef(0.0, -100.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, green_light);
	}
	if(x > 600 && y < 150  )  // top right
	{
		glTranslatef(-100.0, -100.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, blue_light);
	}
	
	if(x > 600 && y < 450 && y > 150  )  // top right
	{
		glTranslatef(-100.0, 0.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light);
	}
	
	if(x > 600 && y > 450 )  // top right
	{
		glTranslatef(-100.0, 0.0, -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, blue_light);
	}	
	glutPostRedisplay();
}


void draw()
{
	GLfloat rotation[16];  // stores rotation matrix. 4X4 quaternion
	glClearColor(0.0, 0.0, 0.0, 0.0);
   	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   	init_lighting();
   	glLoadIdentity();
   	glScalef(scale,scale,scale);
   	glTranslatef(tx,ty,tz);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);  // defines view volume.
  
    glPushMatrix(); 	
	if(rotationFlag == 1)
	{
		quaternion_fill_opengl_rotation_matrix(model_orientation, rotation);
		glMultMatrixf(rotation);
	}
	glColor3f(0, 0, 1);
    
	if(perPolygonFlag==1)  // per Polygon
	{
		for(int i=0;i<plyfaceCount;i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
				glNormal3f(nrml[i][0],nrml[i][1],nrml[i][2]);
				
				glVertex3f(plyVertex[plyFace[i][0]][0],plyVertex[plyFace[i][0]][1],plyVertex[plyFace[i][0]][2]);
				glVertex3f(plyVertex[plyFace[i][1]][0],plyVertex[plyFace[i][1]][1],plyVertex[plyFace[i][1]][2]);
				glVertex3f(plyVertex[plyFace[i][2]][0],plyVertex[plyFace[i][2]][1],plyVertex[plyFace[i][2]][2]);
			glEnd();
		}
	}
	else  // per Vertex.
	{
		for(int i=0;i<plyfaceCount;i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			
				glNormal3f((plyVertexNormal[plyFace[i][0]][0]/plyVertexNormal[plyFace[i][0]][3]),(plyVertexNormal[plyFace[i][0]][1]/plyVertexNormal[plyFace[i][0]][3]),(plyVertexNormal[plyFace[i][0]][2]/plyVertexNormal[plyFace[i][0]][3]));
				glVertex3f(plyVertex[plyFace[i][0]][0],plyVertex[plyFace[i][0]][1],plyVertex[plyFace[i][0]][2]);
			
				glNormal3f((plyVertexNormal[plyFace[i][1]][0]/plyVertexNormal[plyFace[i][1]][3]),(plyVertexNormal[plyFace[i][1]][1]/plyVertexNormal[plyFace[i][1]][3]),(plyVertexNormal[plyFace[i][1]][2]/plyVertexNormal[plyFace[i][1]][3]));
				glVertex3f(plyVertex[plyFace[i][1]][0],plyVertex[plyFace[i][1]][1],plyVertex[plyFace[i][1]][2]);
			
				glNormal3f((plyVertexNormal[plyFace[i][2]][0]/plyVertexNormal[plyFace[i][2]][3]),(plyVertexNormal[plyFace[i][2]][1]/plyVertexNormal[plyFace[i][2]][3]),(plyVertexNormal[plyFace[i][2]][2]/plyVertexNormal[plyFace[i][2]][3]));
				glVertex3f(plyVertex[plyFace[i][2]][0],plyVertex[plyFace[i][2]][1],plyVertex[plyFace[i][2]][2]);
			
			glEnd();
		}
	}
		
   glPopMatrix();  
   glutSwapBuffers();
	
}


void init_circularLight()  // assigns co-ordinates for light source
{
	int i, counter = 0;
	for(i = 100; i >= -100 ; i = i - 10){
		circularLight[counter][0] = 100;
		circularLight[counter][1] = i;
		counter++;
	}
	for(i = 100; i >= -100 ; i = i - 10){
		circularLight[counter][0] = i;
		circularLight[counter][1] = -100;
		counter++;
	}
	for(i = -100; i <= 100 ; i = i + 10){
		circularLight[counter][0] = -100;
		circularLight[counter][1] = i;
		counter++;
	}
	for(i = -100; i <= 100 ; i = i + 10){
		circularLight[counter][0] = i;
		circularLight[counter][1] = 100;
		counter++;
	}
	
}


void animateLight()  // Idle function.
{	
	if(circularLightFlag==1){
		if(c==80)
			c=0;
		
		c++;	
		glTranslatef(circularLight[c][0],circularLight[c][1] , -5.0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow_light);
		
		glutPostRedisplay();
		usleep(50000);	
	}

	if(discoLightingFlag == 1){		
		if(discoFlag == 0){
			discoFlag = 1;
			glTranslatef(0.0, 100.0, -5.0);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light);
		}
		else if(discoFlag == 1){
			discoFlag = 2;
			glTranslatef(10.0, -10.0, -5.0);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow_light);
		}
		else if( discoFlag == 2){
			discoFlag = 3;
			glTranslatef(-10.0, -10.0, -5.0);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, blue_light);
		}	
		else{
			discoFlag = 0;
			glTranslatef(-10.0, 10.0, -5.0);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, green_light);
		}
	
		glutPostRedisplay();
		usleep(500000);		
	}
	
}

void init_Quaternion()
{
	mouse_pressed = 0;
	mouse_x = 1;
	mouse_y = 0;
	callback_set = 0;
	downwards = make_vector(quaternionX, quaternionY, quaternionZ);
	model_position = make_vector(0, 0, 0);
	model_orientation = make_quaternion(1, 0, 0, 0);
}

int main(int argc, char** argv)
{
	
	if (argc != 2) {
		printf("Provide ply filename as argument.\nEx: ./PlyRenderer bunny.ply %d", argc);
		return 0;
	} 
	else {
		strcpy(fileName, argv[1]);
		Utility obj;
		obj.loadPlyfile(fileName);
	}
	
	init_Quaternion();
	init_circularLight();
	
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(900,900);
    glutInitWindowPosition(100,100);
    glutCreateWindow("CS606: COMPUTER GRAPHICS; Assignment 2: LUckY BuNNy ");
    glutMouseFunc(mouseHandler);
    glutIdleFunc(animateLight);
    glutKeyboardFunc(keyPressed);
    glutVisibilityFunc(visibility);
	glutDisplayFunc(draw);
	
glutMainLoop();    
return 0;
}
