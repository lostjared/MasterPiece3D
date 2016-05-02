#ifndef __MX__H__GAME
#define __MX__H__GAME

#include<mx.h>
#include "mp.h"




namespace mx
{



	class gameApp : public mxWnd {


	public:
		gameApp();
		virtual ~gameApp();
		virtual void renderScreen();
		virtual void eventPassed(SDL_Event &e);
		virtual void resizeWindow(int x, int y);
		void initOpenGL();

		void generateTexture(GLuint &, const mxSurface &surface);
		bool  releaseTexture(const GLuint &tid);
		void drawCube(const GLuint &tid);
		void drawScores();
		void timeUpdate();
		void startScreen();
		void startGame();
	private:
		mxSurface logo_surface;
		mxSurface intro_texture, game_over;
		mxSurface game_blocks[8];
		GLuint itext_id, tex_block[8], g_id;
		mpGame mp_game;
		int game_speed;
		float rot_x,  rot_y, rot_z;
		int w, h;
		SDL_Font *game_font;
		int g_scr;
		bool moving_away;
		float rotate_f;


	};













}















#endif



