#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "../inc/backexp_bmp.h"
#include "../inc/backexp2_bmp.h"
#include "../inc/font.h"

#define TRUE  1
#define FALSE 0

struct exp_message
{
  int id;
  int value;
  std::string title;
  std::string desc1;
  std::string desc2;
  Uint32 t[6];
};

std::vector<struct exp_message> mexp_list;
int mexp_running=FALSE;
int mexp_show=TRUE;

//graficos
SDL_Surface* mexp_screen;
SDL_Surface* mexp_backexp;
SDL_Surface* mexp_backexp2;
TTF_Font *mexp_font;


/* ************************ */
/* FUNCIONES DE USO INTERNO */
/* ************************ */

void im_split_string(char* s, std::string &s1,std::string &s2)
{
  if(s)
  {
    std::string str(s);
    if(str.size()>=49)
    {
      for(unsigned int f=48; f>0; f--)
      {
        if(str.substr(f,1)==" ")
        {
          s1=str.substr(0,f);
          s2=str.substr(f+1);
          break;
        }
      }
    }
    else
      s1=s;
  }
}

void im_draw_text(SDL_Surface* dst, char* string, int x, int y, int fR, int fG, int fB, TTF_Font *f=mexp_font)
{
  if(string)
  {
    /*std::string str1;
    std::string str2;
    im_split_string(string,str1,str2);*/
    SDL_Color foregroundColor={fR,fG,fB};

    SDL_Surface *textSurface=TTF_RenderText_Blended(f,string,foregroundColor);
    SDL_Rect textLocation={x,y,0,0};
    SDL_BlitSurface(textSurface,NULL,dst,&textLocation);
    SDL_FreeSurface(textSurface);

/*    textSurface=TTF_RenderText_Blended(f,str2.c_str(),foregroundColor);
    SDL_Rect textLocation2={x,y+10,0,0};
    SDL_BlitSurface(textSurface,NULL,dst,&textLocation2);
    SDL_FreeSurface(textSurface);*/
  }
}

/* ************************ */
/* FUNCIONES A EXPORTAR     */
/* ************************ */

// muestra el osd o no
void exp_screen(SDL_Surface* screen)
{
  mexp_screen=screen;

  //carga los gráficos
  SDL_Rect rect;
  SDL_Surface* tmpsurface;
  SDL_RWops *rw = SDL_RWFromMem(backexp_bmp, BACKEXP_BMP_LEN);
  tmpsurface=SDL_LoadBMP_RW(rw,1);
  rect.x=0;
  rect.y=0;
  rect.w=tmpsurface->w;
  rect.h=tmpsurface->h;
  mexp_backexp=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
  SDL_BlitSurface(tmpsurface,&rect,mexp_backexp,NULL);
  SDL_SetColorKey(mexp_backexp,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
  SDL_FreeSurface(tmpsurface);

  rw = SDL_RWFromMem(backexp2_bmp, BACKEXP2_BMP_LEN);
  tmpsurface=SDL_LoadBMP_RW(rw,1);
  rect.x=0;
  rect.y=0;
  rect.w=tmpsurface->w;
  rect.h=tmpsurface->h;
  mexp_backexp2=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, rect.w, rect.h, 16, 0,0,0,0);
  SDL_BlitSurface(tmpsurface,&rect,mexp_backexp2,NULL);
  SDL_SetColorKey(mexp_backexp2,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
  SDL_FreeSurface(tmpsurface);

  // y la fuente
  TTF_Init();
  mexp_font=TTF_OpenFontRW(SDL_RWFromMem(font_pfcb,FONT_PFCB_LEN),1, 8);
}

void exp_osd(int value)
{
  mexp_show=value;
}

void exp_callback(int id, int value, char* title, char* desc)
{
  exp_message m;
  m.id=id;
  m.value=value;
  m.t[0]=SDL_GetTicks();
  m.t[1]=150;
  m.t[2]=m.t[1]+150;

  if(title)
  {
    m.title=std::string(title);
    if(m.title.size()>0)
      m.t[3]=m.t[2]+2150;
    else
      m.t[3]=m.t[2];
  }
  else
    m.t[3]=m.t[2];
  if(desc)
  {
    im_split_string(desc,m.desc1,m.desc2);
    if(m.desc1.size()>0)
      m.t[4]=m.t[3]+2150;
    else
      m.t[4]=m.t[3];
  }
  else
    m.t[4]=m.t[3];
  m.t[5]=m.t[4]+200;

  mexp_list.push_back(m);
}

void exp_update()
{
  if(!mexp_running)
  {
    mexp_running=TRUE;
    if(mexp_list.size()>0)
      mexp_list[0].t[0]=SDL_GetTicks();
  }

  SDL_Rect dest;
  // si existen notificaciones, las muestra
  if(mexp_list.size()>0 && mexp_show)
  {
    Uint32 diff=SDL_GetTicks()-mexp_list[0].t[0];
    if(diff<mexp_list[0].t[1])
    {
      // sube la notificacion
      dest.x=148;
      dest.y=240-(diff*30/mexp_list[0].t[1]);
      if(mexp_backexp && mexp_screen)
        SDL_BlitSurface(mexp_backexp,NULL,mexp_screen,&dest);
    }
    else if(diff>=mexp_list[0].t[1] && diff<mexp_list[0].t[2])
    {
      // se abre la notificacion
      dest.x=148-((diff-mexp_list[0].t[1])*124/(mexp_list[0].t[2]-mexp_list[0].t[1]));
      dest.y=210;
      // rectangulo de fondo
      SDL_Rect rect;
      rect.x=dest.x+22;
      rect.y=210;
      rect.h=24;
      rect.w=(160-dest.x)*2-4;
      Uint32 c=SDL_MapRGB(mexp_screen->format, 101, 143, 101);
      SDL_FillRect(mexp_screen, &rect, c);
      // iconos laterales
      if(mexp_backexp && mexp_screen)
        SDL_BlitSurface(mexp_backexp,NULL,mexp_screen,&dest);
      dest.x=320-dest.x-2;
      if(mexp_backexp2 && mexp_screen)
        SDL_BlitSurface(mexp_backexp2,NULL,mexp_screen,&dest);
    }
    else if(diff>=mexp_list[0].t[2] && diff<mexp_list[0].t[3])
    {
      // se muestra el titulo
      // rectangulo de fondo
      SDL_Rect rect;
      rect.x=46;
      rect.y=210;
      rect.h=24;
      rect.w=248;
      Uint32 c=SDL_MapRGB(mexp_screen->format, 101, 143, 101);
      SDL_FillRect(mexp_screen, &rect, c);
      // iconos laterales
      dest.x=24;
      dest.y=210;
      if(mexp_backexp && mexp_screen)
        SDL_BlitSurface(mexp_backexp,NULL,mexp_screen,&dest);
      dest.x=294;
      if(mexp_backexp2 && mexp_screen)
        SDL_BlitSurface(mexp_backexp2,NULL,mexp_screen,&dest);
      im_draw_text(mexp_screen,(char*)mexp_list[0].title.c_str(),50,208,255,255,0);
    }
    else if(diff>=mexp_list[0].t[3] && diff<mexp_list[0].t[4])
    {
      // se muestra la descripcion
      // rectangulo de fondo
      SDL_Rect rect;
      rect.x=46;
      rect.y=210;
      rect.h=24;
      rect.w=248;
      Uint32 c=SDL_MapRGB(mexp_screen->format, 101, 143, 101);
      SDL_FillRect(mexp_screen, &rect, c);
      // iconos laterales
      dest.x=24;
      dest.y=210;
      if(mexp_backexp && mexp_screen)
        SDL_BlitSurface(mexp_backexp,NULL,mexp_screen,&dest);
      dest.x=294;
      if(mexp_backexp2 && mexp_screen)
        SDL_BlitSurface(mexp_backexp2,NULL,mexp_screen,&dest);
      im_draw_text(mexp_screen,(char*)mexp_list[0].desc1.c_str(),50,208,255,255,0);
      im_draw_text(mexp_screen,(char*)mexp_list[0].desc2.c_str(),50,218,255,255,0);
    }
    else if(diff>=mexp_list[0].t[4] && diff<mexp_list[0].t[5])
    {
      // se esconde la notificacion
      dest.x=24;
      dest.y=210+((diff-mexp_list[0].t[4])*30/(mexp_list[0].t[5]-mexp_list[0].t[4]));
      // rectangulo de fondo
      SDL_Rect rect;
      rect.x=46;
      rect.y=dest.y;
      rect.h=24;
      rect.w=248;
      Uint32 c=SDL_MapRGB(mexp_screen->format, 101, 143, 101);
      SDL_FillRect(mexp_screen, &rect, c);
      // iconos laterales
      if(mexp_backexp && mexp_screen)
        SDL_BlitSurface(mexp_backexp,NULL,mexp_screen,&dest);
      dest.x=294;
      if(mexp_backexp2 && mexp_screen)
        SDL_BlitSurface(mexp_backexp2,NULL,mexp_screen,&dest);
      if(mexp_list[0].desc1.size()>0)
      {
        im_draw_text(mexp_screen,(char*)mexp_list[0].desc1.c_str(),50,dest.y,255,255,0);
        im_draw_text(mexp_screen,(char*)mexp_list[0].desc2.c_str(),50,dest.y+10,255,255,0);
      }
      else
        im_draw_text(mexp_screen,(char*)mexp_list[0].title.c_str(),50,dest.y,255,255,0);
    }
    else
    {
      // se acaba la notificacion
      mexp_list.erase(mexp_list.begin());
      if(mexp_list.size()>0)
        mexp_list[0].t[0]=SDL_GetTicks();
    }
  }
  else if(!mexp_show)
  {
    mexp_list.erase(mexp_list.begin());
  }
}

void exp_end()
{
  mexp_screen=NULL;
  if(mexp_backexp)
    SDL_FreeSurface(mexp_backexp);
  if(mexp_backexp2)
    SDL_FreeSurface(mexp_backexp2);
}
