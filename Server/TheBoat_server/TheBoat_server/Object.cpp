#include "stdafx.h"
#include "Object.h"
#include "CHeightMapImage.h"

Object::Object()
{
	position.x = 0.f;
	position.y = 0.f;
	position.z = 0.f;

	bounding_box.Center.x = 0.f;
	bounding_box.Center.y = 0.f;
	bounding_box.Center.z = 0.f;
}

void Object::SetObbExtents(XMFLOAT3& input_scale) {
	bounding_box.Extents.x = input_scale.x;
	bounding_box.Extents.y = input_scale.y;
	bounding_box.Extents.z = input_scale.z;
}


void Object::SetPosition(XMFLOAT3& input_pos, CHeightMapImage* height_map) {

	XMFLOAT4X4 danwi(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, input_pos.x, height_map->GetHeight(input_pos.x, input_pos.z), input_pos.z, 1);
	bounding_box.Transform(bounding_box,
		DirectX::XMLoadFloat4x4(&danwi));
	XMStoreFloat4(&bounding_box.Orientation, XMQuaternionNormalize(XMLoadFloat4(&bounding_box.Orientation)));


	bounding_box.Center.x = input_pos.x;
	bounding_box.Center.y = input_pos.y;
	bounding_box.Center.z = input_pos.z;

	position.x = input_pos.x;
	position.y = input_pos.y;
	position.z = input_pos.z;

}

XMFLOAT3 Object::GetPosition() {
	return position;

}

Object::~Object()
{
}

