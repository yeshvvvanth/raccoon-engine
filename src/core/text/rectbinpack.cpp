#include"rectbinpack.h"
#include"font.h"

Node froot;
int binWidth;
int binHeight;
std::vector<Node*> nodes;
void Init(int width, int height,Font* font)
{
    binWidth = width;
    binHeight = height;
    froot.left = froot.right = 0;
    froot.x = froot.y = 0;
    froot.width = width;
    froot.height = height;
    
    for(int i=0;i<nodes.size();i++){
	if(nodes[i])
	{
	  delete nodes[i];
	  nodes[i]=0;
	}
    }
    nodes.clear();	
}
int pad=1;

Node * Insert(Node *node, int width, int height)
{
    // If this node is an internal node, try both leaves for possible space.
    // (The rectangle in an internal node stores used space, the leaves store free space)
    if (node->left || node->right)
    {
        if (node->left)
        {
	    Node *newNode = Insert(node->left, width, height);
	    if (newNode)
		    return newNode;
        }
        if (node->right)
        {
	    Node *newNode = Insert(node->right, width, height);
	    if (newNode)
		    return newNode;
	}
	return 0; // Didn't fit into either subtree!
    }

    // This node is a leaf, but can we fit the new rectangle here?
    if (width > node->width || height > node->height)
	    return 0; // Too bad, no space.

    // The new cell will fit, split the remaining space along the shorter axis,
    // that is probably more optimal.
    int w = node->width - width;
    int h = node->height - height;
    node->left = new Node;nodes.push_back(node->left);
    node->right = new Node;nodes.push_back(node->right);

    node->left->right=0;node->left->left=0;
    node->right->right=0;node->right->left=0;
    if (w <= h) // Split the remaining space in horizontal direction.
    {
	    node->left->x = node->x + width;
	    node->left->y = node->y;
	    node->left->width = w;
	    node->left->height = height;

	    node->right->x = node->x;
	    node->right->y = node->y + height;
	    node->right->width = node->width;
	    node->right->height = h;
    }
    else // Split the remaining space in vertical direction.
    {
	    node->left->x = node->x;
	    node->left->y = node->y + height;
	    node->left->width = width;
	    node->left->height = h;

	    node->right->x = node->x + width;
	    node->right->y = node->y;
	    node->right->width = w;
	    node->right->height = node->height;
    }

    // This node is now a non-leaf, so shrink its area - it now denotes
    // *occupied* space instead of free space. Its children spawn the resulting
    // area of free space.
    node->width = width;
    node->height = height;
    return node;
}
bool tryInsert(Font* font){
  bool success=false;
  for(int i=0;i<font->Characters.size();i++){
    Character* c=&font->Characters[i];
    
    c->node=Insert(&froot,c->width+pad,c->height+pad);
    
    success=(c->node!=0);
    if(success){

    }
    else{break;}
  }
  
  return success;
}
unsigned char* Pack(Font* font,int* fwidth,int* fheight)
{
  unsigned char* fimage;
  int b=32;
  while(true){
    Init(b,b,font);
    if(tryInsert(font))
	break;
    b*=2;
  }
  
  
  fimage=new unsigned char[binWidth*binHeight];
  memset(fimage,0,binWidth*binHeight);
  ///add font images to image buffer
  for(int i=0;i<font->num_glyphs;i++){
      Character* c=&font->Characters[i];
      GLubyte ch=i+font->st_char;
      
      FT_Load_Char(font->face,ch, FT_LOAD_RENDER);
      
      
      c->u=(float)(c->node->x)/binWidth;
      c->v=(float)(c->node->y)/binHeight;
      if(i==115 || i==117){
// 	  printf("c.n.u= %d \n",c->node->x);
// 	  printf("c.n.v= %d \n",c->node->y);
// 	  printf("width = %d \n",c->node->width);
// 	  printf("height = %d \n",c->node->height);
// 	  printf("c.u= %f \n",c->u);
// 	  printf("c.v= %f \n",c->v);
// 	  printf("__________\n");
      }
      unsigned int j=c->node->y,lim=c->node->y+c->height;
      for(;j<lim;j++){
 	
	memcpy(fimage+(c->node->x+binWidth*j),font->face->glyph->bitmap.buffer+(c->width*(j-c->node->y)),c->width);
	
      }

  }
  Init(b,b,font);
  
  *fwidth=binWidth;
  *fheight=binHeight;
  return fimage;
}



