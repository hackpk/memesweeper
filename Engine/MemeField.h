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
		void Draw(const Vei2& screenPos, bool memed,Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
		void Flagged();
		bool IsFlagged() const;
		void SetNeighbourMeme(int memeCount);
	private:
		State state = { State::Hidden };
		bool hasMeme = false;
		int nNegihbourMeme = -1;
	};
public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx) const;
	RectI GetRect()const;
	void OnClickReveal(const Vei2& screenPos);
	void OnClickFlagged(const Vei2& screenPos);
private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2& screenPos)const;
	int nNeighbourMemeCount(const Vei2& gridPos)const;
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	bool isMemed = false;
	Tile Field[width * height];
	
};
