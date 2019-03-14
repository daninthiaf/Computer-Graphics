//Daninthia Fox
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Global variables 
int xangle = 0;
int yangle = 0;
int zangle = 0;
#define SIZE 31

//surface coordinates
float Px[SIZE + 1][SIZE + 1];
float Py[SIZE + 1][SIZE + 1];
float Pz[SIZE + 1][SIZE + 1];

//surface normals
float Nx[SIZE + 1][SIZE + 1];
float Ny[SIZE + 1][SIZE + 1];
float Nz[SIZE + 1][SIZE + 1];

//red, blue, green colors
float R[SIZE + 1][SIZE + 1];
float G[SIZE + 1][SIZE + 1];
float B[SIZE + 1][SIZE + 1];

//light pos
float lx[SIZE + 1][SIZE + 1];
float ly[SIZE + 1][SIZE + 1];
float lz[SIZE + 1][SIZE + 1];

// Roate, Light 1 & Light 2 modes
#define ROTATE 1
#define L1 2
#define L2 3

int mode = ROTATE;

// Material properties
float Ka = 0.2;
float Kd = 0.4;
float Ks = 0.4;
float Kp = 0.5;

float lx1 = 1;
float ly1 = 1;
float lz1 = 1;
float lx2 = 1;
float ly2 = 1;
float lz2 = 1;
//---------------------------------------
// Initialize material properties
float myRand(float R)
{
   return (2 * R * rand()) / RAND_MAX - R;
}

float Mr1 = myRand(1);
float Mg1 = myRand(1);
float Mb1 = myRand(1);
float Mr2 = myRand(1);
float Mg2 = myRand(1);
float Mb2 = myRand(1);

float Lr1 = myRand(1);
float Lg1 = myRand(1);
float Lb1 = myRand(1);
float Lr2 = myRand(1);
float Lg2 = myRand(1);
float Lb2 = myRand(1);

#define STEP 0.1

void split(int xlow, int xhigh, int ylow, int yhigh, float radius)
{
   // Check terminating condition
   if ((xhigh > xlow+1) || (yhigh > ylow+1))
   {
      // Calculate length of diagonal
      int xmid = (xhigh + xlow) / 2;
      int ymid = (yhigh + ylow) / 2;
      float dx = Px[xhigh][yhigh] - Px[xlow][ylow];
      float dy = Py[xhigh][yhigh] - Py[xlow][ylow];
      float dz = Pz[xhigh][yhigh] - Pz[xlow][ylow];
      float length = sqrt(dx * dx + dy * dy + dz * dz) / radius;

      // Generate five midpoints with random displacements
      Px[xlow][ymid] = (Px[xlow][ylow] + Px[xlow][yhigh]) / 2 + myRand(length);
      Py[xlow][ymid] = (Py[xlow][ylow] + Py[xlow][yhigh]) / 2 + myRand(length);
      Pz[xlow][ymid] = (Pz[xlow][ylow] + Pz[xlow][yhigh]) / 2 + myRand(length);

      Px[xhigh][ymid] = (Px[xhigh][ylow] + Px[xhigh][yhigh]) / 2 + myRand(length);
      Py[xhigh][ymid] = (Py[xhigh][ylow] + Py[xhigh][yhigh]) / 2 + myRand(length);
      Pz[xhigh][ymid] = (Pz[xhigh][ylow] + Pz[xhigh][yhigh]) / 2 + myRand(length);

      Px[xmid][ylow] = (Px[xlow][ylow] + Px[xhigh][ylow]) / 2 + myRand(length);
      Py[xmid][ylow] = (Py[xlow][ylow] + Py[xhigh][ylow]) / 2 + myRand(length);
      Pz[xmid][ylow] = (Pz[xlow][ylow] + Pz[xhigh][ylow]) / 2 + myRand(length);

      Px[xmid][yhigh] = (Px[xlow][yhigh] + Px[xhigh][yhigh]) / 2 + myRand(length);
      Py[xmid][yhigh] = (Py[xlow][yhigh] + Py[xhigh][yhigh]) / 2 + myRand(length);
      Pz[xmid][yhigh] = (Pz[xlow][yhigh] + Pz[xhigh][yhigh]) / 2 + myRand(length);

      Px[xmid][ymid] = (Px[xlow][ylow] + Px[xhigh][yhigh]) / 2 + myRand(length);
      Py[xmid][ymid] = (Py[xlow][ylow] + Py[xhigh][yhigh]) / 2 + myRand(length);
      Pz[xmid][ymid] = (Pz[xlow][ylow] + Pz[xhigh][yhigh]) / 2 + myRand(length);

      //recursuve calls with split function
      split(xlow, xmid, ylow, ymid, radius);
      split(xmid, xhigh, ylow, ymid, radius);
      split(xlow, xmid, ymid, yhigh, radius);
      split(xmid, xhigh, ymid, yhigh, radius);
   }
}


//---------------------------------------
// Calculate surface normals
//---------------------------------------
void init_normals()
{
   // Initialize surface normals
   int i, j;
   for (i=0; i<=SIZE; i++)
   for (j=0; j<=SIZE; j++)
   {
      // Get tangents S and T
      float Sx = (i<SIZE) ? Px[i+1][j] - Px[i][j] : Px[i][j] - Px[i-1][j];
      float Sy = (i<SIZE) ? Py[i+1][j] - Py[i][j] : Py[i][j] - Py[i-1][j];
      float Sz = (i<SIZE) ? Pz[i+1][j] - Pz[i][j] : Pz[i][j] - Pz[i-1][j];
      float Tx = (j<SIZE) ? Px[i][j+1] - Px[i][j] : Px[i][j] - Px[i][j-1];
      float Ty = (j<SIZE) ? Py[i][j+1] - Py[i][j] : Py[i][j] - Py[i][j-1];
      float Tz = (j<SIZE) ? Pz[i][j+1] - Pz[i][j] : Pz[i][j] - Pz[i][j-1];
      
      // Calculate N = S cross T
      float Slength = sqrt(Sx*Sx + Sy*Sy + Sz*Sz);
      float Tlength = sqrt(Tx*Tx + Ty*Ty + Tz*Tz);
      if ((Slength * Tlength) > 0)
      {
         Nx[i][j] = (Sy*Tz - Sz*Ty) / (Slength * Tlength);
         Ny[i][j] = (Sz*Tx - Sx*Tz) / (Slength * Tlength);
         Nz[i][j] = (Sx*Ty - Sy*Tx) / (Slength * Tlength);
      }
   }
}


void init_surface(float Xmin, float Xmax, float Ymin, float Ymax,
		            float Wxx, float Wxy, float Wyy, float Wx, float Wy, float W1)
{
   Px[0][0] = -0.5;
   Py[0][0] = -0.5;
   Pz[0][0] = 0.0;
   Px[0][SIZE] = -0.5;
   Py[0][SIZE] = 0.5;
   Pz[0][SIZE] = 0.0;
   Px[SIZE][0] = 0.5;
   Py[SIZE][0] = -0.5;
   Pz[SIZE][0] = 0.0;
   Px[SIZE][SIZE] = 0.5;
   Py[SIZE][SIZE] = 0.5;
   Pz[SIZE][SIZE] = 0.0;
   init_normals();
   split(0, SIZE, 0, SIZE, 20);
}
   
//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void init()
{
    // Initialize OpenGL
      glClearColor(0.0, 0.0, 0.0, 1.0);   
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
      glEnable(GL_DEPTH_TEST);
      init_surface(-1.0, 1.0, -1.0, 1.0, -1, 0, -1, 0, 0, 0);
}
  
//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display()
{
   // Incrementally rotate objects
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glRotatef(xangle, 1.0, 0.0, 0.0);
   glRotatef(yangle, 0.0, 1.0, 0.0);
   glRotatef(zangle, 0.0, 0.0, 1.0);
   
   for (int i = 0; i < SIZE; ++i)
   {
      for (int j = 0; j < SIZE; ++j)
      {
	float Lx1 = lx1 - Px[i][j];
	float Ly1 = ly1 - Py[i][j];
	float Lz1 = lz1 - Pz[i][j];

	float length = sqrt((Lx1 * Lx1) + (Ly1 * Ly1) + (Lz1 * Lz1));
	//divide by length here
	Lx1 = Lx1/length;
	Ly1 = Ly1/length;
	Lz1 = Lz1/length;

	//diffuse term/dot product
	float dotProduct = (Lx1 * Nx[i][j]) + (Ly1 * Ny[i][j]) + (Lz1 * Nz[i][j]);
	
	float  Lx2 = lx2 - Px[i][j];
        float  Ly2 = ly2 - Py[i][j];
        float  Lz2 = lz2 - Pz[i][j];

	float length2 =  sqrt((Lx2 * Lx2) + (Ly2 * Ly2) + (Lz2 * Lz2));
	//divide by length here
        Lx2 = Lx2/length2;
        Ly2 = Ly2/length2;
        Lz2 = Lz2/length2;

	float dotProduct2 = (Lx2 * Nx[i][j]) + (Ly2 * Ny[i][j]) + (Lz2 * Nz[i][j]);
	//float Mr, Mg, Mb;
	//float Lr, Lg, Lb;
			
	R[i][j] = (Mr1 * Lr1 * dotProduct) + (Mr2 * Lr2 * dotProduct2);
	G[i][j] = (Mg1 * Lg1 * dotProduct) + (Mg2 * Lg2 * dotProduct2);
	B[i][j] = (Mb1 * Lb1 * dotProduct) + (Mb2 * Lb2 * dotProduct2);
      }
   }
   for (int i = 0; i < SIZE; ++i)
   {
      for (int j = 0; j < SIZE; ++j)
      {
         glBegin(GL_POLYGON);         	 
         glColor3f(R[i][j], G[i][j], B[i][j]);
         glVertex3f(Px[i][j], Py[i][j], Pz[i][j]);

         glColor3f(R[i + 1][j], G[i + 1][j], B[i + 1][j]);
         glVertex3f(Px[i + 1][j], Py[i + 1][j], Pz[i + 1][j]);

         glColor3f(R[i + 1][j + 1], G[i + 1][j + 1], B[i + 1][j + 1]);
         glVertex3f(Px[i + 1][j + 1], Py[i + 1][j + 1], Pz[i + 1][j + 1]);
         
         glColor3f(R[i][j + 1], G[i][j + 1], B[i][j + 1]);
         glVertex3f(Px[i][j + 1], Py[i][j + 1], Pz[i][j + 1]);
         glEnd();
      }
   }
   glFlush();
}

//---------------------------------------
// Keyboard callback for OpenGL
//---------------------------------------
void keyboard(unsigned char key, int x, int y)
{
   // Initialize surface
   if ((key == 'I') || (key == 'i'))
      init_surface(-1.0, 1.0, -1.0, 1.0, -1, 0, -1, 0, 0, 0);

   if((key == 'W') || (key =='w'))
     mode = ROTATE;
   else if(key == '1')
     mode = L1;
   else if(key == '2')
     mode = L2;

   // Update angles
 if(mode == ROTATE)
{
   if (key == 'x')
      xangle -= 5;
   else if (key == 'y')
      yangle -= 5;
   else if (key == 'z')
      zangle -= 5;
   else if (key == 'X')
      xangle += 5;
   else if (key == 'Y')
      yangle += 5;
   else if (key == 'Z')
      zangle += 5;
}
 else if(mode == L1)
    {
      if (key == 'R')
         Lr1 += 0.5;
      else if (key == 'r')
         Lr1 -= 0.5;
      else if (key == 'G')
         Lg1 += 0.5;
      else if (key == 'g')
         Lg1 -= 0.5;
      else if (key == 'B')
         Lb1 += 0.5;
      else if (key == 'b')
	 Lb1 -= 0.5;
      else if (key == 'X')
         lx1 += 0.2;
      else if (key == 'x')
         lx1 -= 0.2;
      else if (key == 'Y')
         ly1 += 0.2;
      else if (key == 'y')
         ly1 -= 0.2;
      else if (key == 'Z')
         lz1 += 0.2;
      else if (key == 'z')
         lz1 -= 0.2;
     }

     else if (mode == L2)
   {
      if (key == 'R')
         Lr2 += 0.5;
      else if (key == 'r')
         Lr2 -= 0.5;
      else if (key == 'G')
         Lg2 += 0.5;
      else if (key == 'g')
         Lg2 -= 0.5;
      else if (key == 'B')
         Lb2 += 0.5;
      else if (key == 'b')
         Lb2 -= 0.5;
      else if (key == 'X')
         lx2 += 0.2;
      else if (key == 'x')
         lx2 -= 0.2;
      else if (key == 'Y')
         ly2 += 0.2;
      else if (key == 'y')
         ly2 -= 0.2;
      else if (key == 'Z')
	 lz2 += 0.2;
      else if (key == 'z')
	 lz2 -= 0.2;
   } 
   // Redraw objects
   glutPostRedisplay();
}

void print_menu() 
{
    printf("Keyboard commands:\n");
    printf("Select 'I' or 'i' to change surfaces\n");
    printf("Select 'W' or 'w' for ROTATE mode\n");
    printf("   'x' - rotate x-axis -5 degrees\n");
    printf("   'X' - rotate x-axis +5 degrees\n");
    printf("   'y' - rotate y-axis -5 degrees\n");
    printf("   'Y' - rotate y-axis +5 degrees\n");
    printf("   'z' - rotate z-axis -5 degrees\n");
    printf("   'Z' - rotate z-axis +5 degrees\n");
    
    printf("Select '1' for Light 1 mode\n");
    printf("   'r'   decrease red light at a location by 0.2\n");
    printf("   'R'   increase red light at a location by 0.2\n");
    printf("   'g'   decrease green light at a location by 0.2\n");
    printf("   'G'   increase green light at a location by 0.2\n");
    printf("   'b'   decrease blue light at a location by 0.2\n");
    printf("   'B'   increase blue light at a location by 0.2\n");
    printf("   'x' - change x-axis location of selected light -0.5 degrees\n");
    printf("   'X' - change x-axis location of selected light +0.5 degrees\n");
    printf("   'y' - change y-axis location of selected light -0.5 degrees\n");
    printf("   'Y' - change y-axis location of selected light +0.5 degrees\n");
    printf("   'z' - change z-axis location of selected light -0.5 degrees\n");
    printf("   'Z' - change z-axis location of selected light +0.5 degrees\n");

    printf("Select '2' for Light 2 mode\n");
    printf("   'r'   decrease red light at a location by 0.2\n");
    printf("   'R'   increase red light at a location by 0.2\n");
    printf("   'g'   decrease green light at a location by 0.2\n");
    printf("   'G'   increase green light at a location by 0.2\n");
    printf("   'b'   decrease blue light at a location by 0.2\n");
    printf("   'B'   increase blue light at a location by 0.2\n");
    printf("   'x' - change x-axis location of selected light -0.5 degrees\n");
    printf("   'X' - change x-axis location of selected light +0.5 degrees\n");
    printf("   'y' - change y-axis location of selected light -0.5 degrees\n");
    printf("   'Y' - change y-axis location of selected light +0.5 degrees\n");
    printf("   'z' - change z-axis location of selected light -0.5 degrees\n");
    printf("   'Z' - change z-axis location of selected light +0.5 degrees\n");

}

//---------------------------------------
// Main program
//---------------------------------------
int main(int argc, char *argv[])
{
   glutInit(&argc, argv);
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(250, 250);
   glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
   glutCreateWindow("Gouraud Shading");
   init();

   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   init();
   init_surface(-1.0, 1.0, -1.0, 1.0, -1, 0, -1, 0, 0, 0);
   print_menu();
   glutMainLoop();

   return 0;
}
