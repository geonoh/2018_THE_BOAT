#include "stdafx.h"
#include "Object.h"


Object::Object()
{
	position.x = 0.f;
	position.y = 0.f;
	position.z = 0.f;

	bounding_box.Center.x = 0.f;
	bounding_box.Center.y = 0.f;
	bounding_box.Center.z = 0.f;
}

void Object::SetPosition(XMFLOAT3& input_pos) {
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

