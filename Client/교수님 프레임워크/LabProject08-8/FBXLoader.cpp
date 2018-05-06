#include <cstdio>
#include <fbxsdk.h>
#include "stdafx.h"
#include "FBXLoader.h"

// 頂?デ??獲得
static void GetFBXVertexData(FbxMesh* pMesh, VertexDataArray& outVertexData);

// ?テリアル?示
static void GetMatrialData(FbxSurfaceMaterial* mat);

// メッシュ情報処理(再帰関数)
static void GetMeshData(FbxNode *child, VertexDataArray& outVertexData);

// FBXデ??から頂?デ??にコンバ?ト
// ※パスは「/」でしか通らない
//const char* filename = "../datas/box.fbx";
bool LoadFBXConvertToVertexData(const char* filename, VertexDataArray& outVertexData)
{
	//==============================================================================
	// FBXオブジェクト初期化
	//==============================================================================
	// FBX?ネ?ジャ?作成
	FbxManager* pFBXManager = FbxManager::Create();

	// シ?ン作成
	FbxScene* pScene = FbxScene::Create(pFBXManager, "");

	// FBXのIO設定オブジェクト作成
	FbxIOSettings *pIO = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIO);

	// イン??トオブジェクト作成
	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");

	// フ?イルイン??ト
	if (pImporter->Initialize(filename, -1, pFBXManager->GetIOSettings()) == false)
	{
		return false;
	}

	// シ?ンへイン??ト
	if (pImporter->Import(pScene) == false)
	{
		return false;
	}

	// ※この時?でイン??トオブジェクトはいらない
	pImporter->Destroy();

	//==============================================================================
	// FBXオブジェクトの処理
	//==============================================================================
	// シ?ンのものすべてを三角化
	FbxGeometryConverter geometryConverte(pFBXManager);
	geometryConverte.Triangulate(pScene, true);

	// メッシュ情報処理
	GetMeshData(pScene->GetRootNode(), outVertexData);

	//==============================================================================
	// FBXオブジェクト色々破棄
	//==============================================================================
	pIO->Destroy();
	pScene->Destroy();
	pFBXManager->Destroy();

	//getchar();   -- ?

	return true;
}

// メッシュ情報処理(再帰関数)
void GetMeshData(FbxNode *parent, VertexDataArray& outVertexData)
{
	// メッシュだけ処理
	int numKids = parent->GetChildCount();
	for (int i = 0; i < numKids; i++)
	{
		FbxNode *child = parent->GetChild(i);

		// メッシュを見つけたら
		if (child->GetMesh())
		{
			FbxMesh* pMesh = child->GetMesh();// static_cast<FbxMesh*>(child->GetNodeAttribute());
			//printf("メッシュ発見\n");

			//printf("名前:%s\n", pMesh->GetName());
			//printf("?リゴン数:%d\n", pMesh->GetPolygonCount());
			//printf("?テリアル数:%d\n", pMesh->GetElementMaterialCount());

			//printf("コントロ?ル?イント数(頂?座標):%d\n", pMesh->GetControlPointsCount());
			//printf("UV数:%d\n", pMesh->GetTextureUVCount());

			FbxArray<FbxVector4> normals;
			pMesh->GetPolygonVertexNormals(normals);
			//printf("?線数:%d\n", normals.GetCount());

			// 頂?情報取得
			GetFBXVertexData(pMesh, outVertexData);
		}

		// ?テリアル
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
			//printf("?テリアルなし\n");
		}

		child->GetChild(0);

		// 更に子を処理
		GetMeshData(child, outVertexData);
	}
}

// 頂?デ??獲得
void GetFBXVertexData(FbxMesh* pMesh, VertexDataArray& outVertexData)
{
	// 頂?座標と?線ベクトル獲得
	std::vector<FbxVector4> positions, normals;
	FbxVector4 normal;

	// メモ:GetPolygonCount = 面数、GetPolygonSize = 頂?数
	for (int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		for (int j = 0; j < pMesh->GetPolygonSize(i); j++)
		{
			// 頂?座標
			positions.push_back(pMesh->GetControlPointAt(pMesh->GetPolygonVertex(i, j)));

			// ?線ベクトル
			pMesh->GetPolygonVertexNormal(i, j, normal);
			normals.push_back(normal);
		}
	}

	// 配列初期化
	outVertexData.resize(positions.size());

	// 頂?座標と?線情報を格?
	for (size_t i = 0; i < outVertexData.size(); ++i)
	{
		fbxsdk::FbxVector4& pos = positions[i];
		fbxsdk::FbxVector4& normal = normals[i];

		outVertexData[i].pos = DirectX::XMFLOAT3(pos.mData[0], pos.mData[2], -1 * pos.mData[1]);
		outVertexData[i].normal = DirectX::XMFLOAT4(normal.mData[0], normal.mData[2], -1 * normal.mData[1], normal.mData[3]);
	}

	// UVセットの名前配列獲得
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

// ?テリアルプロパティ獲得
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
		//printf("テクス?ャ\n");
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		for (int i = 0; i<lTextureCount; i++)
		{
			FbxFileTexture* lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(i);
			if (lFileTexture)
			{
				FbxString uvsetName = lFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = lFileTexture->GetFileName();

				//printf("UVSet名=%s\n", uvSetString.c_str());
				//printf("テクス?ャ名=%s\n", filepath.c_str());
			}
		}
		//puts("");

		//printf("レイヤ?ドテクス?ャ\n");
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

					//printf("UVSet名=%s\n", uvSetString.c_str());
					//printf("テクス?ャ名=%s\n", filepath.c_str());
				}
			}
		}
		//puts("");
	}

	return lResult;
}

// ?テリアル?示
void GetMatrialData(FbxSurfaceMaterial* mat)
{
	if (mat == nullptr)
	{
		return;
	}

	//puts("");

	if (mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		//printf("ランバ?ト?イプ\n");
	}
	else if (mat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		//printf("フォン?イプ\n");
	}

	const FbxDouble3 lEmissive = GetMaterialProperty(mat, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
	//printf("エ?ッシブカラ?:r = %f, g = %f, b = %f\n", lEmissive.mData[0], lEmissive.mData[1], lEmissive.mData[2]);

	const FbxDouble3 lAmbient = GetMaterialProperty(mat, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	//printf("アンビエントカラ?:r = %f, g = %f, b = %f\n", lAmbient.mData[0], lAmbient.mData[1], lAmbient.mData[2]);

	const FbxDouble3 lDiffuse = GetMaterialProperty(mat, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	//printf("ディフュ?ズカラ?:r = %f, g = %f, b = %f\n", lDiffuse.mData[0], lDiffuse.mData[1], lDiffuse.mData[2]);

	const FbxDouble3 lSpecular = GetMaterialProperty(mat, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	//printf("スペキュラカラ?:r = %f, g = %f, b = %f\n", lSpecular.mData[0], lSpecular.mData[1], lSpecular.mData[2]);

	FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (lTransparencyFactorProperty.IsValid())
	{
		double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
		//printf("透明度 = %lf\n", lTransparencyFactor);
	}

	FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
	if (lShininessProperty.IsValid())
	{
		double lShininess = lShininessProperty.Get<FbxDouble>();
		//printf("スペキュラ = %lf\n", lShininess);
	}
}
