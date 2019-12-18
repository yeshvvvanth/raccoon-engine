#pragma once
#include<vector>
class Font;
struct Node 
{
    Node* left;
    Node* right;
    // The top-left coordinate of the rectangle.
    int x;
    int y;
    // The dimension of the rectangle.
    int width;
    int height;
};



void Init(int width, int height,Font* font);

Node *Insert(Node *node, int width, int height);

unsigned char* Pack(Font* font,int* width,int* height);