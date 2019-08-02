#define EXP_ERROR     0
#define EXP_READY     1
#define EXP_COMPLETE  2

#define EXP_MAX_EXPERIENCE  50
#define EXP_MAX_POINTS      1000

int exp_init(char* author, char* game);
int exp_add(int id, int value, int secret=0);
int exp_add_desc(int id, char* name, char* desc, char* lang=NULL);
int exp_add_img(int id, char* img);
int exp_add_icon(char* img);
int exp_set_lang(char* lang);
void exp_win(int id);
char* exp_get_lang();
char* exp_user();
int exp_get_value(int id);
int exp_get_won(int id);
char* exp_get_name(int id);
char* exp_get_desc(int id);
char* exp_data_path();
void exp_set_callback(void (*callback)(int,int,char*,char*));
