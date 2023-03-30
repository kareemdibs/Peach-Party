#if defined(__APPLE__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "freeglut.h"
#include "GameController.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "SoundFX.h"
#include "SpriteManager.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <cstdlib>
#include <algorithm>
using namespace std;

/*
spriteWidth = .67
spritesPerRow = 16

RowWidth = spriteWidth*spritesPerRow = 10.72
PixelWidth = RowWidth/256 = .041875
newSpriteWidth = PixelWidth * NumPixels

spriteHeight = .54
spritesPerCol = 16

ColHeight = spriteHeight*spritesPerCol = 8.64

PixelHeight = ColHeight/256 = .03375

newSpriteHeight = PixelHeight * NumPixels
*/

static const int WINDOW_WIDTH = 768; //1024;
static const int WINDOW_HEIGHT = 768;

static const int PERSPECTIVE_NEAR_PLANE = 4;
static const int PERSPECTIVE_FAR_PLANE	= 22;

static const double VISIBLE_MIN_X = -2.39;
static const double VISIBLE_MAX_X = 2.1; // 2.39;
static const double VISIBLE_MIN_Y = -2.1;
static const double VISIBLE_MAX_Y = 1.9;
static const double VISIBLE_MIN_Z = -20;
// static const double VISIBLE_MAX_Z = -6;

static const double FONT_SCALEDOWN = 760.0;

static const double SCORE_Y = 3.8;
static const double SCORE_Z = -10;

static const int MS_PER_FRAME = 5;

int GameController::m_ms_per_tick = kDefaultMsPerTick;

static void convertToGlutCoords(double x, double y, double& gx, double& gy, double& gz);
static void drawPrompt(string mainMessage, string secondMessage);
static void drawScoreAndLives(string);

enum GameController::GameControllerState : int {
    welcome, init, cleanup, makemove, animate, gameover, prompt, quit, not_applicable
};

void GameController::initDrawersAndSounds()
{

	struct SpriteInfo
	{
		int imageID;
		int frameNum;
		string tgaFileName;
		string imageName;
	};

	const SpriteInfo drawers[] = {
		{ IID_PEACH, 0, "peach1.tga", "PEACH" },
		{ IID_PEACH, 1, "peach2.tga", "PEACH" },
		{ IID_YOSHI, 0, "yoshi1.tga", "YOSHI" },
		{ IID_YOSHI, 1, "yoshi2.tga", "YOSHI" },
		{ IID_BLUE_COIN_SQUARE, 0, "blue_coin_square.tga", "BLUE COIN" },
		{ IID_RED_COIN_SQUARE, 0, "red_coin_square.tga", "RED COIN" },
		{ IID_DIR_SQUARE, 0, "dir_square.tga", "DIRECTION SQUARE" },
		{ IID_EVENT_SQUARE, 0, "event_square.tga", "EVENT SQUARE" },
		{ IID_BANK_SQUARE, 0, "bank_square.tga", "BANK SQUARE" },
		{ IID_STAR_SQUARE, 0, "star_square.tga", "STAR SQUARE" },
		{ IID_DROPPING_SQUARE, 0, "dropping_square.tga", "DROPPING SQUARE" },
		{ IID_BOWSER, 0, "bowser1.tga", "BOWSER" },
		{ IID_BOWSER, 1, "bowser2.tga", "BOWSER" },
		{ IID_BOO, 0, "boo1.tga", "BOO" },
		{ IID_BOO, 1, "boo2.tga", "BOO" },
		{ IID_VORTEX, 0, "vortex1.tga", "VORTEX" },
		{ IID_VORTEX, 1, "vortex2.tga", "VORTEX" },
	};

	m_soundMap = {
		{ SOUND_PLAYER_FIRE               , "fire.wav" },
		{ SOUND_GIVE_COIN                 , "give_coins.wav" },
		{ SOUND_TAKE_COIN                 , "take_coins.wav" },
		{ SOUND_GIVE_STAR                 , "give_star.wav" },
		{ SOUND_WITHDRAW_BANK             , "withdraw_bank.wav" },
		{ SOUND_DEPOSIT_BANK              , "deposit_bank.wav" },
		{ SOUND_PLAYER_TELEPORT           , "player_teleport.wav" },
		{ SOUND_GIVE_VORTEX               , "give_vortex.wav" },
		{ SOUND_DROPPING_SQUARE_CREATED   , "dropping_sq_created.wav" },
		{ SOUND_DROPPING_SQUARE_ACTIVATE  , "dropping_sq_activate.wav" },
		{ SOUND_BOWSER_ACTIVATE           , "bowser_activate.wav" },
		{ SOUND_BOO_ACTIVATE              , "boo_activate.wav" },
		{ SOUND_HIT_BY_VORTEX             , "hit_by_vortex.wav" },
		{ SOUND_THEME                     , "mario_party_theme.wav" },
		{ SOUND_GAME_FINISHED             , "game_over.wav" },
	};

	string path = m_gw->assetPath();

	for (const auto& d : drawers)
	{
		if (!m_spriteManager.loadSprite(path + d.tgaFileName, d.imageID, d.frameNum))
		{
			cerr << "***** Error loading sprite: " << (path + d.tgaFileName) << endl;
			setGameState(quit);
		}
		m_imageNameMap[d.imageID] = d.imageName;
	}
}

bool GameController::passesThruWhenSingleStepping(int key) const
{
    static set<int> passThruKeys = {
        'a', 'd', 'w', 's', KEY_PRESS_TAB, '`', KEY_PRESS_LEFT, KEY_PRESS_RIGHT,
        KEY_PRESS_UP, KEY_PRESS_DOWN, KEY_PRESS_ENTER, '\\'
    };
    return passThruKeys.find(key) != passThruKeys.end();
}

static void doSomethingCallback()
{
	Game().doSomething();
}

static void reshapeCallback(int w, int h)
{
	Game().reshape(w, h);
}

static void keyboardEventCallback(unsigned char key, int x, int y)
{
	Game().keyboardEvent(key, x, y);
}

static void specialKeyboardEventCallback(int key, int x, int y)
{
	Game().specialKeyboardEvent(key, x, y);
}

void GameController::timerFuncCallback(int)
{
	Game().doSomething();
    glutTimerFunc(MS_PER_FRAME, timerFuncCallback, 0);
}

#if defined(__APPLE__)
void windowCloseCallback()
{
    SoundFX().abortClip();
}
#endif

void GameController::run(int argc, char* argv[], GameWorld* gw, string windowTitle)
{
	gw->setController(this);
	m_gw = gw;
	setGameState(welcome);
    m_singleStep = false;
    m_postInitPreCleanup = false;
	m_curIntraFrameTick = 0;
	m_winner = GWSTATUS_CONTINUE_GAME;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(windowTitle.c_str());

    initDrawersAndSounds();  // won't work unless *after* window created

	glutKeyboardFunc(keyboardEventCallback);
	glutSpecialFunc(specialKeyboardEventCallback);
	glutReshapeFunc(reshapeCallback);
	glutDisplayFunc(doSomethingCallback);
	glutTimerFunc(MS_PER_FRAME, timerFuncCallback, 0);
#if defined(__APPLE__)
    glutWMCloseFunc(windowCloseCallback);
#endif

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
	delete m_gw;
    reportLeakedGraphObjects();
}

void GameController::keyboardEvent(unsigned char key, int /* x */, int /* y */)
{
	switch (key)
	{
		default:
            m_keysHit.push_back(key);
            break;
		case 'f':
            m_singleStep = true;
            break;
		case 'r':
            m_singleStep = false;
            break;
		case '\x03':  // CTRL-C
		case KEY_PRESS_ESCAPE:
            setGameState(quit);
            break;
	}
}

void GameController::specialKeyboardEvent(int key, int /* x */, int /* y */)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:	 m_keysHit.push_back(KEY_PRESS_LEFT);  break;
		case GLUT_KEY_RIGHT: m_keysHit.push_back(KEY_PRESS_RIGHT); break;
		case GLUT_KEY_UP:	 m_keysHit.push_back(KEY_PRESS_UP);    break;
		case GLUT_KEY_DOWN:	 m_keysHit.push_back(KEY_PRESS_DOWN);  break;
	}
}

void GameController::playSound(int soundID)
{
	if (soundID == SOUND_NONE)
		return;

	auto p = m_soundMap.find(soundID);
	if (p != m_soundMap.end())
	{
		string path = m_gw->assetPath();
		SoundFX().playClip(path + p->second);
	}
}

void GameController::setGameState(GameControllerState s)
{
    if (m_gameState != quit)
        m_gameState = s;
}

void GameController::doSomething()
{
	switch (m_gameState)
	{
		case not_applicable:
			break;
		case welcome:
			playSound(SOUND_THEME);
			m_mainMessage = "Welcome to Peach Party!";
            m_secondMessage = "Press 1 or 2 or ... or 9 to choose board";
			setGameState(prompt);
			m_nextStateAfterPrompt = init;
			break;
		case makemove:
			m_curIntraFrameTick = ANIMATION_POSITIONS_PER_TICK;
			m_nextStateAfterAnimate = not_applicable;
			{
				int status = m_gw->move();
				if (status == GWSTATUS_PEACH_WON  ||  status == GWSTATUS_YOSHI_WON)
				{
					m_winner = status;
					m_nextStateAfterAnimate = gameover;
				}
                else if (status == GWSTATUS_NOT_IMPLEMENTED)
                {
                    m_mainMessage = "Game not implemented!";
                    m_secondMessage = "Press ESC to quit...";
                    setGameState(prompt);
                    m_nextStateAfterPrompt = quit;
                    break;
                }
			}
			setGameState(animate);
			break;
		case animate:
			displayGamePlay();
			if (m_curIntraFrameTick-- <= 0)
			{
				if (m_nextStateAfterAnimate != not_applicable)
					setGameState(m_nextStateAfterAnimate);
				else if (!m_singleStep)
                    setGameState(makemove);
                else {
                    int key;
                    if (getKeyIfAny(key))
                    {
                        if (passesThruWhenSingleStepping(key))
                            putBackKey(key);
                        setGameState(makemove);
                    }
                }
			}
			break;
		case cleanup:
            if (m_postInitPreCleanup)  // should always be true here
            {
                m_gw->cleanUp();
                m_postInitPreCleanup = false;
            }
            setGameState(init);
			break;
		case gameover:
			{
				ostringstream oss;
				oss << (m_winner == GWSTATUS_PEACH_WON ? "PEACH" : "YOSHI") << " WON!"
					<< " STARS: " << m_gw->getWinnerStars() << " COINS: " << m_gw->getWinnerCoins();
				m_mainMessage = oss.str();
			}
			m_secondMessage = "Press ESC to quit...";
			setGameState(prompt);
			m_nextStateAfterPrompt = quit;
			break;
		case prompt: // @sbui review
			drawPrompt(m_mainMessage, m_secondMessage);
			{
				int key;
                if (getKeyIfAny(key)  &&  key >= '1'  &&  key <= '9')
                {
                    m_gw->setBoardNumber(key - '0');
                    setGameState(m_nextStateAfterPrompt);
                }
            }
			break;
		case init:
			{
				int status = m_gw->init();
                m_postInitPreCleanup = true;
				SoundFX().abortClip();
                switch (status)
                {
                  case GWSTATUS_CONTINUE_GAME:
					setGameState(makemove);
					break;
                  case GWSTATUS_PEACH_WON:  // shouldn't happen
                  case GWSTATUS_YOSHI_WON:  // shouldn't happen
					m_winner = status;
					setGameState(gameover);
					break;
                  case GWSTATUS_BOARD_ERROR:
                    m_mainMessage = "Error in board data file!";
                    m_secondMessage = "Press ESC to quit...";
                    setGameState(prompt);
                    m_nextStateAfterPrompt = quit;
					break;
				  default:
                    m_mainMessage = "init returned a wrong status!";
                    m_secondMessage = "Press ESC to quit...";
                    setGameState(prompt);
                    m_nextStateAfterPrompt = quit;
					break;
                }
            }
			break;
		case quit:
            if (m_postInitPreCleanup)  // might be false if aborted game
            {
                m_gw->cleanUp();
                m_postInitPreCleanup = false;
            }
            SoundFX().abortClip();
			glutLeaveMainLoop();
			break;
	}
}

void GameController::displayGamePlay()
{
    glEnable(GL_DEPTH_TEST); // must be done each time before displaying graphics or gets disabled for some reason
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

	for (int i = GraphObject::NUM_DEPTHS - 1; i >= 0; --i)
	{
		std::set<GraphObject*> &graphObjects = GraphObject::getGraphObjects(i);

		for (auto it = graphObjects.begin(); it != graphObjects.end(); it++)
		{
			GraphObject* cur = *it;
			if (cur->isVisible())
			{
				cur->animate();

				double x, y, gx, gy, gz;
				cur->getAnimationLocation(x, y);
				convertToGlutCoords(x, y, gx, gy, gz);

				int angle = cur->getDirection();
				int imageID = cur->getID();

				m_spriteManager.plotSprite(imageID, cur->getAnimationNumber() % m_spriteManager.getNumFrames(imageID), gx, gy, gz, angle, cur->getSize());
			}
		}
	}

	drawScoreAndLives(m_gameStatText);

	glutSwapBuffers();
}

void GameController::reportLeakedGraphObjects() const
{
    int totalLeaked = 0;
    for (int i = 0; i < GraphObject::NUM_DEPTHS; i++)
    {
        auto& graphObjects = GraphObject::getGraphObjects(i);
        if (graphObjects.empty())
            continue;
        cerr << "***** " << graphObjects.size() << " leaked objects at graphical depth " << i << ":" << endl;
         
        for (GraphObject* go : graphObjects)
            cerr << "At (" << go->getX() << "," << go->getY() << "): "
			     <<  m_imageNameMap.at(go->m_imageID) << endl;
        totalLeaked += graphObjects.size();
    }
    if (totalLeaked > 0)
        cout << "***** Total leaked objects: " << totalLeaked << endl;
}

void GameController::reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45.0, double(WINDOW_WIDTH) / WINDOW_HEIGHT, PERSPECTIVE_NEAR_PLANE, PERSPECTIVE_FAR_PLANE);
    glMatrixMode (GL_MODELVIEW);
}

static void convertToGlutCoords(double x, double y, double& gx, double& gy, double& gz)
{
	x /= VIEW_WIDTH;
	y /= VIEW_HEIGHT;
	gx = 2 * VISIBLE_MIN_X + .3 + x * 2 * (VISIBLE_MAX_X - VISIBLE_MIN_X);
	gy = 2 * VISIBLE_MIN_Y +	  y * 2 * (VISIBLE_MAX_Y - VISIBLE_MIN_Y);
	gz = .6 * VISIBLE_MIN_Z;
}

static void doOutputStroke(double x, double y, double z, double size, const char* str, bool centered)
{
	if (centered)
	{
		double len = glutStrokeLength(GLUT_STROKE_ROMAN, reinterpret_cast<const unsigned char*>(str)) / FONT_SCALEDOWN;
		x = -len / 2;
		size = 1;
	}
	GLfloat scaledSize = static_cast<GLfloat>(size / FONT_SCALEDOWN);
	glPushMatrix();
	glLineWidth(1);
	glLoadIdentity();
	glTranslatef(static_cast<GLfloat>(x), static_cast<GLfloat>(y), static_cast<GLfloat>(z));
	glScalef(scaledSize, scaledSize, scaledSize);
	for ( ; *str != '\0'; str++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *str);
	glPopMatrix();
}

//static void outputStroke(double x, double y, double z, double size, const char* str)
//{
//	doOutputStroke(x, y, z, size, str, false);
//}

static void outputStrokeCentered(double y, double z, const char* str)
{
	doOutputStroke(0, y, z, 1, str, true);
}

static void drawPrompt(string mainMessage, string secondMessage)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	glLoadIdentity ();
	outputStrokeCentered(1, -5, mainMessage.c_str());
	outputStrokeCentered(-1, -5, secondMessage.c_str());
	glutSwapBuffers();
}

static void drawScoreAndLives(string gameStatText)
{
	static int RATE = 1;
	static GLfloat rgb[3] =
		{ static_cast<GLfloat>(.6), static_cast<GLfloat>(.6), static_cast<GLfloat>(.6) };
	for (int k = 0; k < 3; k++)
	{
		double strength = rgb[k] + randInt(-RATE, RATE) / 100.0;
		if (strength < .6)
			strength = .6;
		else if (strength > 1.0)
			strength = 1.0;
		rgb[k] = static_cast<GLfloat>(strength);
	}
	glColor3f(rgb[0], rgb[1], rgb[2]);
	outputStrokeCentered(SCORE_Y, SCORE_Z, gameStatText.c_str()); // GAME DISPLAY LOCATION
}

#if defined(__APPLE__)
#pragma GCC diagnostic pop
#endif
