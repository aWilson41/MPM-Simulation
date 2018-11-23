#pragma once

class Renderer;

class AbstractMapper
{
public:
	virtual void update() { }

	virtual void draw(Renderer* ren) { }
};