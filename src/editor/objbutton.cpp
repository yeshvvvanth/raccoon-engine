#include"entitymenu.h"
 short Inspector::H=0;
 short Inspector::y=0;
 short Inspector::x=0;



template <class T>
ObjButton<T>::ObjButton(float x, float y, std::string ltex,T* obj, Font* font): TextButton(x, y, ltex,font)
{
  object=obj;
  expand=false;
}
template <class T>
void ObjButton<T>::tick()
{
  Button::tick();
}

template class ObjButton<Script>;
template class ObjButton<Entity>;
template class ObjButton<RawMesh>;
template class ObjButton<Animation>;
template class ObjButton<Texture>;
template class ObjButton<Shader>;
template class ObjButton<Scene>;
template class ObjButton<Audio>;
template class ObjButton<Font>;

// template class ObjButton<Sprite>;
/////////////////////
