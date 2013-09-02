#include "DebugDraw.h"

DebugDraw::DebugDraw(Game* game)
{
	this->game = game;
	this->m_drawFlags = 0x001F;
}

DebugDraw::~DebugDraw()
{
	//dtor
}

void 	DebugDraw::SetFlags (uint32 flags)
{
	this->m_drawFlags = flags;
}
uint32 	DebugDraw::GetFlags ()
{
	return m_drawFlags;
}
void 	DebugDraw::AppendFlags (uint32 flags)
{
	this->m_drawFlags |= flags;
}
void 	DebugDraw::ClearFlags (uint32 flags)
{
	this->m_drawFlags &~ flags;
}
void 	DebugDraw::DrawPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color)
{
	game->DrawPolygon(
		vertices,
		vertexCount,
		0xFF000000 + ((int)(0xFF * color.r) << 8) + ((int)(0xFF * color.g) << 4) + (int)(0xFF * color.b),
		0
	);
}
void 	DebugDraw::DrawSolidPolygon (const b2Vec2 *vertices, int32 vertexCount, const b2Color &color)
{
	game->DrawPolygon(
		vertices,
		vertexCount,
		0xFF000000 + ((int)(0xFF * color.r) << 8) + ((int)(0xFF * color.g) << 4) + (int)(0xFF * color.b),
		0xAA000000 + ((int)(0xFF * color.r) << 8) + ((int)(0xFF * color.g) << 4) + (int)(0xFF * color.b)
	);
}
void 	DebugDraw::DrawCircle (const b2Vec2 &center, float32 radius, const b2Color &color)
{
	game->DrawCircle(
		center,
		radius,
		0xFF000000 + ((int)(0xFF * color.r) << 8) + ((int)(0xFF * color.g) << 4) + (int)(0xFF * color.b),
		0
	);
}
void 	DebugDraw::DrawSolidCircle (const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color)
{
	game->DrawCircle(
		center,
		radius,
		0xFF000000 + ((int)(0xFF * color.r) << 8) + ((int)(0xFF * color.g) << 4) + (int)(0xFF * color.b),
		0xAA000000 + ((int)(0xFF * color.r) << 8) + ((int)(0xFF * color.g) << 4) + (int)(0xFF * color.b)
	);
}
void 	DebugDraw::DrawSegment (const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color)
{
	game->DrawSegment(
		p1,
		p2,
		0xFF000000 + ((int)(0xFF * color.r) << 8) + ((int)(0xFF * color.g) << 4) + (int)(0xFF * color.b)
	);
}
void 	DebugDraw::DrawTransform (const b2Transform &xf)
{
	game->DrawTransform(xf);
}
