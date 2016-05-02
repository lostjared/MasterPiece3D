#include "mp.h"
#include<cstdlib>
#include<ctime>

mpGame::mpGame()
{

	memset(Tiles, 0, sizeof(Tiles));
	memset(&gameblock, 0, sizeof(gameblock));
	score = lines = 0;
	srand((unsigned)time(0));
	over = false;

}


void mpGame::newGame()
{

	memset(Tiles, 0, sizeof(Tiles));
	nextBlock();
	score = lines = 0;
	over = false;

}

const bool mpGame::isOver() const
{
	return (over);
}

void mpGame::update()
{

	proc_down();

}

void mpGame::moveLeft()
{
	if(gameblock.x > 0 && gameblock.y < TILE_H-3 && Tiles[gameblock.x-1][gameblock.y+2] == 0)
		gameblock.x--;


}

void mpGame::moveRight()
{

	if(gameblock.x < TILE_W-1 && gameblock.y < TILE_H-3 && Tiles[gameblock.x+1][gameblock.y+2] == 0) gameblock.x++;

}

void mpGame::moveDown()
{

	if(gameblock.y < TILE_H-3) {
		if(Tiles[gameblock.x][gameblock.y+3] != 0) {
			if(gameblock.y <= 1) {
				// GAME OVER
				over = true;



			}
			else 
			merge_block();
			
			nextBlock();
			
		}
		else
		gameblock.y++;

	}
	else {
		merge_block();
		nextBlock();
	}

}

void mpGame::shiftBlock()
{
	int c[4];
	c[0] = gameblock.block_a[0];
	c[1] = gameblock.block_a[1];
	c[2] = gameblock.block_a[2];
	gameblock.block_a[0] = c[2];
	gameblock.block_a[1] = c[0];
	gameblock.block_a[2] = c[1];
}


void mpGame::addScore()
{
	score += 5;
	lines++;
}

int mpGame::operator()(int x, int y)
{
	if(x < TILE_W && y < TILE_H && x >= 0 && y >= 0)
	return Tiles[x][y];

	return 0;

}


void mpGame::proc_blocks()
{

	int i,z;
	for(i = 0; i < TILE_W; i++) {
		for(z = 0; z < TILE_H-1; z++) {

			if(Tiles[i][z] != 0 && Tiles[i][z+1] == 0) {
				Tiles[i][z+1] = Tiles[i][z];
				Tiles[i][z] = 0;
			}
		}
	}

}

void mpGame::merge_block()
{
	Tiles[gameblock.x][gameblock.y] = gameblock.block_a[0];
	Tiles[gameblock.x][gameblock.y+1] = gameblock.block_a[1];
	Tiles[gameblock.x][gameblock.y+2] = gameblock.block_a[2];

}


int mpGame::bounds(int x, int y) 
{
	if(x >= 0 && y >= 0)
		return 1;
	return 0;
}

const mpBlock *mpGame::getBlock() const
{
	return &gameblock;
}

void mpGame::proc_down()
{

	int i,z;

	for(i = 0; i < TILE_W; i++) {
		for(z = 0; z < TILE_H; z++) {
			int c = Tiles[i][z];
			if(c != 0) {
				if(bounds(i,z+2) && c == Tiles[i][z+1] && c == Tiles[i][z+2]) {
					Tiles[i][z] = 0;
					Tiles[i][z+1] = 0;
					Tiles[i][z+2] = 0;
					addScore();
					return;
				}

				if(bounds(i+2, z) && c == Tiles[i+1][z] && c == Tiles[i+2][z]) {
					Tiles[i][z] = 0;
					Tiles[i+1][z] = 0;
					Tiles[i+2][z] = 0;
					addScore();
					return;
				}
				if(bounds(i+2, z+2) && c == Tiles[i+1][z+1] && c == Tiles[i+2][z+2]) {
					Tiles[i][z] = 0;
					Tiles[i+1][z+1] = 0;
					Tiles[i+2][z+2] = 0;
					addScore();
					return;
				}

				if(bounds(i-2,z-2) && c == Tiles[i-1][z-1] && c == Tiles[i-2][z-2]) {
					Tiles[i][z] = 0;
					Tiles[i-1][z-1] = 0;
					Tiles[i-2][z-2] = 0;
					addScore();
					return ;
				}

				if(bounds(i-2,z) && c == Tiles[i-1][z+1] && Tiles[i-2][z+2] == c) {

					Tiles[i][z] = 0;
					Tiles[i-1][z+1] = 0;
					Tiles[i-2][z+2] = 0;
					addScore();
					return;
				}

				if(bounds(i,z-2) && c == Tiles[i+1][z-1] == c && Tiles[i+2][z-2] == c) {
					Tiles[i][z] = 0;
					Tiles[i+1][z-1] = 0;
					Tiles[i+2][z-2] = 0;
					addScore();
					return;
				}
			}
		}
	}
	proc_blocks();

}

void mpGame::nextBlock()
{

	randBlock(gameblock);
	gameblock.x = TILE_W/2;
	gameblock.y = 0;

}

void mpGame::randBlock(mpBlock &b)
{

	do
	{
		b.block_a[0] = 1+rand()%7;
		b.block_a[1] = 1+rand()%7;
		b.block_a[2] = 1+rand()%7;

	} while ( b.block_a[0] == b.block_a[1] && b.block_a[0] == b.block_a[2] );

}

const int mpGame::getScore() const
{
	return score;
}

const int mpGame::getLines() const
{
	return lines;
}

