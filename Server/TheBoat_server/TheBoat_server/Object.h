#pragma once

class CHeightMapImage;

class Object
{
protected:
	XMFLOAT3 position;
	BoundingOrientedBox bounding_box;
	XMFLOAT3 obb_extents;	// Object OBB Size
public:
	virtual void SetObbExtents(XMFLOAT3& input_scale);
	void SetPosition(XMFLOAT3& input_pos, CHeightMapImage* height_map);
	virtual XMFLOAT3 GetPosition();
	Object();
	~Object();
};