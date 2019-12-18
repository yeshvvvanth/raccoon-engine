#include"components.h"
#include"inspector.h"

template<class T>
struct EList:public Menu{
   EList(float X, float Y, Font* fnt,std::vector<T*>& vec);
   T *activeObj;
   std::vector<T*> *tvec;
   ObjButton<T>* addButton(T* obj);
   void repos(TextButton* tb,char pos);
   void refresh();
   void tick();
   ~EList();
};

class EMenu:public Menu{
public:
  static EMenu* emenu;
  static Entity* activeEntity;
  static std::string pscnname;
  static Scene* pactiveScene;
  static bool addable(Entity* tent);
  static int entsize;
  bool dragged;
  Inspector* inspector;
  Sprite *sp;
  void sort();
  void tick();
  void cmenuUpdate();
  void reset();
  Scene* cscene;
  std::vector<Entity*> copied;
  ObjButton<Entity>* find(Entity* ent);
  void repos(TextButton* tb,char pos);
  ObjButton<Entity>* addButton(Entity* ent);
  ContextMenu *cmenu;
  EMenu(Font* fnt);
  ~EMenu();
  
};

////////
