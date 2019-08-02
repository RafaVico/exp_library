#include <string>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#define EXP_ERROR     0
#define EXP_READY     1
#define EXP_COMPLETE  2

#define EXP_MAX_EXPERIENCE  50
#define EXP_MAX_POINTS      1000

int exp_init(char* author, char* game, SDL_Surface* screen=NULL);
int exp_add(int id, int value, int secret=FALSE);
int exp_add_desc(int id, char* name, char* desc, char* lang=NULL);
int exp_add_img(int id, SDL_Surface* img);
int exp_add_icon(SDL_Surface* img);
int exp_set_lang(char* lang);
void exp_win(int id);
char* exp_get_lang();
char* exp_user();
int exp_get_value(int id);
int exp_get_won(int id);
char* exp_get_name(int id);
char* exp_get_desc(int id);
char* exp_data_path();
void exp_osd(int value);
void exp_update();
void exp_end();
