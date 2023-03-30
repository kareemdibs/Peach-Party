#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

#include <random>
#include <utility>

// image IDs for the game objects

const int IID_PEACH = 0;
const int IID_YOSHI = 1;
const int IID_BLUE_COIN_SQUARE = 2;
const int IID_RED_COIN_SQUARE = 3;
const int IID_DIR_SQUARE = 4;
const int IID_EVENT_SQUARE = 5;
const int IID_BANK_SQUARE = 6;
const int IID_STAR_SQUARE = 7;
const int IID_DROPPING_SQUARE = 8;
const int IID_BOWSER = 9;
const int IID_BOO = 10;
const int IID_VORTEX = 11;

// sounds

const int SOUND_PLAYER_FIRE = 0;
const int SOUND_GIVE_COIN = 1;
const int SOUND_TAKE_COIN = 2;
const int SOUND_GIVE_STAR = 3;
const int SOUND_WITHDRAW_BANK = 4;
const int SOUND_DEPOSIT_BANK = 5;
const int SOUND_PLAYER_TELEPORT = 6;
const int SOUND_GIVE_VORTEX = 7;
const int SOUND_DROPPING_SQUARE_ACTIVATE = 8;
const int SOUND_BOWSER_ACTIVATE = 9;
const int SOUND_DROPPING_SQUARE_CREATED = 10;
const int SOUND_BOO_ACTIVATE = 11;
const int SOUND_HIT_BY_VORTEX = 12;
const int SOUND_THEME = 13;
const int SOUND_GAME_FINISHED = 14;
const int SOUND_NONE = -1;

// keys the user can hit

const int KEY_PRESS_LEFT   = 1000;
const int KEY_PRESS_RIGHT  = 1001;
const int KEY_PRESS_UP     = 1002;
const int KEY_PRESS_DOWN   = 1003;
const int KEY_PRESS_SPACE  = ' ';
const int KEY_PRESS_ESCAPE = '\x1b';
const int KEY_PRESS_TAB    = '\t';
const int KEY_PRESS_ENTER  = '\r';

// actions directed by keys

const int ACTION_NONE  = 0;
const int ACTION_LEFT  = 1;
const int ACTION_RIGHT = 2;
const int ACTION_UP    = 3;
const int ACTION_DOWN  = 4;
const int ACTION_ROLL  = 5;
const int ACTION_FIRE  = 6;

// board and sprite dimensions

const int VIEW_WIDTH = 256;
const int VIEW_HEIGHT = 256;

const int SPRITE_WIDTH = 16;
const int SPRITE_HEIGHT = 16;

const int BOARD_WIDTH = VIEW_WIDTH / SPRITE_WIDTH;
const int BOARD_HEIGHT = VIEW_HEIGHT / SPRITE_HEIGHT;

const double SPRITE_WIDTH_GL = .6; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .6; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming

// status of each tick (did the player die?)

const int GWSTATUS_CONTINUE_GAME   = 0;
const int GWSTATUS_PEACH_WON       = 1;
const int GWSTATUS_YOSHI_WON       = 2;
const int GWSTATUS_BOARD_ERROR     = 3;
const int GWSTATUS_NOT_IMPLEMENTED = 4;

// test parameter constants

const int NUM_TEST_PARAMS = 1;

// Return a uniformly distributed random int from min to max, inclusive

inline
int randInt(int min, int max)
{
	if (max < min)
		std::swap(max, min);
	static std::random_device rd;
	static std::default_random_engine generator(rd());
	std::uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

#endif // GAMECONSTANTS_H_
