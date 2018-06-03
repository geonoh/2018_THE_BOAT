#pragma once

class Object
{
protected:
	XMFLOAT3 position;
	BoundingOrientedBox bounding_box;
	XMFLOAT3 obb_extents;	// Object OBB Size

	void SetOBB(XMFLOAT3 xmCenter, XMFLOAT3 xmExtents, XMFLOAT4 xmOrientation);
public:
	void SetPosition(XMFLOAT3& input_pos, XMFLOAT3& extents);

	virtual XMFLOAT3 GetPosition();
	Object();
	~Object();
};