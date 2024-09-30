#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"


//Declarations for test cube
#define N_CUBE_VERTICES 8
extern vec3_t cube_vertices[N_CUBE_VERTICES];
#define N_CUBE_FACES (6 * 2)
extern face_t cube_faces[N_CUBE_FACES];

//////////////////////////////////////////////////////////////////////////////
//Define a struct for dynamic size meshes, with array of vertices and faces
//////////////////////////////////////////////////////////////////////////////

typedef struct {
    vec3_t* vertices; //dynamic array of vertices
    face_t* faces;    //dynamic array of faces
    vec3_t rotation; // rotation with x, y, and z values
    vec3_t scale;    //scale with x, y, and z
    vec3_t translation; //translation with z, y and z values
} mesh_t;

void load_cube_mesh_data(void);

extern mesh_t mesh;

#endif