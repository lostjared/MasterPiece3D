#include "game.h"
#include<sstream>
#include<cmath>
#include<mxclamp.h>
#include<mxinterface.h>
#include<mxf.h>





namespace mx
{


	using std::ostringstream;

	Uint32 timer_call(Uint32 interval, void *ptr)
	{

		gameApp *type = (gameApp*) ptr;

		type->timeUpdate();


		return interval;

	}


	gameApp::gameApp() : mxWnd(1024,768,0,0,true)
	{

		initOpenGL();
		resizeWindow(front.getSurface()->w, front.getSurface()->h);


		int i;


		mxClampResourceFile rez("img/data.mxd");


		std::cout << "\n";


		w = 640, h = 480;

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		game_speed = 700;
		setTitle("Ultimate MasterPiece3D");


		moving_away = false;
		rotate_f = 0.0f;


		try
		{


			SDL_RWops *zb;
			unsigned int siz;

			void *b = rez.accessRawBytes("background.bmp", siz);
			zb = SDL_RWFromMem(b, siz);
			intro_texture = SDL_LoadBMP_RW(zb, 0);
			SDL_FreeRW(zb);
			free(b);

			b = rez.accessRawBytes("logo.bmp", siz);
			zb = SDL_RWFromMem(b, siz);
			logo_surface = SDL_LoadBMP_RW(zb, 0);
			SDL_FreeRW(zb);
			free(b);



			    static int rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif



            game_over = SDL_CreateRGBSurface(SDL_SWSURFACE , 640, 480, SDL_GetVideoSurface()->format->BitsPerPixel, rmask, gmask, bmask, amask);


			game_font = SDL_InitFont("font/system.mxf");
			if(!game_font)
				throw mx::mxException<std::string>("  could not load font [ system.mxf ] ");


			SDL_PrintText(intro_texture, game_font, 20, 35,  SDL_MapRGB(intro_texture.getSurface()->format, 255, 255, 255), "Score: ");




			for(i = 0; i < 8; i++)
			{

				SDL_RWops *opz;
				unsigned int size;


				ostringstream stream;
				stream << "bblock" << (i+1) << ".bmp";

				cout << "Loading texture: " << stream.str() << "\n";

				void *buffer = rez.accessRawBytes(stream.str(), size);
				opz = SDL_RWFromMem(buffer, size);

				SDL_Surface *temp = SDL_LoadBMP_RW(opz, 0);
				if(temp==0) throw mx::mxException<std::string>(" error could not load texture surface ");
				game_blocks[i] = temp;



				generateTexture(tex_block[i],  game_blocks[i] );
				SDL_FreeRW(opz);
				free(buffer);
			}

			 generateTexture(itext_id , intro_texture );
			 generateTexture(g_id , game_over );
		}
		catch( mx::mxException<std::string> &mxe )
		{

			std::cerr << "caught exception: ";
			mxe.printError(std::cerr);
		}

		g_scr = 1;


	}

	void gameApp::startGame()
	{
		mp_game.newGame();
		SDL_AddTimer( game_speed, timer_call, this );
		rot_x=0, rot_y=0, rot_z=0;
		g_scr = 0;
	}

	gameApp::~gameApp()
	{
		int i;



		for(i = 0; i < 8; i++)
		{

			if(tex_block[i])
				releaseTexture(tex_block[i]);

		}

		releaseTexture(itext_id);

		if(game_font)
		SDL_FreeFont(game_font);

	}

	void gameApp::initOpenGL()
	{

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth( 1.0f );
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LEQUAL );
    	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		glShadeModel( GL_SMOOTH );


	}


	void gameApp::startScreen()
	{



		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		static mx::mxSurfacePainter paint(&game_over);
		game_over.copySurface(logo_surface, 0, 0);

		SDL_Rect rc = { 5,5,630,100 };
		SDL_FillRect(game_over, &rc, 0);

		if(moving_away == true) {

			glRotatef(rotate_f, 1.0f, 1.0f, 1.0f);

			rotate_f += 3.0f;

			if(rotate_f > 80.5f)
			{

				rotate_f = 0.0f;
				g_scr = 0;
				moving_away = false;
				startGame();
				g_scr = 0;

			}


		}

		glTranslatef(0.0f,0.0f,-4.0f);

		paint.setFont(game_font);

		if(g_scr == 1 && mp_game.isOver()) {



			std::ostringstream stream;
			stream << "Game Over Your score was: " << mp_game.getScore() << " you cleared: " << mp_game.getLines() << "\nPress Escape to quit, enter to play agian";
			paint.printText(25, 25, mx::Color(255, 0, 0),stream.str());
		}
		else if(g_scr == 1)
		{

			paint.printText(10,30,mx::Color(rand()%255,rand()%255,rand()%255), "New Game Press Enter, escape to quit");

		}


		glBindTexture(GL_TEXTURE_2D, g_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, game_over.getSurface()->w, game_over.getSurface()->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, game_over.getSurface()->pixels);
		drawCube(g_id);
		SDL_GL_SwapBuffers();



	}

	void gameApp::renderScreen()
	{

		if(mp_game.isOver())
			g_scr = 1;



		if(g_scr == 1)
		{
			startScreen();
			return;
		}




		mp_game.update();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glLoadIdentity();


		glTranslatef(.8f,.9f, -3.0f);
		drawScores();

		glLoadIdentity();


		glTranslatef(-25.0f,30.0f,-90.0f);


		if(rot_x!=0)
			glRotatef(rot_x, 1.0f, 0.0f, 0.0f);

		if(rot_y!=0)
			glRotatef(rot_y, 0.0f, 1.0f,0.0f);

		if(rot_z!=0)
			glRotatef(rot_z, 0.0f, 0.0f, 1.0f);



		for(int i = 0; i < TILE_W; i++)
		{
			for(int z = 0; z < TILE_H; z++)
			{

				int cube = mp_game(i,z);
				const mpBlock *block = mp_game.getBlock();

				if(cube != 0)
				drawCube(tex_block[cube]);
				else if(block->x == i && block->y == z)
				{

					drawCube(tex_block[block->block_a[0]]);
					glTranslatef(0.0f, -2.5f, 0.0f);
					drawCube(tex_block[block->block_a[1]]);
					glTranslatef(0.0f, -2.5f, 0.0f);
					drawCube(tex_block[block->block_a[2]]);
					glTranslatef(0.0f, -2.5f, 0.0f);

					z += 2;
					continue;

				}



				glTranslatef(0.0f, -2.5f, 0.0f);

			}

			glTranslatef(3.0f, 62.5, 0.0f);

		}


		SDL_GL_SwapBuffers();

	}


	void gameApp::eventPassed(SDL_Event &e)
	{


		switch(e.type)
		{
		case SDL_VIDEORESIZE:
		{
			resizeWindow(e.resize.w, e.resize.h);

		}
		break;

		case SDL_KEYDOWN:
		{

			if(g_scr == 1)
			{
				if(e.key.keysym.sym == SDLK_ESCAPE)
					quit();
				if(e.key.keysym.sym == SDLK_RETURN)
				{
					//startGame();
					//g_scr = 0;
					moving_away = true;
				}

				return;
			}

			switch(e.key.keysym.sym)
			{
			case SDLK_LEFT:
			mp_game.moveLeft();
			break;
			case SDLK_RIGHT:
			mp_game.moveRight();
			break;
			case SDLK_DOWN:
			mp_game.moveDown();
			break;
			case SDLK_UP:
			case 'a':
			case 'A':
			case 's':
			case 'S':
			mp_game.shiftBlock();
			break;

			case 'z':
			case 'Z':
			rot_x += 1.0f;
			break;
			case 'X':
			case 'x':
			rot_x -= 1.0f;
			break;


			case 'c':
			case 'C':
			rot_y += 1.0f;
			break;

			case 'v':
			case 'V':
			rot_y -= 1.0f;
			break;

			case 'd':
			case 'D':
			rot_z += 1.0f;
			break;

			case 'f':
			case 'F':
			rot_z -= 1.0f;
			break;
			default:
			break;
			}

		}
		break;
		}



		mxWnd::eventPassed(e);

	}

	void gameApp::generateTexture(GLuint &texture_identifier, const mxSurface &surface)
	{


		if(surface.getSurface()==0)
		{

			throw mx::mxException<std::string>(" error passing null surface to gen texture ");
			return ;
		}



		int mode;


		if(surface.getSurface()->format->BytesPerPixel == 3)
			mode = GL_RGB;
			//mode = GL_BGR;
		else
			if(surface.getSurface()->format->BytesPerPixel == 4)
				mode = GL_RGBA;
		else
		{

			throw mx::mxException<std::string>(" cannot create texture must be ethier 32bit or 24bit ");

		}

		glGenTextures(1, &texture_identifier);
		glBindTexture(GL_TEXTURE_2D, texture_identifier);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface.getSurface()->w, surface.getSurface()->h, 0, mode, GL_UNSIGNED_BYTE, surface.getSurface()->pixels);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glEnable(GL_TEXTURE_2D);

	}



	void gameApp::resizeWindow(int w, int h)
	{

		if(h == 0)
			h = 1;


		GLfloat ratio = 0.0f;
		ratio = static_cast<GLfloat>( w / h );
		glViewport( 0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, ratio, 0.1f, 200.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		this->w = w, this->h = h;


	}

	bool gameApp::releaseTexture(const GLuint &tid)
	{

		std::cout << "released texture: " << tid << "\n";
		glDeleteTextures(1, &tid);

		return true;

	}


	// inverted
	void gameApp::drawCube(const GLuint &tid)
	{
		glBindTexture(GL_TEXTURE_2D, tid);

		glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f, -1.0f, 1.0f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f, -1.0f, 1.0f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f,  1.0f, 1.0f );
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f,  1.0f, 1.0f );

		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f, -1.0f );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f,  1.0f, -1.0f );
      	glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f,  1.0f, -1.0f );
      	glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f, -1.0f, -1.0f );

      	glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.0f,  1.0f, -1.0f );
     	glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f,  1.0f,  1.0f );
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f(  1.0f,  1.0f,  1.0f );
      	glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  1.0f,  1.0f, -1.0f );

		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f, -1.0f, -1.0f );
      	glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f, -1.0f, -1.0f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f, -1.0f,  1.0f );
      	glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f,  1.0f );

		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( 1.0f, -1.0f, -1.0f );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( 1.0f,  1.0f, -1.0f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f( 1.0f,  1.0f,  1.0f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f( 1.0f, -1.0f,  1.0f );

		glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f, -1.0f, -1.0f );
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f,  1.0f );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f,  1.0f,  1.0f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.0f,  1.0f, -1.0f );
		glEnd( );

	}


	void gameApp::timeUpdate()
	{

		mp_game.moveDown();

	}

	void gameApp::drawScores()
	{


		std::ostringstream ss, stream;


		ss << "Score  " << mp_game.getScore();


		SDL_Rect rc = { 20, 25, intro_texture.getSurface()->w-50, intro_texture.getSurface()->h-50 };
		SDL_FillRect(intro_texture, &rc, 0);



		SDL_PrintText(intro_texture, game_font, 20, 35,  SDL_MapRGB(intro_texture.getSurface()->format, 255, 255, 255), ss.str().c_str());



		stream << "Lines " << mp_game.getLines();


		SDL_PrintText(intro_texture, game_font, 20, 55,  SDL_MapRGB(intro_texture.getSurface()->format, 255, 255,
		 255), stream.str().c_str());


		glBindTexture(GL_TEXTURE_2D, itext_id);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, intro_texture.getSurface()->w, intro_texture.getSurface()->h, 0, GL_BGR, GL_UNSIGNED_BYTE, intro_texture.getSurface()->pixels);
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -0.3f, -0.3f, 0.3f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  0.3f, -0.3f, 0.3f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  0.3f,  0.3f, 0.3f );
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -0.3f,  0.3f, 0.3f );
		glEnd();


	}

}
