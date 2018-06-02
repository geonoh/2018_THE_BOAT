#pragma once

class Object
{
protected:
	XMFLOAT3 position;
	BoundingOrientedBox bounding_box;
	XMFLOAT3 obb_extents;	// Object OBB Size
public:
	virtual void SetPosition(XMFLOAT3& input_pos);
	virtual XMFLOAT3 GetPosition();
	Object();
	~Object();
};