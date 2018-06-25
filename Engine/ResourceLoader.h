#pragma once
#include "Engine\MathHelper.h"
#include "Engine\Polygon.h"

#include <iostream>
#include <fstream>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <qdir.h>
#include <qmessagebox.h>

namespace resourceLoader
{
	// Loads a scene from files found in a directory (no scene graph/object implemented yet)
	/*static void loadDirScene(QString path)
	{
		QDir dir(path);
		QFileInfoList list = dir.entryInfoList();

		for (int i = 0; i < list.size(); i++)
		{
			QFileInfo fileInfo = list[i];
			if (fileInfo.completeSuffix() == "obj")
				loadPolygon(fileInfo.path().toStdString());
		}
	}*/

	static Poly* loadPolygon(std::string path)
	{
		Assimp::Importer importer;
		const aiScene* aiscene = importer.ReadFile(path, aiProcess_Triangulate/* | aiProcess_JoinIdenticalVertices*/);
		if (aiscene == nullptr)
			return nullptr;
		if (!aiscene->HasMeshes())
			return nullptr;
		aiMesh* aimesh = aiscene->mMeshes[0];

		// This function only loads in a single mesh from a scene
		Poly* poly = new Poly();

		// Get the vertices
		VertexData* vertexData = new VertexData[aimesh->mNumVertices];
		GLuint* indexData = new GLuint[aimesh->mNumVertices];
		for (UINT i = 0; i < aimesh->mNumVertices; i++)
		{
			vertexData[i].pos.x = aimesh->mVertices[i].x;
			vertexData[i].pos.y = aimesh->mVertices[i].y - 5.0f;
			vertexData[i].pos.z = aimesh->mVertices[i].z;

			vertexData[i].normal.x = aimesh->mNormals[i].x;
			vertexData[i].normal.y = aimesh->mNormals[i].y;
			vertexData[i].normal.z = aimesh->mNormals[i].z;
			indexData[i] = i;
		}
		poly->setVertexBuffer(vertexData, aimesh->mNumVertices);
		poly->setIndexBuffer(indexData, aimesh->mNumVertices);

		// Get the faces connectivity information (not used for rendering but stored)
		/*std::vector<FaceData> faceData = std::vector<FaceData>(aimesh->mNumFaces);
		for (UINT i = 0; i < aimesh->mNumFaces; i++)
		{
			UINT numIndices = aimesh->mFaces[i].mNumIndices;
			faceData[i].vertices = std::vector<VertexData*>(numIndices);
			for (UINT j = 0; j < numIndices; j++)
			{
				faceData[i].vertices[j] = &vertexData[aimesh->mFaces[i].mIndices[j]];
			}
		}*/

		return poly;
	}

	// Loads in file of the format
	/*static SpringMassMesh* loadSpringMesh(std::string path)
	{
		std::ifstream input;
		input.open(path);
		if (input.fail())
			return nullptr;

		SpringMassMesh* poly = new SpringMassMesh();
		unsigned int count = 0;
		input >> count;
		std::vector<VertexData> vertexData = std::vector<VertexData>(count);
		for (unsigned int i = 0; i < count; i++)
		{
			VertexData vData;
			input >> vData.pos.x >> vData.pos.y >> vData.pos.z;
			vertexData[i] = vData;
		}

		input >> count;
		std::vector<GLuint> indexData = std::vector<GLuint>(count * 2);
		for (unsigned int i = 0; i < count * 2; i += 2)
		{
			input >> indexData[i] >> indexData[i + 1];
		}

		input.close();

		poly->setVertexBuffer(vertexData);
		poly->setIndexBuffer(indexData);
		poly->setupSpringMesh();
		return poly;
	}*/

	static void addEdge(int i1, int i2, std::vector<GLuint>& indices)
	{
		for (int i = 0; i < indices.size(); i += 2)
		{
			int j1 = indices[i];
			int j2 = indices[i + 1];
			if ((j1 == i1 && j2 == i2) || (j2 == i1 && j1 == i2))
				return;
		}
		indices.push_back(i1);
		indices.push_back(i2);
	}
	static SpringMassMesh* loadTetgenMesh(std::string path, float scale = 1.0f)
	{
		// Read the vertices
		std::ifstream input;
		input.open(path + ".node");
		if (input.fail())
			return nullptr;

		SpringMassMesh* poly = new SpringMassMesh();
		int trash = -1;
		unsigned int vertexCount = 0;
		input >> vertexCount >> trash >> trash >> trash;
		VertexData* vertexData = new VertexData[vertexCount];
		poly->neighbors = std::vector<std::vector<Face*>>(vertexCount);
		for (unsigned int i = 0; i < vertexCount; i++)
		{
			VertexData vData;
			input >> trash >> vData.pos.x >> vData.pos.y >> vData.pos.z;
			vData.pos *= scale;
			vertexData[i] = vData;
		}
		input.close();

		// Read the edges
		input.open(path + ".ele");
		if (input.fail())
			return nullptr;

		unsigned int springIndexCount = -1;
		input >> springIndexCount >> trash >> trash;
		std::vector<GLuint> springIndexData;
		for (unsigned int i = 0; i < springIndexCount; i++)
		{
			int j = i * 12;
			int index[4];
			input >> trash >> index[0] >> index[1] >> index[2] >> index[3];
			index[0]--;
			index[1]--;
			index[2]--;
			index[3]--;

			// Kinda inefficient (but sufficient for the size of meshes I can simulate)
			addEdge(index[0], index[1], springIndexData);
			addEdge(index[0], index[2], springIndexData);
			addEdge(index[0], index[3], springIndexData);
			addEdge(index[1], index[2], springIndexData);
			addEdge(index[1], index[3], springIndexData);
			addEdge(index[2], index[3], springIndexData);
		}
		input.close();

		// Read faces
		input.open(path + ".face");
		if (input.fail())
			return nullptr;

		unsigned int faceCount = -1;
		input >> faceCount >> trash;
		GLuint* indexData = new GLuint[faceCount * 3];
		Face* faceData = new Face[faceCount];
		for (unsigned int i = 0; i < faceCount; i++)
		{
			int index[3];
			input >> trash >> index[0] >> index[1] >> index[2] >> trash;
			index[2]--;
			index[1]--;
			index[0]--;
			indexData[i * 3] = index[2];
			indexData[i * 3 + 1] = index[1];
			indexData[i * 3 + 2] = index[0];

			// Add the face
			Face* face = &faceData[i];
			face->v1 = &vertexData[index[0]];
			face->v2 = &vertexData[index[1]];
			face->v3 = &vertexData[index[2]];

			// Add reference to face from vertex
			poly->neighbors[index[2]].push_back(face);
			poly->neighbors[index[1]].push_back(face);
			poly->neighbors[index[0]].push_back(face);
		}
		input.close();

		poly->setVertexBuffer(vertexData, vertexCount);
		poly->setIndexBuffer(indexData, faceCount * 3);
		poly->setFaceData(faceData, faceCount);
		//poly->setupSpringMesh();
		poly->setupSpringMesh(springIndexData.data(), static_cast<int>(springIndexData.size()));
		
		return poly;
	}
}