#pragma once

struct vec2f
{
	float x;
	float y;
};

struct vec4f
{
	float x;
	float y;
	float x1;
	float y1;
};

struct mat3f
{
	float x;
	float y;
	float z;
};

struct ColourValue
{
	float r;
	float g;
	float b;
	float a;
};

struct Resolution
{
	float top;
	float left;
	float right;
	float bottom;
};

struct Viewport
{
	float top;
	float left;
	float right;
	float bottom;
};

typedef char* String;