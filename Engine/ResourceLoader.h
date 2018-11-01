//#pragma once
//#include "Engine\MathHelper.h"
//#include "Engine\Polygon.h"
//
//#include <iostream>
//#include <fstream>
//
//#include <assimp\scene.h>
//#include <assimp\Importer.hpp>
//#include <assimp\postprocess.h>
//#include <qdir.h>
//#include <qmessagebox.h>
//
//namespace resourceLoader
//{
//	// Loads a scene from files found in a directory (no scene graph/object implemented yet)
//	/*static void loadDirScene(QString path)
//	{
//		QDir dir(path);
//		QFileInfoList list = dir.entryInfoList();
//
//		for (int i = 0; i < list.size(); i++)
//		{
//			QFileInfo fileInfo = list[i];
//			if (fileInfo.completeSuffix() == "obj")
//				loadPolygon(fileInfo.path().toStdString());
//		}
//	}*/
//
//	static Poly* loadPolygon(std::string path)
//	{
//		Assimp::Importer importer;
//		const aiScene* aiscene = importer.ReadFile(path, aiProcess_Triangulate/* | aiProcess_JoinIdenticalVertices*/);
//		if (aiscene == nullptr)
//			return nullptr;
//		if (!aiscene->HasMeshes())
//			return nullptr;
//		aiMesh* aimesh = aiscene->mMeshes[0];
//
//		// This function only loads in a single mesh from a scene
//		Poly* poly = new Poly();
//
//		// Get the vertices
//		VertexData* vertexData = new VertexData[aimesh->mNumVertices];
//		GLuint* indexData = new GLuint[aimesh->mNumVertices];
//		for (UINT i = 0; i < aimesh->mNumVertices; i++)
//		{
//			vertexData[i].pos.x = aimesh->mVertices[i].x;
//			vertexData[i].pos.y = aimesh->mVertices[i].y - 5.0f;
//			vertexData[i].pos.z = aimesh->mVertices[i].z;
//
//			vertexData[i].normal.x = aimesh->mNormals[i].x;
//			vertexData[i].normal.y = aimesh->mNormals[i].y;
//			vertexData[i].normal.z = aimesh->mNormals[i].z;
//			indexData[i] = i;
//		}
//		poly->setVertexBuffer(vertexData, aimesh->mNumVertices);
//		poly->setIndexBuffer(indexData, aimesh->mNumVertices);
//
//		// Get the faces connectivity information (not used for rendering but stored)
//		/*std::vector<FaceData> faceData = std::vector<FaceData>(aimesh->mNumFaces);
//		for (UINT i = 0; i < aimesh->mNumFaces; i++)
//		{
//			UINT numIndices = aimesh->mFaces[i].mNumIndices;
//			faceData[i].vertices = std::vector<VertexData*>(numIndices);
//			for (UINT j = 0; j < numIndices; j++)
//			{
//				faceData[i].vertices[j] = &vertexData[aimesh->mFaces[i].mIndices[j]];
//			}
//		}*/
//
//		return poly;
//	}
//}