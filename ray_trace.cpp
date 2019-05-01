#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#ifdef MAC
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Include ray tracing and phong shading code
#include "ray_classes.h"

// Global variables
#define XDIM 800
#define YDIM 800
unsigned char image[YDIM][XDIM][3];
float position = 2;
int num = 5;
string mode = "phong";

float myrand(float R)
{
   return (2 * R * rand()) / (RAND_MAX - R);
}

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void ray_trace()
{
   // Define Phong shader
   Phong shader;
   // Define camera point
   Point3D camera;
   camera.set(0,0,-position);
   shader.SetCamera(camera);
   float ka[num], kd[num], ks[num], kp[num];

   // Define light source
   ColorRGB color;
   //color.set(200,200,200);
   Vector3D dir;
   dir.set(-1,-1,-1);
   dir.normalize();
   shader.SetLight(color, dir);

   // Set object properties
   ColorRGB colors[num];
   //Count the objects from input.txt
    int count = 0; 

    //Open testing.txt
    string filename;     
    ifstream cin;
    cin.open("testing.txt");

    while(cin >> filename) 
    {
        if(filename == "Camera") 
        {
            float cx, cy, cz;
            cin >> cx >> cy >> cz;
            //Set camera position - a higher number prevents stretching effect
            camera.set(cx, cy, -1);
            for(int i = 0; i < num; i++) 
            {
                shader.SetCamera(camera);
            }
        }
        else if(filename == "Light") 
        {
            float lr, lg, lb, lx, ly, lz;
            cin >> lr >> lg >> lb >> lx >> ly >> lz;
            //Set color and direction of the light
            color.set(lr, lb, lg);
            dir.set(lx, ly, lz);
            for(int i = 0; i < num; i++) 
            {
                shader.SetLight(color, dir);
            }
        }
        else if(filename == "Object") 
        {
            float mr, mg, mb;
            cin >>     mr >> mg >> mb >> 
                ka[count] >> kd[count] >> ks[count] >> kp[count];
            //Counting the objects and setting their color from testing.txt
            colors[count].set(mr, mg, mb);
            count++; 
        }
    }

    //Close testing.txt
    cin.close();
     
   // Define test sphere
   Sphere3D sphere[num];
   for (int i = 0; i < num; i++)
   {
      Point3D center;
      float x_rand = myrand(1);
      float y_rand = myrand(1);
      float z_rand = 1 + rand() % 2;
      center.set(x_rand, y_rand, z_rand);
      float radius = 1 + rand() % 1;
      sphere[i].set(center, radius);
      color.set(rand() % 328, rand() % 128, rand() % 256);
   }
   

   // Perform ray tracing
   for (int y = 0; y < YDIM; y++)
   {
      bool intersected = false;
      for (int x = 0; x < XDIM; x++)
      {
         // Clear image
         image[y][x][0] = 0;
         image[y][x][1] = 0;
         image[y][x][2] = 0;

         // Define sample point on image plane
     float xpos = (x - XDIM/2) * 2.0 / XDIM;
         float ypos = (y - YDIM/2) * 2.0 / YDIM;
         Point3D point;
         point.set(xpos, ypos, 0);
      
         // Define ray from camera through image
         Ray3D ray;
         Ray3D shadow;
         ray.set(camera, point);

         // Perform sphere intersection
         Point3D p;
     Point3D intersectTemp;
         Point3D pointTemp;
         Vector3D n;
         Vector3D vect;

         for(int i = 0; i < num; i++) 
         {
            if(sphere[i].get_intersection(ray, p, n))
            { 
                if(camera.distance(intersectTemp) > camera.distance(p) || intersected == false)
        {
            shadow.set(p, dir);
             shader.SetObject(colors[i], ka[i], kd[i], ks[i], kp[i]);

            //Use loop to see if spheres intersect each other
            for(int j = 0; j < num; j++)
            {
            if(sphere[j].get_intersection(shadow, pointTemp, vect) && j != i)
            {
               //uses ambient term if an intersection occurs
               shader.SetObject(colors[i],ka[i], 0, 0, kp[i]);
            }
            }
        }
               // Display surface normal
               if (mode == "normal")
               {
                  image[y][x][0] = 127 + n.vx * 127;
                  image[y][x][1] = 127 + n.vy * 127;
                  image[y][x][2] = 127 + n.vz * 127;
               }
        
               // Calculate Phong shade
               if (mode == "phong")
                 {
                  //shader.SetObject(colors[i], 0.3, 0.4, 0.4, 10);
                  shader.GetShade(p, n, color);
                  image[y][x][0] = color.R;
                  image[y][x][1] = color.G;
                  image[y][x][2] = color.B;
                       
               }
         //intersectTemp.set(p.px, p.py, p.pz);
         //intersected = true;
            }
         }
      }
}  }

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void init()
{
   // Initialize OpenGL
   glClearColor(0.0, 0.0, 0.0, 1.0);

   // Print command menu
   cout << "Program commands:\n"
        << "   '+' - increase camera distance\n"
        << "   '-' - decrease camera distance\n"
        << "   'p' - show Phong shading\n"
        << "   'n' - show surface normals\n"
        << "   'q' - quit program\n";
   ray_trace();
}

//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display()
{
   // Display image
   glClear(GL_COLOR_BUFFER_BIT);
   glDrawPixels(XDIM, YDIM, GL_RGB, GL_UNSIGNED_BYTE, image);
   glFlush();
}

//---------------------------------------
// Keyboard callback for OpenGL
//---------------------------------------
void keyboard(unsigned char key, int x, int y)
{
   // End program
   if (key == 'q')
      exit(0);

   // Move camera position
   else if (key == '+' && position < 5)
      position = position * 1.1;
   else if (key == '-' && position > 1)
      position = position / 1.1;

   // Change display mode
   else if (key == 'n')
      mode = "normal";
   else if (key == 'p')
      mode = "phong";

   // Perform ray tracing
   ray_trace();
   glutPostRedisplay();
}

//---------------------------------------
// Main program
//---------------------------------------
int main(int argc, char *argv[])
{
   // Create OpenGL window
   glutInit(&argc, argv);
   glutInitWindowSize(XDIM, YDIM);
   glutInitWindowPosition(0, 0);
   glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
   glutCreateWindow("Ray Trace");
   init();

   // Specify callback function
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}
