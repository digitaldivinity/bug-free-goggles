#ifndef KORCHAGIN_OBJLOADER
#define KORCHAGIN_OBJLOADER
#include <glm/glm.hpp>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

GLfloat * loadOBJ(const char * path){
	
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file !\n");
		return NULL;
	}
	while( 1 ){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return NULL;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	vector<glm::vec3>out_vertices;
	vector<glm::vec3>out_normals;
	vector<glm::vec2>out_uvs;
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		out_vertices.push_back(vertex);
	}
	for( unsigned int i=0; i<uvIndices.size(); i++ ){
		unsigned int uvIndex = uvIndices[i];
		glm::vec3 vertex = temp_vertices[ uvIndex-1 ];
		out_uvs.push_back(vertex);
	}
	for( unsigned int i=0; i<normalIndices.size(); i++ ){
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 vertex = temp_vertices[ normalIndex-1 ];
		out_normals.push_back(vertex);
	}
	printf("v = %d\nn=%d\nt=%d\n",out_vertices.size(),out_normals.size(),out_uvs.size());
	GLfloat * vertices=new GLfloat[out_vertices.size()*8];
	printf("sizeof vertices %d\n",out_vertices.size());
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8]=out_vertices[i].x;
	}
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8+1]=out_vertices[i].y;
	}
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8+2]=out_vertices[i].z;
	}
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8+3]=out_normals[i].x;
	}
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8+4]=out_normals[i].y;
	}
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8+5]=out_normals[i].z;
	}
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8+6]=out_uvs[i].x;
	}
	for (unsigned int i=0;i<out_vertices.size();i++){
		vertices[i*8+7]=out_uvs[i].y;
	}
	return vertices;
}

class Model{
	GLfloat * vertices;
	long size;
	public:
	Model(const char * path){
		vertices=loadOBJ(path);
		size=sizeof(vertices);
	}
	~Model(){
		delete [] vertices;
	}
	GLfloat * getVertices(){
		return vertices;
	}
};
#endif
