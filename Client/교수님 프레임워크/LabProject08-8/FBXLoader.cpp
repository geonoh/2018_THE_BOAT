#include <cstdio>
#include <fbxsdk.h>
#include "stdafx.h"
#include "FBXLoader.h"

// ��?�f??�l��
static void GetFBXVertexData(FbxMesh* pMesh, VertexDataArray& outVertexData);

// ?�e���A��?��
static void GetMatrialData(FbxSurfaceMaterial* mat);

// ���b�V����񏈗�(�ċA�֐�)
static void GetMeshData(FbxNode *child, VertexDataArray& outVertexData);

// FBX�f??���璸?�f??�ɃR���o?�g
// ���p�X�́u/�v�ł����ʂ�Ȃ�
//const char* filename = "../datas/box.fbx";
bool LoadFBXConvertToVertexData(const char* filename, VertexDataArray& outVertexData)
{
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
	if (pImporter->Initialize(filename, -1, pFBXManager->GetIOSettings()) == false)
	{
		return false;
	}

	// �V?���փC��??�g
	if (pImporter->Import(pScene) == false)
	{
		return false;
	}

	// �����̎�?�ŃC��??�g�I�u�W�F�N�g�͂���Ȃ�
	pImporter->Destroy();

	//==============================================================================
	// FBX�I�u�W�F�N�g�̏���
	//==============================================================================
	// �V?���̂��̂��ׂĂ��O�p��
	FbxGeometryConverter geometryConverte(pFBXManager);
	geometryConverte.Triangulate(pScene, true);

	// ���b�V����񏈗�
	GetMeshData(pScene->GetRootNode(), outVertexData);

	//==============================================================================
	// FBX�I�u�W�F�N�g�F�X�j��
	//==============================================================================
	pIO->Destroy();
	pScene->Destroy();
	pFBXManager->Destroy();

	//getchar();   -- ?

	return true;
}

// ���b�V����񏈗�(�ċA�֐�)
void GetMeshData(FbxNode *parent, VertexDataArray& outVertexData)
{
	// ���b�V����������
	int numKids = parent->GetChildCount();
	for (int i = 0; i < numKids; i++)
	{
		FbxNode *child = parent->GetChild(i);

		// ���b�V������������
		if (child->GetMesh())
		{
			FbxMesh* pMesh = child->GetMesh();// static_cast<FbxMesh*>(child->GetNodeAttribute());
			//printf("���b�V������\n");

			//printf("���O:%s\n", pMesh->GetName());
			//printf("?���S����:%d\n", pMesh->GetPolygonCount());
			//printf("?�e���A����:%d\n", pMesh->GetElementMaterialCount());

			//printf("�R���g��?��?�C���g��(��?���W):%d\n", pMesh->GetControlPointsCount());
			//printf("UV��:%d\n", pMesh->GetTextureUVCount());

			FbxArray<FbxVector4> normals;
			pMesh->GetPolygonVertexNormals(normals);
			//printf("?����:%d\n", normals.GetCount());

			// ��?���擾
			GetFBXVertexData(pMesh, outVertexData);
		}

		// ?�e���A��
		int numMat = child->GetMaterialCount();
		for (int j = 0; j < numMat; ++j)
		{
			FbxSurfaceMaterial* mat = child->GetMaterial(j);
			if (mat)
			{
				GetMatrialData(mat);
			}
		}

		if (numMat == 0)
		{
			//printf("?�e���A���Ȃ�\n");
		}

		child->GetChild(0);

		// �X�Ɏq������
		GetMeshData(child, outVertexData);
	}
}

// ��?�f??�l��
void GetFBXVertexData(FbxMesh* pMesh, VertexDataArray& outVertexData)
{
	// ��?���W��?���x�N�g���l��
	std::vector<FbxVector4> positions, normals;
	FbxVector4 normal;

	// ����:GetPolygonCount = �ʐ��AGetPolygonSize = ��?��
	for (int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		for (int j = 0; j < pMesh->GetPolygonSize(i); j++)
		{
			// ��?���W
			positions.push_back(pMesh->GetControlPointAt(pMesh->GetPolygonVertex(i, j)));

			// ?���x�N�g��
			pMesh->GetPolygonVertexNormal(i, j, normal);
			normals.push_back(normal);
		}
	}

	// �z�񏉊���
	outVertexData.resize(positions.size());

	// ��?���W��?�������i?
	for (size_t i = 0; i < outVertexData.size(); ++i)
	{
		fbxsdk::FbxVector4& pos = positions[i];
		fbxsdk::FbxVector4& normal = normals[i];

		outVertexData[i].pos = DirectX::XMFLOAT3(pos.mData[0], pos.mData[2], -1 * pos.mData[1]);
		outVertexData[i].normal = DirectX::XMFLOAT4(normal.mData[0], normal.mData[2], -1 * normal.mData[1], normal.mData[3]);
	}

	// UV�Z�b�g�̖��O�z��l��
	FbxStringList uvSetNames;
	pMesh->GetUVSetNames(uvSetNames);

	bool unmapped = false;
	int UVCount = 0;

	for (int i = 0; i < uvSetNames.GetCount(); ++i)
	{
		for (int j = 0; j < pMesh->GetPolygonCount(); ++j)
		{
			for (int k = 0; k < pMesh->GetPolygonSize(j); ++k)
			{
				FbxVector2 UV;
				pMesh->GetPolygonVertexUV(j, k, uvSetNames.GetStringAt(i), UV, unmapped);

				if (outVertexData.size() > UVCount)
				{
					outVertexData[UVCount].uv = DirectX::XMFLOAT2(UV.mData[0], UV.mData[1]);
				}
				UVCount++;
			}
		}
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
	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if (lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}
	}

	if (lProperty.IsValid())
	{
		//printf("�e�N�X?��\n");
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		for (int i = 0; i<lTextureCount; i++)
		{
			FbxFileTexture* lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(i);
			if (lFileTexture)
			{
				FbxString uvsetName = lFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = lFileTexture->GetFileName();

				//printf("UVSet��=%s\n", uvSetString.c_str());
				//printf("�e�N�X?����=%s\n", filepath.c_str());
			}
		}
		//puts("");

		//printf("���C��?�h�e�N�X?��\n");
		const int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
		for (int i = 0; i<lLayeredTextureCount; i++)
		{
			FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(i);

			const int lTextureFileCount = lLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

			for (int j = 0; j<lTextureFileCount; j++)
			{
				FbxFileTexture* lFileTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(j);
				if (lFileTexture)
				{
					FbxString uvsetName = lFileTexture->UVSet.Get();
					std::string uvSetString = uvsetName.Buffer();
					std::string filepath = lFileTexture->GetFileName();

					//printf("UVSet��=%s\n", uvSetString.c_str());
					//printf("�e�N�X?����=%s\n", filepath.c_str());
				}
			}
		}
		//puts("");
	}

	return lResult;
}

// ?�e���A��?��
void GetMatrialData(FbxSurfaceMaterial* mat)
{
	if (mat == nullptr)
	{
		return;
	}

	//puts("");

	if (mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		//printf("�����o?�g?�C�v\n");
	}
	else if (mat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		//printf("�t�H��?�C�v\n");
	}

	const FbxDouble3 lEmissive = GetMaterialProperty(mat, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
	//printf("�G?�b�V�u�J��?:r = %f, g = %f, b = %f\n", lEmissive.mData[0], lEmissive.mData[1], lEmissive.mData[2]);

	const FbxDouble3 lAmbient = GetMaterialProperty(mat, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	//printf("�A���r�G���g�J��?:r = %f, g = %f, b = %f\n", lAmbient.mData[0], lAmbient.mData[1], lAmbient.mData[2]);

	const FbxDouble3 lDiffuse = GetMaterialProperty(mat, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	//printf("�f�B�t��?�Y�J��?:r = %f, g = %f, b = %f\n", lDiffuse.mData[0], lDiffuse.mData[1], lDiffuse.mData[2]);

	const FbxDouble3 lSpecular = GetMaterialProperty(mat, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	//printf("�X�y�L�����J��?:r = %f, g = %f, b = %f\n", lSpecular.mData[0], lSpecular.mData[1], lSpecular.mData[2]);

	FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (lTransparencyFactorProperty.IsValid())
	{
		double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
		//printf("�����x = %lf\n", lTransparencyFactor);
	}

	FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
	if (lShininessProperty.IsValid())
	{
		double lShininess = lShininessProperty.Get<FbxDouble>();
		//printf("�X�y�L���� = %lf\n", lShininess);
	}
}
