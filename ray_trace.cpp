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
string mode = "phong";

//---------------------------------------
// Init function for OpenGL
//---------------------------------------
void ray_trace()
{
   // Define Phong shader
   Phong shader;
   ColorRGB shading;
   Phong phong_Arr[4];
   Sphere3D sphere_Arr[4];
   ColorRGB object_Arr[4];
   Point3D point_Arr[4];
   float ka[4], kd[4], ks[4], kp[4];

   // Define camera point
   Point3D camera;
   camera.set(0,0,-position);
   shader.SetCamera(camera);

   // Define light source
   ColorRGB color;
   color.set(200,200,200);
   Vector3D dir;
   dir.set(-1,-1,-1);
   dir.normalize();
   shader.SetLight(color, dir);

   // Set object properties
   color.set(200,0,100);
   shader.SetObject(color, 0.3, 0.4, 0.4, 10);

   // Define test sphere
   Sphere3D sphere;
   Point3D center;
   center.set(0,0,3);
   float radius = 2;
   sphere.set(center, radius);

   // Perform ray tracing
   for (int y = 0; y < YDIM; y++)
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
      ray.set(camera, point);

      // Perform sphere intersection
      Point3D p;
      Vector3D n;
      if (sphere.get_intersection(ray, p, n))
      {
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
            shader.GetShade(p, n, color);
            image[y][x][0] = color.R;
            image[y][x][1] = color.G;
            image[y][x][2] = color.B;
         }
      }

   }

    int count = 0; 

	//Open input.txt
	string name; 	
	ifstream cin;
	cin.open("testing.txt");

	while(cin >> name) 
	{
		if(name == "Camera") 
		{
			float cx, cy, cz;
			cin >> cx >> cy >> cz;
			//Set camera position - a higher number prevents stretching effect
			camera.set(cx, cy, -8000);
			for(int i = 0; i < 4; i++) 
			{
				phong_Arr[i].SetCamera(camera);
			}
		}
		else if(name == "Light") 
		{
			float lr, lg, lb, lx, ly, lz;
			cin >> lr >> lg >> lb >> lx >> ly >> lz;
			//Set color and direction of the light
			color.set(lr, lb, lg);
			dir.set(lx, ly, lz);
			for(int i = 0; i < 4; i++) 
			{
				phong_Arr[i].SetLight(color, dir);
			}
		}
		else if(name == "Object") 
		{
			float mr, mg, mb;
			cin >> 	mr >> mg >> mb >> ka[count] >> kd[count] >> ks[count] >> kp[count];
			//Counting the objects and setting their color from the input.txt
			object_Arr[count].set(mr, mg, mb);
			count++; 
		}
	}

	//Close input.txt
	cin.close();
	
        //Set sphere center (towards negative = left/down, towards positive = right/up)
	//Coordinates for the collision simulation in comments next to sphere color
	point_Arr[0].set(-100, -100, 600); //Yellow sphere (-100, -50, 200)
	point_Arr[1].set(50, -50, 100); //Blue sphere (50, -150, 100)
	point_Arr[2].set(-100, 100, 200); //Purple sphere (100, -200, 150)
	point_Arr[3].set(50, 150, 0); //Black sphere (50, 90, 0)

	//Set sphere with above centers
	sphere_Arr[0].set(point_Arr[0], 90); //Yellow z-axis
	sphere_Arr[1].set(point_Arr[1], 100); //Blue z-axis
	sphere_Arr[2].set(point_Arr[2], 100); //Purple z-axis
	sphere_Arr[3].set(point_Arr[3], 70);//Black z-axis

	//Loop to set the rays and to find if they intersect or not
	for(int y = 0; y < YDIM; y++)
	{
		for(int x = 0; x < XDIM; x++)
		{
			bool intersected = false;
			for(int i = 0; i < 4; i++)
			{	
				//Objects			
				Ray3D ray;
				Point3D intersect;
				Vector3D normal;
				Point3D intersectTemp;
				Ray3D shadow;
				Point3D pointTemp;
				Vector3D vector;

				//Sets for point and ray
				center.set(x-XDIM/2, y-YDIM/2, 0);
				ray.set(camera, center);

				//Light intersection checking
				if(sphere_Arr[i].get_intersection(ray, intersect, normal))
				{

					//Closest point to camera by checking the distance
					if(camera.distance(intersectTemp) > camera.distance(intersect) || intersected == false)
					{
						//Set shadow and the colors for the object
						shadow.set(intersect, dir);
						phong_Arr[i].SetObject(object_Arr[i], ka[i], kd[i], ks[i], kp[i]);

						//Loop to check is there's an intersection with other spheres
						for(int j = 0; j < 4; j++)
						{
							if(sphere_Arr[j].get_intersection(shadow, pointTemp, vector) && j != i)
							{
								//Use ambient term if there's an intersection
								phong_Arr[i].SetObject(object_Arr[i], ka[i], 0, 0, kp[i]);
							}
						}
		
						//Get shade at intersection
						phong_Arr[i].GetShade(intersect, normal, shading);

						//Set shade of the colors
						image[y][x][0] = shading.R;
						image[y][x][1] = shading.G;
						image[y][x][2] = shading.B;

						//Set the temp for the intersections to compare against each other
						intersectTemp.set(intersect.px, intersect.py, intersect.pz);
						intersected = true;
					}
				}
			}
		}
	}
 
}
 
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

   // Perform ray tracing
   ray_trace();
}

//---------------------------------------
// Display callback for OpenGL
//---------------------------------------
void display()
{
    /*Point3D camera;
    ColorRGB color; 
    Vector3D direction;
    Point3D point; 
    ColorRGB shade;
    //Objects as arrays
    Phong phong[4];
    ColorRGB object[4]; 
    Sphere3D sphere[4];
    Point3D center[4];
    float ka[4], kd[4], ks[4], kp[4];
    int count = 0; 

	//Open input.txt
	string name; 	
	ifstream cin;
	cin.open("testing.txt");

	while(cin >> name) 
	{
		if(name == "Camera") 
		{
			float cx, cy, cz;
			cin >> cx >> cy >> cz;
			//Set camera position - a higher number prevents stretching effect
			camera.set(cx, cy, -8000);
			for(int i = 0; i < 4; i++) 
			{
				phong[i].SetCamera(camera);
			}
		}
		else if(name == "Light") 
		{
			float lr, lg, lb, lx, ly, lz;
			cin >> lr >> lg >> lb >> lx >> ly >> lz;
			//Set color and direction of the light
			color.set(lr, lb, lg);
			direction.set(lx, ly, lz);
			for(int i = 0; i < 4; i++) 
			{
				phong[i].SetLight(color, direction);
			}
		}
		else if(name == "Object") 
		{
			float mr, mg, mb;
			cin >> 	mr >> mg >> mb >> 
				ka[count] >> kd[count] >> ks[count] >> kp[count];
			//Counting the objects and setting their color from the input.txt
			object[count].set(mr, mg, mb);
			count++; 
		}
	}

	//Close input.txt
	cin.close();
	*/
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
