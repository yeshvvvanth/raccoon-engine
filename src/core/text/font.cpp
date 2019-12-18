#include"font.h"
#include"rectbinpack.h"
#include"../UI/textfield.h"
Label* Label::label=0;
Shader * Font::shader=0;
GLuint Font::uniFnt=GL_INVALID_VALUE;
GLuint Label::uniTint=GL_INVALID_VALUE;
FT_Library         Font::ft=0;
std::vector<Font*> Font::fonts;
std::vector<Font*> Font::efonts;
void Font::start()
{
  if (FT_Init_FreeType(&ft))
    printf("ERROR::FREETYPE: Could not init FreeType Library\n");
}

void Font::init()
{
  Label::uniTint=glGetUniformLocation(shader->PID, "tint");
  uniFnt=glGetUniformLocation(shader->PID,"fnt");
  Label::label=new Label("sample text",0,0,vec3(1.0),24.0);
  TextField::ibeam=new Ibeam();
  Sprite::Init();
  Sprite::sp=new Sprite(0.0,0.0,0.0,0.0);
}

Font::Font(std::string fontName,std::ifstream &ifs, short int fontsize,bool eda):
name(fontName),fontSize(fontsize),lheight(0)
{
    buffer=0; bsize=0;
    ifs.read((char*)&bsize,4);
    buffer=new unsigned char[bsize];
    printf("size of font=%d \n",bsize);
    ifs.read((char*)buffer,bsize);
    
   if (FT_New_Memory_Face(ft,buffer,bsize,0,&face))
        printf("ERROR::FREETYPE: Failed to load font\n");
    
    
   FT_Set_Pixel_Sizes(face, 0,fontSize);  
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   st_char=0;
   end_char=st_char+128;
   num_glyphs=end_char-st_char;
  
   for (GLubyte c = st_char; c <end_char; c++)
   {
	// Load character glyph 
	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	{
	    printf("ERROR::FREETYTPE: Failed to load Glyph\n");
	    continue;
	}
	Character character = {
	    0,0,
	    face->glyph->bitmap.width,
	    face->glyph->bitmap.rows,
	    vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
	    face->glyph->advance.x,
	    0
	};
	//printf("char width=%d height =%d \n",character.width,character.height);
	Characters.push_back(character);
	if(character.Bearing.y>lheight){lheight=character.Bearing.y;}
    }
    unsigned char* fimage= Pack(this,&twidth,&theight);
    
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
#ifdef EMSCRIPTEN
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, twidth, theight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, fimage);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RED, GL_UNSIGNED_BYTE, fimage);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] fimage;
    
    printf("fimage width =%d , fimage height =%d \n\n\n\n",twidth,theight);

}


Font::~Font()
{
  FT_Done_Face(face);
  delete []buffer;
}
void Label::setScale(float tscale)
{
  scale=tscale;
  lheight=scale*font->lheight/font->fontSize;
  setText(text);
}

Label::Label(std::string ltex, GLfloat x, GLfloat y,vec3 tcolor,GLfloat scale,Font* tfont):
text(ltex),x(x),y(y),scale(scale),color(tcolor),lwidth(0)
{
  this->x=x;this->y=y;
  this->font=(tfont!=0)?tfont:Font::efonts[0];
  multiline=false;width=150;height=160;
  Font::shader->use();
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24 * 256, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2* sizeof(GLfloat)) );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  
    setScale(scale);
}
void Label::setPos(float tx, float ty)
{
   x=tx;y=ty;
   setText(text);
}
void Label::setText(std::string ftext)
{
   text=ftext;
   Font::shader->use();
   lwidth=0;
   xoffsets.clear();
   short tempx=0,tempy=0;
   xoffsets.push_back(tempx);
   float rscale=scale/font->fontSize;
   unsigned short len=text.length();
   float* vertices=new float[len*24];

   float verts[4*6];
   
   for(int i=0;i<text.length();i++){
     
	int k=(int)text[i]-font->st_char;
	
	Character& ch=font->Characters[k];
	
     if(multiline && (tempx+ch.Bearing.x * rscale)>width){tempx=0;tempy+=font->fontSize;}
    
	GLfloat xpos = x  + ch.Bearing.x * rscale       +tempx;
	GLfloat ypos = y - (ch.height - ch.Bearing.y) * rscale -tempy;

	GLfloat w = ch.width * rscale ;
	GLfloat h = ch.height * rscale;

	
	float chw=(float)(ch.width)/(float)font->twidth,chh=(float)(ch.height)/(float)font->theight;

	verts[0]=xpos;   verts[2]=ch.u;
	verts[1]=ypos+h; verts[3]=ch.v;
	
	verts[4]=xpos;   verts[6]=ch.u;
	verts[5]=ypos;   verts[7]=(ch.v+chh);
	
	verts[8]=xpos+w; verts[10]=(ch.u+chw);
	verts[9]=ypos;   verts[11]=(ch.v+chh);
	
	verts[12]=xpos+w; verts[14]=(ch.u+chw);
	verts[13]=ypos;   verts[15]=(ch.v+chh);
	
	verts[16]=xpos;   verts[18]=ch.u;
	verts[17]=ypos+h; verts[19]=ch.v;
	
	verts[20]=xpos+w; verts[22]=(ch.u+chw);
	verts[21]=ypos+h; verts[23]=(ch.v);
	
	tempx += (ch.Advance >> 6) * rscale ;
	lwidth+=(ch.Advance >> 6) * rscale ;
	
	xoffsets.push_back(tempx);
	memcpy(vertices+i*24,verts,24*sizeof(float));
   }
   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);

   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*len*24, vertices); 
   delete[] vertices;
}


void Label::tick()
{ 
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  Font::shader->use();
  glBindVertexArray(VAO);
  
  glBindTexture(GL_TEXTURE_2D,font->texture);
  mat4 temp;
  glUniformMatrix4fv(Font::shader->uniModel, 1, GL_FALSE,temp.elements);
  glUniform1f(Font::shader->uniAlpha,1.0f);
  glUniform1f(font->uniFnt,1.0f);
  
  glUniform3f(uniTint, color.x, color.y, color.z);

  glDrawArrays(GL_TRIANGLES, 0,text.length()*6);
  
  glUniform1f(font->uniFnt,0.0f);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
   glEnable(GL_CULL_FACE);
}
Label::~Label()
{

}

Font* Font::findFont(std::string fname)
{
  Font* font=0;
  for(int i=0;i<Font::fonts.size();i++)
  {
//       printf("@@@@@@@@@@@@@@@fname=%s, font num=%d font name=%s \n",fname.c_str(),i,Font::fonts[i]->name.c_str());
      if(strcmp(fname.c_str(),Font::fonts[i]->name.c_str())==0)
      {
        font=Font::fonts[i];break;
      }
  }
  return font;
}

void Font::cleanUp()
{
  for(int i=0;i<fonts.size();i++){delete fonts[i];}
  for(int i=0;i<efonts.size();i++){delete efonts[i];}
  FT_Done_FreeType(ft);
  Sprite::End();
  delete Sprite::sp;
  delete Label::label;
  delete TextField::ibeam;
}

