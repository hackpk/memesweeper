#pragma once
#include "Vei2.h"
#include "Graphics.h"
#include "RectI.h"

class MemeField
{
public:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flag,
			Revealed
		};
	public:
		void SpawnMeme();
		bool HasMeme()const;
		void Draw(const Vei2& screenPos, Graphics& gfx);
	private:
		State state = { State::Hidden };
		bool hasMeme = false;
	};
public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx);
	RectI GetRect()const;
private:
	Tile& TileAt(const Vei2& gridPos);
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile Field[width * height];
};
