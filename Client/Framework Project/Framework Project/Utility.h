
#pragma once
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>


#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
<<<<<<< HEAD
#define EPSILON 1.0e-8f
extern const int gNumFrameResources;
inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

using namespace DirectX;
using namespace DirectX::PackedVector;

//3차원 벡터의 연산

namespace Vector3 {
	XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{

		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, xmvVector);

		return(xmf3Result);

	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)

	{

		XMFLOAT3 xmf3Result;

		if (bNormalize)

			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);

		else

			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar); return(xmf3Result);

	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)

	{

		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));

		return(xmf3Result);

	}

	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)

	{

		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));

		return(xmf3Result);

	}

	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)

	{

		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));

		return(xmf3Result);

	}

	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)

	{

		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));

		return(xmf3Result.x);

	}

	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)

	{

		XMFLOAT3 xmf3Result;

		if (bNormalize)

			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));

		else

			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));

		return(xmf3Result);

	}

	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)

	{

		XMFLOAT3 m_xmf3Normal;

		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));

		return(m_xmf3Normal);

	}

	inline float Length(XMFLOAT3& xmf3Vector)

	{

		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));

		return(xmf3Result.x);

	}

	inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)

	{

		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);

		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));

	}

	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)

	{

		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));

	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)

	{

		XMFLOAT3 xmf3Result;

		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));

		return(xmf3Result);

	}
=======
>>>>>>> origin/frame


#define FRAME_BUFFER_WIDTH 640
#define FRAME_BUFFER_HEIGHT 480
#define EPSILON 1.0e-8f

using namespace DirectX;
extern const int gNumFrameResources;
inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }

inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}
inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}
inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

class Utility
{
public:

	static bool IsKeyDown(int vkeyCode);

	static std::string ToString(HRESULT hr);

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}

	static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);
};

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

	std::wstring ToString()const;

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};
struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	BoundingBox Bounds;
};

struct MeshGeometry
{
	std::string Name;

	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct Light
{
	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          
	XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };
	float FalloffEnd = 10.0f;                           
	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  
	float SpotPower = 64.0f;                            
};

#define MaxLights 16

//struct MaterialConstants
//{
//	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
//	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
//	float Roughness = 0.25f;
//
//	// Used in texture mapping.
//	XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
//};
//
//// Simple struct to represent a material for our demos.  A production 3D engine
//// would likely create a class hierarchy of Materials.
//struct Material
//{
//	// Unique material name for lookup.
//	std::string Name;
//
//	// Index into constant buffer corresponding to this material.
//	int MatCBIndex = -1;
//
//	// Index into SRV heap for diffuse texture.
//	int DiffuseSrvHeapIndex = -1;
//
//	// Index into SRV heap for normal texture.
//	int NormalSrvHeapIndex = -1;
//
//	// Dirty flag indicating the material has changed and we need to update the constant buffer.
//	// Because we have a material constant buffer for each FrameResource, we have to apply the
//	// update to each FrameResource.  Thus, when we modify a material we should set 
//	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
//	int NumFramesDirty = gNumFrameResources;
//
//	// Material constant buffer data used for shading.
//	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
//	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
//	float Roughness = .25f;
//	XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
//};
//
//struct Texture
//{
//	// Unique material name for lookup.
//	std::string Name;
//
//	std::wstring Filename;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
//};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif


