#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#ifdef PLATFORM_WIN
#include <shlobj.h>
#endif

#define TRUE  1
#define FALSE 0

#define EXP_ERROR     0
#define EXP_READY     1
#define EXP_COMPLETE  2

#define EXP_MAX_EXPERIENCE  50
#define EXP_MAX_POINTS      1000

struct exp
{
  int id;
  int value;
  int secret;
  int won;
};

// esto va separado de exp, porque puede haber varios idiomas
struct exp_desc
{
  int id;
  char name[21];
  char desc[101];
};

// variables
#ifdef PLATFORM_GP2X
std::string mexp_root="/mnt/sd/exp/";
#endif
#ifdef PLATFORM_WIN
std::string mexp_root="c:/exp/";
#endif
std::string mexp_root_exp;
std::string mexp_root_game;
std::string mexp_root_saves;
std::string mexp_author;
std::string mexp_game;
std::string mexp_author_dir;
std::string mexp_game_dir;
std::string mexp_lang;
std::string mexp_lang_desc;
std::string mexp_first_lang;
std::string mexp_expname;
int mexp_OK=FALSE;
int mexp_won=0;
int mexp_won_max=0;
int mexp_total=0;
int mexp_total_max=0;
int mexp_played=0;

//listas
std::vector<struct exp> mexp_list_exp;
std::vector<struct exp_desc> mexp_list_desc; // logros en el idioma actual

// callback
void (*im_callback)(int,int,char*,char*);

#ifdef PLATFORM_WIN
void get_root_path()
{
  LPITEMIDLIST pidl;
  HRESULT hr = SHGetSpecialFolderLocation(NULL, 35, &pidl);
  char szPath[_MAX_PATH];
  SHGetPathFromIDList(pidl, szPath);
  LPMALLOC pMalloc;
  hr = SHGetMalloc(&pMalloc);
  pMalloc->Free(pidl);
  pMalloc->Release();
  mexp_root=std::string(szPath)+"/exp/";
}
#endif

// declaracion de funciones
int exp_set_lang(char* lang);

const char* debug_exp()
{
  return mexp_lang.c_str();
}

std::string im_remove_spaces(std::string str)
{
  std::string s;
  for(unsigned int f=0; f<str.size(); f++)
  {
    if(str.substr(f,1)>" ")
      s=s+str.substr(f,1);
  }

  return s;
}

std::string im_uptolow(std::string str)
{
  for (unsigned int i=0;i<strlen(str.c_str());i++)
    if (str[i] >= 0x41 && str[i] <= 0x5A)
      str[i] = str[i] + 0x20;
  return str;
}

int im_search_exp(int id)
{
  for(unsigned int f=0; f<mexp_list_exp.size(); f++)
    if(mexp_list_exp[f].id==id)
      return 1;

  return 0;
}

int im_search_desc(int id)
{
  for(unsigned int f=0; f<mexp_list_desc.size(); f++)
    if(mexp_list_desc[f].id==id)
      return 1;

  return 0;
}

void im_crypt_desc()
{
  // encriptamos el texto con un XOR para que no se lea con un editor de textos
  for(unsigned int f=0; f<mexp_list_desc.size(); f++)
  {
    for(unsigned int i=0; i<21; i++)
      mexp_list_desc[f].name[i]^=255;
    for(unsigned int i=0; i<101; i++)
      mexp_list_desc[f].desc[i]^=255;
  }
}

void im_file__resolution(char* filename, int &width, int &height)
{
  FILE* exp_file;
  exp_file=fopen(filename,"rb");
  if(exp_file)
  {
    fseek(exp_file,18,SEEK_SET);
    fread(&width,sizeof(int),1,exp_file);
    fread(&height,sizeof(int),1,exp_file);
    fclose(exp_file);
  }
}

int im_load_expname()
{
  std::ifstream cfgfile;
  std::string exp_file=mexp_root_exp+std::string("exp.cfg");
  cfgfile.open(exp_file.c_str());

  if(cfgfile)
  {
    char buffer[1024];
    std::string bfline;
    std::string parm;
    std::string value;
    while(cfgfile.getline(buffer,1024))
    {
      bfline=buffer;
      unsigned int i=bfline.find("=",1);
      parm=bfline.substr(0,i);
      if(i<=parm.size())
        value=bfline.substr(i);
      while(value.substr(value.size()-1,1)<=" ")
        value=value.substr(0,value.size()-1);

      if(parm==std::string("lang") && value.size()>0)
        exp_set_lang((char*)value.substr(1).c_str());
      if(parm==std::string("name") && value.size()>0)
        mexp_expname=value.substr(1);
    }
    cfgfile.close();
    return 1;
  }
  return 0;
}

void im_save_summary()
{
  std::string fname=mexp_root_game+"exp.summary";
  FILE* exp_file;
  exp_file=fopen(fname.c_str(),"wb");
  if(exp_file)
  {
    fwrite(&mexp_won,sizeof(int),1,exp_file);
    fwrite(&mexp_won_max,sizeof(int),1,exp_file);
    fwrite(&mexp_total,sizeof(int),1,exp_file);
    fwrite(&mexp_total_max,sizeof(int),1,exp_file);
    fwrite(&mexp_played,sizeof(int),1,exp_file);
    fclose(exp_file);
  }
  fname=mexp_root_exp+"update";
  exp_file=fopen(fname.c_str(),"wb");
  fclose(exp_file);
}

int im_load_summary()
{
  std::string fname=mexp_root_game+"exp.summary";
  FILE* exp_file;
  exp_file=fopen(fname.c_str(),"rb");
  if(exp_file)
  {
    fread(&mexp_won,sizeof(int),1,exp_file);
    fread(&mexp_won_max,sizeof(int),1,exp_file);
    fread(&mexp_total,sizeof(int),1,exp_file);
    fread(&mexp_total_max,sizeof(int),1,exp_file);
    fread(&mexp_played,sizeof(int),1,exp_file);
    fclose(exp_file);
    return 1;
  }
  return 0;
}

int im_load_first_lang()
{
  std::ifstream efile;
  std::string exp_file=mexp_root_game+std::string("exp.lang");
  efile.open(exp_file.c_str());

  if(efile)
  {
    char buffer[1024];
    efile.getline(buffer,1024);
    mexp_first_lang=std::string(buffer);
    efile.close();
    return 1;
  }
  return 0;
}

void im_save_first_lang()
{
  std::string fname=mexp_root_game+std::string("exp.lang");
  FILE* expfile;
  expfile=fopen(fname.c_str(),"wb");
  if(expfile)
  {
    char expopt[100];
    sprintf(expopt,"%s",mexp_first_lang.c_str());
    std::string e=expopt;
    fwrite(e.c_str(), e.size(), 1, expfile);
    fclose(expfile);
  }
}

void im_save_title()
{
  std::string fname=mexp_root_game+std::string("exp.title");
  struct stat st;
  if(stat(fname.c_str(),&st) != 0)
  {
    FILE* expfile;
    expfile=fopen(fname.c_str(),"wb");
    if(expfile)
    {
      char expopt[100];
      sprintf(expopt,"title=%s\15\12",mexp_game.c_str());
      std::string e=std::string(expopt);
      fwrite(e.c_str(), e.size(), 1, expfile);
      sprintf(expopt,"author=%s\15\12",mexp_author.c_str());
      e=std::string(expopt);
      fwrite(e.c_str(), e.size(), 1, expfile);
      fclose(expfile);
    }
  }
}

int im_load_exp()
{
  std::string fname=mexp_root_game+"exp.dat";
  FILE* exp_file;
  exp_file=fopen(fname.c_str(),"rb");
  if(exp_file)
  {
    exp e;
    while(fread(&e,sizeof(exp),1,exp_file)==1)
      mexp_list_exp.push_back(e);
    fclose(exp_file);
    return 1;
  }
  return 0;
}

void im_save_exp()
{
  std::string fname=mexp_root_game+"exp.dat";
  FILE* exp_file;
  exp_file=fopen(fname.c_str(),"wb");
  if(exp_file)
  {
    for(unsigned int f=0; f<mexp_list_exp.size(); f++)
      fwrite(&mexp_list_exp[f], sizeof(exp), 1, exp_file);
    fclose(exp_file);
  }
}

void im_save_exp_desc(std::string lang)
{
  std::string fname;
  if(lang.size()==0)
    fname=mexp_root_game+"exp.desc.default";
  else
    fname=mexp_root_game+"exp.desc."+lang;

  im_crypt_desc();

  FILE* exp_file;
  exp_file=fopen(fname.c_str(),"wb");
  if(exp_file)
  {
    for(unsigned int f=0; f<mexp_list_desc.size(); f++)
      fwrite(&mexp_list_desc[f], sizeof(exp_desc), 1, exp_file);
    fclose(exp_file);
  }

  im_crypt_desc();
}

int im_load_active_exp()
{
  std::ifstream act_exp;
  std::string f=mexp_root+"activeexp";
  act_exp.open(f.c_str());
  if(act_exp)
  {
    char buffer[1024];
    act_exp.getline(buffer,1024);
    //std::string exp=std::string(buffer);
    mexp_root_exp=mexp_root+std::string(buffer)+"/";
  }
  act_exp.close();

  return 1;
}

int im_load_desc()
{
  mexp_list_desc.clear();
  std::string fname;
  if(mexp_lang_desc.size()==0)
    fname=mexp_root_game+"exp.desc.default";
  else
    fname=mexp_root_game+"exp.desc."+mexp_lang_desc;

  FILE* exp_file;
  exp_file=fopen(fname.c_str(),"rb");
  if(exp_file)
  {
    exp_desc e;
    while(fread(&e, sizeof(exp_desc), 1, exp_file)==1)
      mexp_list_desc.push_back(e);
    im_crypt_desc();

    fclose(exp_file);
    return 1;
  }

  return 0;
}

/* ************************************************************** */
/* FUNCIONES PUBLICAS                                             */
/* ************************************************************** */

// inicia la librería, carga el exp activo y recupera los logros
// o crea la carpeta del juego si no existe
// Máximo 50 logros y 1000 puntos.
int exp_init(char* author, char* game)
{
  struct stat st;

  im_callback=NULL;
  if(!author || !game)
    return EXP_ERROR;

#ifdef PLATFORM_WIN
  get_root_path();
#endif

  // si no hay nombre de autor, da error
  mexp_author=std::string(author);
  mexp_game=std::string(game);
  mexp_author_dir=im_uptolow(im_remove_spaces(std::string(author)));
  if(mexp_author_dir.size()==0)
    return EXP_ERROR;
  // si no hay nombre de juego, da error
  mexp_game_dir=im_uptolow(im_remove_spaces(std::string(game)));
  if(mexp_game_dir.size()==0)
    return EXP_ERROR;

  // carga la carpeta del EXP activo
  if(!im_load_active_exp())
    return EXP_ERROR;

  // recupera el nombre e idioma del exp
  if(!im_load_expname())
    return EXP_ERROR;

  // si no existe la carpeta de juego, la crea
  mexp_root_game=mexp_root_exp+mexp_author_dir+"_"+mexp_game_dir+"/";
  mexp_root_saves=mexp_root_game+"/saves/";
  if(stat(mexp_root_game.c_str(),&st) != 0)
  {
#ifdef PLATFORM_GP2X
    mkdir(mexp_root_game.c_str(),0);
    mkdir(mexp_root_saves.c_str(),0);
#endif
#ifdef PLATFORM_WIN
    mkdir(mexp_root_game.c_str());
    mkdir(mexp_root_saves.c_str());
#endif
  }

  // guarda el titulo y autor
  im_save_title();
  // recupera el primer idioma
  im_load_first_lang();
  // recupera el resumen de los logros existentes
  im_load_summary();
  mexp_played++;
  im_save_summary();

  mexp_OK=TRUE; // hasta aquí, ya tiene suficiente para funcionar

  if(im_load_exp())
  {
    if(!im_load_desc())
    {
      exp_set_lang((char*)mexp_first_lang.c_str());
      im_load_desc();
    }
    return EXP_COMPLETE;
  }

  return EXP_READY;
}

int exp_add(int id, int value, int secret=FALSE)
{
  // si no tiene id, no mete el logro
  if(id<=0 || id>EXP_MAX_EXPERIENCE)
    return 0;
  // si ya existe, no mete el logro
  if(im_search_exp(id))
    return 0;
  // si el valor supera los 1000 puntos totales, no mete el logro
  if(mexp_total+value>EXP_MAX_POINTS)
    return 0;

  exp e;
  e.id=id;
  e.value=value;
  e.secret=secret;
  e.won=0;

  mexp_list_exp.push_back(e);
  mexp_total_max+=value;
  mexp_won_max++;

  im_save_exp();
  im_save_summary();

  return 1;
}

int exp_add_desc(int id, char* name, char* desc, char* lang=NULL)
{
  // si no tiene id, no mete el logro
  if(id<=0 || id>EXP_MAX_EXPERIENCE)
    return 0;

  std::string lng;
  if(lang)
    lng=std::string(lang);
  // si es otro idioma, carga el fichero correspondiente
  if(lng!=mexp_lang_desc)
  {
    mexp_lang_desc=lng;
    im_load_desc();
  }

  // si ya existe, no mete el logro
  if(im_search_desc(id))
    return 0;

  // si es el primer idioma metido, lo conserva para mas adelante
  if(mexp_first_lang.size()==0)
  {
    if(lang)
      mexp_first_lang=std::string(lang);
    else
      mexp_first_lang="default";
    im_save_first_lang();
  }

  std::string str;
  exp_desc e;
  e.id=id;
  for(unsigned int f=0; f<21; f++)
    e.name[f]=0;
  if(name)
  {
    str=std::string(name);
    if(str.size()>20)
      str=str.substr(0,20);
    sprintf(e.name, "%s", str.c_str());
  }
  str="";
  for(unsigned int f=0; f<101; f++)
    e.desc[f]=0;
  if(desc)
  {
    str=std::string(desc);
    if(str.size()>100)
      str=str.substr(0,100);
    sprintf(e.desc, "%s", str.c_str());
  }
  mexp_list_desc.push_back(e);

  if(lang)
    lng=std::string(lang);
  else
    lng="";
  im_save_exp_desc(lng);

  return 1;
}

int exp_add_img(int id, char* img)
{
  // si no tiene id, no mete la imagen
  if(id<=0 || id>EXP_MAX_EXPERIENCE)
    return 0;

  if(img)
  {
    // si no tiene el tamaño adecuado, no mete la imagen
    int w=0;
    int h=0;
    im_file__resolution(img,w,h);
    if(h!=24 && w!=24)
      return 0;

    char n[15];
    if(id<10)
      sprintf(n,"00%i",id);
    else if(id<100)
      sprintf(n,"0%i",id);
    else
      sprintf(n,"%i",id);
    std::string fname=mexp_root_game+std::string(n);

    // si es corretco, copia el archivo a la carpeta EXP
    std::ifstream infile(img, std::ios_base::binary);
    std::ofstream outfile(fname.c_str(), std::ios_base::binary);
    outfile << infile.rdbuf();
    return 1;
  }
  return 0;
}

int exp_add_icon(char* img)
{
  if(img)
  {
    // si no tiene el tamaño adecuado, no mete la imagen
    int w=0;
    int h=0;
    im_file__resolution(img,w,h);
    if(h!=24 && w!=24)
      return 0;

    std::string fname=mexp_root_game+"exp.icon";

    // si es corretco, copia el archivo a la carpeta EXP
    std::ifstream infile(img, std::ios_base::binary);
    std::ofstream outfile(fname.c_str(), std::ios_base::binary);
    outfile << infile.rdbuf();
  }

  return 1;
}

int exp_set_lang(char* lang)
{
  if(lang)
    mexp_lang=std::string(lang);
  else
    mexp_lang="default";

  mexp_lang_desc=mexp_lang;
  im_load_desc();

  return 1;
}

void exp_win(int id)
{
  if(!mexp_OK || id<=0 || id>EXP_MAX_EXPERIENCE)
    return;
  if(mexp_lang_desc!=mexp_lang)
  {
    mexp_lang_desc=mexp_lang;
    if(!im_load_desc())
      exp_set_lang((char*)mexp_first_lang.c_str());
  }
  for(unsigned int f=0; f<mexp_list_exp.size(); f++)
    if(mexp_list_exp[f].id==id)
    {
      if(mexp_list_exp[f].won==0)
      {
        mexp_list_exp[f].won=1;
        mexp_won++;
        mexp_total+=mexp_list_exp[f].value;
        im_save_exp();
        im_save_summary();

        if(im_callback)
        {
          for(unsigned int g=0; g<mexp_list_desc.size(); g++)
            if(mexp_list_desc[g].id==id)
            {
              im_callback(id,mexp_list_exp[f].value,mexp_list_desc[g].name,mexp_list_desc[g].desc);
              break;
            }
        }
      }
      break;
    }
}

char* exp_get_lang()
{
  return (char*)mexp_lang.c_str();
}

char* exp_user()
{
  return (char*)mexp_expname.c_str();
}

int exp_get_value(int id)
{
  if(id>0 || id<=EXP_MAX_EXPERIENCE)
    for(unsigned int f=0; f<mexp_list_exp.size(); f++)
      if(mexp_list_exp[f].id==id)
        return mexp_list_exp[f].value;
  return 0;
}

int exp_get_won(int id)
{
  if(id>0 || id<=EXP_MAX_EXPERIENCE)
    for(unsigned int f=0; f<mexp_list_exp.size(); f++)
      if(mexp_list_exp[f].id==id)
        return mexp_list_exp[f].won;
  return 0;
}

char* exp_get_name(int id)
{
  if(id>0 || id<=EXP_MAX_EXPERIENCE)
    for(unsigned int f=0; f<mexp_list_desc.size(); f++)
      if(mexp_list_desc[f].id==id)
        return mexp_list_desc[f].name;
  return NULL;
}

char* exp_get_desc(int id)
{
  if(id>0 || id<=EXP_MAX_EXPERIENCE)
    for(unsigned int f=0; f<mexp_list_desc.size(); f++)
      if(mexp_list_desc[f].id==id)
        return mexp_list_desc[f].desc;
  return NULL;
}

char* exp_data_path()
{
  if(mexp_root_saves.size()>0)
    return (char*)mexp_root_saves.c_str();
  else
    return NULL;
}

void exp_set_callback(void (*callback)(int,int,char*,char*))
{
  im_callback=callback;

  if(mexp_expname.size()>0)
  {
    std::string msg="Login... "+mexp_expname;
    im_callback(0,0,(char*)msg.c_str(), "");
  }
}
