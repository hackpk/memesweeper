#include "MemeField.h"
#include "assert.h"
#include <random>
#include "Vei2.h"
#include "SpriteCodex.h"

MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < width*height);
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int nSpawns = 0; nSpawns < nMemes; nSpawns++)
	{
		Vei2 gridPos;
		do
		{
			gridPos = { xDist(rng),yDist(rng) };

		} while (TileAt(gridPos).HasMeme());

		TileAt(gridPos).SpawnMeme();
	}

	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).SetNeighbourMeme(nNeighbourMemeCount(gridPos));
		}
	}
}

void MemeField::Draw(Graphics & gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).Draw(gridPos*SpriteCodex::tileSize,isMemed,gfx);
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI(0,width*SpriteCodex::tileSize,0,height*SpriteCodex::tileSize);
}

void MemeField::OnClickReveal(const Vei2 & screenPos)
{
	if (!isMemed)
	{
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x <= width && gridPos.y >= 0 && gridPos.y <= height);
		NoNeighbour(gridPos);
	}
}

void MemeField::OnClickFlagged(const Vei2 & screenPos)
{
	if (!isMemed)
	{
		const Vei2 gridPos = ScreenToGrid(screenPos);
		assert(gridPos.x >= 0 && gridPos.x <= width && gridPos.y >= 0 && gridPos.y <= height);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed())
		{
			tile.Flagged();
		}
	}
}

void MemeField::NoNeighbour(const Vei2 & gridPos)
{
	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed() && !tile.IsFlagged())
	{
		tile.Reveal();
		if (tile.HasMeme())
		{
			isMemed = true;
		}
		else if (tile.HasNoNeighbour())
		{
			const int XStart = std::max(0, gridPos.x - 1);
			const int YStart = std::max(0, gridPos.y - 1);
			const int XEnd = std::min(width - 1, gridPos.x + 1);
			const int YEnd = std::min(height - 1, gridPos.y + 1);

	
			for (Vei2 gridPos = { XStart,YStart }; gridPos.y < YEnd; gridPos.y++)
			{
				for (gridPos.x = XStart; gridPos.x < XEnd; gridPos.x++)
				{
					NoNeighbour(gridPos);
				}
			}
		}
	}
}

Vei2 MemeField::ScreenToGrid(const Vei2 & screenPos) const
{
	return screenPos / SpriteCodex::tileSize;;
}

int MemeField::nNeighbourMemeCount(const Vei2 & gridPos) const
{
	const int XStart = std::max(0, gridPos.x - 1);
	const int YStart = std::max(0, gridPos.y - 1);
	const int XEnd = std::min(width - 1, gridPos.x + 1);
	const int YEnd = std::min(height - 1, gridPos.y + 1);
	
	int count = 0;
	for (Vei2 gridPos = { XStart,YStart }; gridPos.y < YEnd; gridPos.y++)
	{
		for (gridPos.x = XStart; gridPos.x < XEnd; gridPos.x++)
		{
			if (TileAt(gridPos).HasMeme())
			{
				count++;
			}
		}
	}
	return count;
}

MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos)
{
	return Field[gridPos.y*width + gridPos.x];
}

const MemeField::Tile & MemeField::TileAt(const Vei2 & gridPos) const
{
	return Field[gridPos.y*width + gridPos.x];
}

void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw(const Vei2 & screenPos, bool Memed, Graphics & gfx) const
{
	if (!Memed)
	{
		switch (state)
		{
		case State::Hidden:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		case State::Flag:
			SpriteCodex::DrawTileButton(screenPos, gfx);
			SpriteCodex::DrawTileFlag(screenPos, gfx);
			break;
		case State::Revealed:
			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNegihbourMeme, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;
		}
	}
	else
	{
		switch (state)
		{
		case State::Hidden:
			if (HasMeme())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;
		case State::Flag:
			if (HasMeme())
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (!HasMeme())
			{
				SpriteCodex::DrawTileNumber(screenPos, nNegihbourMeme, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			break;
		}
	}
}

void MemeField::Tile::Reveal()
{
	assert(state == State::Hidden);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	
	return state == State::Revealed;
}

void MemeField::Tile::Flagged()
{
	assert(!IsRevealed());
	if (state == State::Hidden)
	{
		state = State::Flag;
	}
	else
	{
		state = State::Hidden;
	}
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flag;
}

void MemeField::Tile::SetNeighbourMeme(int memeCount)
{
	assert(nNegihbourMeme == -1);
	nNegihbourMeme = memeCount;
}

bool MemeField::Tile::HasNoNeighbour()
{
	return nNegihbourMeme == 0;
}

