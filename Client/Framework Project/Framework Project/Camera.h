#pragma once
#include <DirectXMath.h>
#include "Utility.h"
#include "Framework.h"
using namespace DirectX;
typedef unsigned long DWORD;
#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))
#define FIRST_PERSON_CAMERA 0x01
#define SPACESHIP_CAMERA 0x02 
#define THIRD_PERSON_CAMERA 0x03
#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CPlayer;

struct VS_CB_CAMERA_INFO

{

	XMFLOAT4X4 m_xmf4x4View;

	XMFLOAT4X4 m_xmf4x4Projection;

};





class Camera

{

protected:
	XMFLOAT3		 m_xmf3Position;
	XMFLOAT3		 m_xmf3Right;
	XMFLOAT3		m_xmf3Up;
	XMFLOAT3		m_xmf3Look;
	float           m_fPitch;
	float           m_fRoll;
	float           m_fYaw;
	DWORD m_nMode;
	XMFLOAT3		 m_xmf3LookAtWorld;
	XMFLOAT3		m_xmf3Offset;
	float           m_fTimeLag;
	XMFLOAT4X4		m_xmf4x4View;
	XMFLOAT4X4		m_xmf4x4Projection;
	D3D12_VIEWPORT	 m_d3dViewport;
	D3D12_RECT		 m_d3dScissorRect;
	CPlayer *m_pPlayer = NULL;

public:
	Camera();
	Camera(Camera *pCamera);
	virtual ~Camera();
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables(); virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);
	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }

	CPlayer *GetPlayer() { return(m_pPlayer); }
	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }
	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }

	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }

	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }

	float& GetRoll() { return(m_fRoll); }

	float& GetYaw() { return(m_fYaw); }



	// void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; } 

	void SetOffset(XMFLOAT3 xmf3Offset)

	{

		m_xmf3Offset = xmf3Offset; m_xmf3Position.x += xmf3Offset.x; m_xmf3Position.y += xmf3Offset.y; m_xmf3Position.z += xmf3Offset.z;

	}

	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }

	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }

	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }

	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }

	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }

	//ī�޶� xmf3Shift ��ŭ �̵��Ѵ�. 

	virtual void Move(const XMFLOAT3& xmf3Shift)

	{

		m_xmf3Position.x += xmf3Shift.x;

		m_xmf3Position.y += xmf3Shift.y;

		m_xmf3Position.z += xmf3Shift.z;

	}

	//ī�޶� x-��, y-��, z-������ ȸ���ϴ� �����Լ��̴�. 

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) { }
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) { }

};



class SpaceShipCamera : public Camera

{

public:

	SpaceShipCamera(Camera *pCamera);

	virtual ~SpaceShipCamera() { }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

};

class FirstPersonCamera : public Camera

{

public:

	FirstPersonCamera(Camera *pCamera);

	virtual ~FirstPersonCamera() { }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

};

class ThirdPersonCamera : public Camera

{

public:

	ThirdPersonCamera(Camera *pCamera);

	virtual ~ThirdPersonCamera() { }

	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);

	virtual void SetLookAt(XMFLOAT3& vLookAt);

};