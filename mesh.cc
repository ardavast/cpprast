#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "mesh.h"

Mesh::Mesh(std::string const pathname)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(pathname, 0);
	aiMesh *mesh = scene->mMeshes[0];

	for (int i = 0; i < mesh->mNumVertices; i++) {
		struct vertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;
		vertex.pos.w = 1.0f;

		vertex.uv[0] = mesh->mTextureCoords[0][i][0];
		vertex.uv[1] = mesh->mTextureCoords[0][i][1];

		vertices.push_back(vertex);
	}
}
