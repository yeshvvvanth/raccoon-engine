#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include"../data_types/shader.h"
#include"../maths/vec3.h"
#include"../maths/vec2.h"
#include"../maths/mat4.h"

#include<fstream>
#include<stdio.h>
#include<string>
#include<string.h>
#include<vector>

struct Node;
struct Character {
    float u,v;
    unsigned int  width,height;       // Size of glyph
    vec2       Bearing;    // Offset from baseline to left/top of glyph
    long     Advance;    // Offset to advance to next glyph
    Node* node;
    
};

struct Font{
  std::string name;
  unsigned short fontSize;
  unsigned char *buffer;
  int bsize;
  static GLuint uniFnt;
  static std::vector<Font*> fonts;
  static std::vector<Font*> efonts;
  static FT_Library ft;
  static Shader* shader;
  static void cleanUp();
  static void init();
  static void start();
  static Font* findFont(std::string name);
  GLfloat lheight;
  GLuint texture;
  FT_Face face;  
  Font(std::string fontName,std::ifstream &ifs,short size,bool eda);
  ~Font();
  std::vector<Character> Characters;
  int twidth,theight;
  unsigned short num_glyphs,st_char,end_char;
  void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
};

struct Label{
 static GLuint uniTint;
 static float dfsize;
 static Label* label;
 std::string text;
 std::vector<short> xoffsets;
 bool multiline;float width,height,lheight;
 vec3 color;
 GLfloat x,y,scale,lwidth;
 GLuint VAO, VBO;
 void tick();
 Font* font;
 void setPos(float x,float y);
 void setText(std::string text);
 void setScale(float tscale=dfsize);
 Label(std::string ltex, GLfloat x, GLfloat y,vec3 tcolor=vec3(1.0),GLfloat scale=dfsize,Font* font=0);
 ~Label();   
};

