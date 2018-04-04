#include <cstdio>
#include <fbxsdk.h>
#include <vector>
#include "FBXUtility.h"


// ��?�f??�l��
void GetFBXVertexData(FbxMesh* pMesh);

// ?�e���A��?��
void GetMatrialData(FbxSurfaceMaterial* mat);

// ���b�V����񏈗�(�ċA�֐�)
void GetMeshData(FbxNode *child);

int main()
{
	// ���p�X�́u/�v�ł����ʂ�Ȃ�
	const char* filename = "../datas/humanoid.fbx";

	//==============================================================================
	// FBX�I�u�W�F�N�g������
	//==============================================================================
	// FBX?�l?�W��?�쐬
	FbxManager* pFBXManager = FbxManager::Create();

	// �V?���쐬
	FbxScene* pScene = FbxScene::Create(pFBXManager, "");

	// FBX��IO�ݒ�I�u�W�F�N�g�쐬
	FbxIOSettings *pIO = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIO);

	// �C��??�g�I�u�W�F�N�g�쐬
	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");

	// �t?�C���C��??�g
	if(pImporter->Initialize(filename, -1, pFBXManager->GetIOSettings()) == false)
	{
		printf("FBX�t?�C���C��??�g�G��?\n");
		printf("�G��?���e: %s\n\n", pImporter->GetStatus().GetErrorString());
		return 1;
	}

	// �V?���փC��??�g
	if(pImporter->Import(pScene) == false)
	{
		printf("FBX�V?���C��??�g�G��?\n");
		printf("�G��?���e: %s\n\n", pImporter->GetStatus().GetErrorString());
		return 1;
	}

	// �����̎�?�ŃC��??�g�I�u�W�F�N�g�͂���Ȃ�
	pImporter->Destroy();

	//==============================================================================
	// FBX�I�u�W�F�N�g�̏���
	//==============================================================================
	// �m?�h��?�����Ă݂�
	traverseScene(pScene->GetRootNode());

	// �V?���̂��̂��ׂĂ��O�p��
	FbxGeometryConverter geometryConverte(pFBXManager);
	geometryConverte.Triangulate(pScene, true);

	// ���b�V����񏈗�
	GetMeshData(pScene->GetRootNode());

	//==============================================================================
	// FBX�I�u�W�F�N�g�F�X�j��
	//==============================================================================
	pIO->Destroy();
	pScene->Destroy();
	pFBXManager->Destroy();

	printf("�S�����I��\n");
	getchar();

	return 0;
}

// ���b�V����񏈗�(�ċA�֐�)
void GetMeshData(FbxNode *parent)
{
	// ���b�V����������
	int numKids = parent->GetChildCount();
	for(int i = 0; i < numKids; i++)
	{
		FbxNode *child = parent->GetChild(i);

		// ���b�V������������
		if(child->GetMesh())
		{
			FbxMesh* pMesh = child->GetMesh();// static_cast<FbxMesh*>(child->GetNodeAttribute());
			printf("���b�V������\n");

			printf("���O:%s\n", pMesh->GetName());
			printf("?���S����:%d\n", pMesh->GetPolygonCount());
			printf("?�e���A����:%d\n", pMesh->GetElementMaterialCount());

			printf("�R���g��?��?�C���g��(��?���W):%d\n", pMesh->GetControlPointsCount());
			printf("UV��:%d\n", pMesh->GetTextureUVCount());

			FbxArray<FbxVector4> normals;
			pMesh->GetPolygonVertexNormals(normals);
			printf("?����:%d\n", normals.GetCount());

			// ��?���擾
			GetFBXVertexData(pMesh);
		}

		// ?�e���A��
		int numMat = child->GetMaterialCount();
		for(int j = 0; j < numMat; ++j)
		{
			FbxSurfaceMaterial* mat = child->GetMaterial(j);
			if(mat)
			{
				GetMatrialData(mat);
			}
		}

		if(numMat == 0)
		{
			printf("?�e���A���Ȃ�\n");
		}

		child->GetChild(0);

		// �X�Ɏq������
		GetMeshData(child);
	}
}

// ��?�f??�l��
void GetFBXVertexData(FbxMesh* pMesh)
{
	// ��?���W��?���x�N�g���l��
	std::vector<FbxVector4> positions, normals;
	FbxVector4 normal;

	// ����:GetPolygonCount = �ʐ��AGetPolygonSize = ��?��
	for(int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		for(int j = 0; j < pMesh->GetPolygonSize(i); j++)
		{
			// ��?���W
			positions.push_back(pMesh->GetControlPointAt(pMesh->GetPolygonVertex(i, j)));

			// ?���x�N�g��
			pMesh->GetPolygonVertexNormal(i, j, normal);
			normals.push_back(normal);
		}
	}

	printf("��?���W\n");
	for(unsigned int i = 0; i < positions.size(); ++i)
	{
		printf("[%d]:X = %f Y = %f Z = %f W = %f\n", i, positions[i].mData[0], positions[i].mData[1], positions[i].mData[2], positions[i].mData[2]);
	}
	puts("");

	printf("?���x�N�g��\n");
	for(unsigned i = 0; i < normals.size(); ++i)
	{
		printf("[%d]:X = %f Y = %f Z = %f W = %f\n", i, normals[i].mData[0], normals[i].mData[1], normals[i].mData[2], normals[i].mData[2]);
	}
	puts("");

	// UV�Z�b�g�̖��O�z��l��
	FbxStringList uvSetNames;
	pMesh->GetUVSetNames(uvSetNames);
	printf("UVSet�� = %d\n", uvSetNames.GetCount());

	bool unmapped = false;
	int UVCount = 0;

	for(int i = 0; i < uvSetNames.GetCount(); ++i)
	{
		printf("UVSet��[%d] = %s\n", i, uvSetNames.GetStringAt(i));

		for(int j = 0; j < pMesh->GetPolygonCount(); ++j)
		{
			for(int k = 0; k < pMesh->GetPolygonSize(j); ++k)
			{
				FbxVector2 UV;
				pMesh->GetPolygonVertexUV(j, k, uvSetNames.GetStringAt(i), UV, unmapped);
				printf("[%d]:U = %f V = %f\n", UVCount++, UV.mData[0], UV.mData[1]);
			}
		}

		puts("");
	}
}

// ?�e���A���v���p�e�B�l��
FbxDouble3 GetMaterialProperty(
	const FbxSurfaceMaterial * pMaterial,
	const char * pPropertyName,
	const char * pFactorPropertyName)
{
	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if(lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if(lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}
	}

	if(lProperty.IsValid())
	{
		printf("�e�N�X?��\n");
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		for(int i = 0; i<lTextureCount; i++)
		{
			FbxFileTexture* lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(i);
			if(lFileTexture)
			{
				FbxString uvsetName = lFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = lFileTexture->GetFileName();

				printf("UVSet��=%s\n", uvSetString.c_str());
				printf("�e�N�X?����=%s\n", filepath.c_str());
			}
		}
		puts("");

		printf("���C��?�h�e�N�X?��\n");
		const int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
		for(int i = 0; i<lLayeredTextureCount; i++)
		{
			FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(i);

			const int lTextureFileCount = lLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

			for(int j = 0; j<lTextureFileCount; j++)
			{
				FbxFileTexture* lFileTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(j);
				if(lFileTexture)
				{
					FbxString uvsetName = lFileTexture->UVSet.Get();
					std::string uvSetString = uvsetName.Buffer();
					std::string filepath = lFileTexture->GetFileName();

					printf("UVSet��=%s\n", uvSetString.c_str());
					printf("�e�N�X?����=%s\n", filepath.c_str());
				}
			}
		}
		puts("");
	}

	return lResult;
}

// ?�e���A��?��
void GetMatrialData(FbxSurfaceMaterial* mat)
{
	if(mat == nullptr)
	{
		return;
	}

	puts("");

	if(mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		printf("�����o?�g?�C�v\n");
	}
	else if(mat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		printf("�t�H��?�C�v\n");
	}

	const FbxDouble3 lEmissive = GetMaterialProperty(mat, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
	printf("�G?�b�V�u�J��?:r = %f, g = %f, b = %f\n", lEmissive.mData[0], lEmissive.mData[1], lEmissive.mData[2]);

	const FbxDouble3 lAmbient = GetMaterialProperty(mat, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	printf("�A���r�G���g�J��?:r = %f, g = %f, b = %f\n", lAmbient.mData[0], lAmbient.mData[1], lAmbient.mData[2]);

	const FbxDouble3 lDiffuse = GetMaterialProperty(mat, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	printf("�f�B�t��?�Y�J��?:r = %f, g = %f, b = %f\n", lDiffuse.mData[0], lDiffuse.mData[1], lDiffuse.mData[2]);

	const FbxDouble3 lSpecular = GetMaterialProperty(mat, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	printf("�X�y�L�����J��?:r = %f, g = %f, b = %f\n", lSpecular.mData[0], lSpecular.mData[1], lSpecular.mData[2]);

	FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if(lTransparencyFactorProperty.IsValid())
	{
		double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
		printf("�����x = %lf\n", lTransparencyFactor);
	}

	FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
	if(lShininessProperty.IsValid())
	{
		double lShininess = lShininessProperty.Get<FbxDouble>();
		printf("�X�y�L���� = %lf\n", lShininess);
	}
}
