#include <cstring>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "BStree.hpp"

BStree::BStree() {root = -1; curStatus = false; bestStatus = false;} 
BStree::~BStree() {root = -1;}

node* BStree::_getNode(int id) {return (id == -1) ? tree[root] : tree[id];}
int BStree::_getRoot() {return root;}
std::vector<node*>& BStree::_getTree() {return tree;}
bool BStree::_getCur() {return curStatus;}
bool BStree::_getBest() {return bestStatus;}
void BStree::setCur (bool set) {curStatus = set;}
void BStree::setBest (bool set) {bestStatus = set;}
void BStree::setblockNum (int blockNum) {this->blockNum = blockNum; tree.reserve(blockNum); local.resize(blockNum); best.resize(blockNum);}
void BStree::push_back (node &ptr) {tree.push_back(&ptr);}
void BStree::setRoot(int id) {root = id; tree[root]->parent = -1;}
void BStree::rearrange() 
{ 
    for (auto block: this->tree)
    {
        block->isPlanned = false; 
        block->parent = block->left = block->right = -1;
    }
}
void BStree::printTree() {
    int cur = root, rPar = root;
    std::cout << cur;
    cur = tree[cur]->left;
    while (cur!=-1)
    {
        std::cout << "->" << cur;
        if (tree[cur]->left != -1) 
        {
            if (tree[cur]->right != -1) std::cout << "*";
            cur = tree[cur]->left;
        }
        else 
        {
            cur = tree[rPar]->right;
            if (cur!=-1) std::cout << "\n |";
            else std::cout << '\n';
            rPar = cur;
        }
    }
}
void BStree::flip (){}

void BStree::saveLocal (int cost) 
{
    localCost = cost;
    for (int i = 0; i < blockNum; ++i) 
    {
        node *tmp = new node(), *ori = _getNode(i);
        tmp->id = i; 
        tmp->width = ori->width;
        tmp->height = ori->height; 
        tmp->x = ori->x;
        tmp->y = ori->y;
        tmp->parent = ori->parent;
        tmp->left = ori->left;
        tmp->right = ori->right;
        tmp->isRotated = ori->isRotated;
        local[i] = tmp;
    }
    localRoot = root;
}
int BStree::getLocal ()
{
    for (int i = 0; i < blockNum; ++i) tree[i] = local[i];
    root = localRoot;
    return localCost;
}
void BStree::saveBest (int cost) 
{
    bestCost = cost;
    for (int i = 0; i < blockNum; ++i)
    {
        node *tmp = new node(), *ori = _getNode(i);
        tmp->id = i; 
        tmp->width = ori->width;
        tmp->height = ori->height; 
        tmp->x = ori->x;
        tmp->y = ori->y;
        tmp->parent = ori->parent;
        tmp->left = ori->left;
        tmp->right = ori->right;
        tmp->isRotated = ori->isRotated;
        best[i] = tmp;
    }
    bestRoot = root;
}
void BStree::getBest () 
{
    for (int i = 0; i < blockNum; ++i) tree[i] = best[i];
    root = bestRoot;
}

void BStree::rotate (int p)
{
    tree[p]->isRotated = !(tree[p]->isRotated);
    int w = tree[p]->width;
    tree[p]->width = tree[p]->height;
    tree[p]->height = w;
}
void BStree::move (int a, int b)
{
    if (a == b || tree[a]->parent == b || tree[b]->parent == a) return;
    swap(a, b);

    int aPar = tree[a]->parent, tmp;
    if (tree[aPar]->left == a) tree[aPar]->left = -1;
    else tree[aPar]->right = -1;

    do {tmp = rand() % blockNum;}
    while( tmp == a || (tree[tmp]->left!=-1 && tree[tmp]->right!=-1) );

    tree[a]->parent = tmp;
    if (tree[tmp]->left == -1) tree[tmp]->left = a;
    else tree[tmp]->right = a;
}

void BStree::swap (int a, int b)
{
    //std::cout << "Swap " << a << ", " << b << '\n' ;

    if (a == b) return;
    int aPar = tree[a]->parent, bPar = tree[b]->parent, 
        aLeft = tree[a]->left, aRight = tree[a]->right,
        bLeft = tree[b]->left, bRight = tree[b]->right;
    
    //std::cout << "hello 2\n" ;
    if (aPar == -1) 
    {
        root = b;
        //std:: cout << "a is root\n";
    }
    else if (tree[aPar]->left == a) tree[aPar]->left = b;
    else tree[aPar]->right = b;

    //std::cout << "hello 3\n" ;
    if (bPar == -1) 
    {
        root = a;
        //std:: cout << "b is root\n";
    }
    else if (tree[bPar]->left == b) tree[bPar]->left = a;
    else tree[bPar]->right = a;
    
    //std::cout << "hello 4\n" ;
    if (aLeft != -1) tree[aLeft]->parent = b; if (aRight != -1) tree[aRight]->parent = b;
    if (bLeft != -1) tree[bLeft]->parent = a; if (bRight != -1) tree[bRight]->parent = a;

    //std::cout << "hello 5\n" ;
    tree[a]->parent = bPar; tree[b]->parent = aPar;
    tree[a]->left = bLeft; tree[b]->left = aLeft;
    tree[a]->right = bRight; tree[b]->right = aRight;

    // int r = rand() % 4;
    // if (r == 0) rotate(a);
    // else if (r == 1) rotate(b);
    // else if (r == 3) {rotate(a); rotate(b);}
}

void BStree::perturb ()
{
    int perturb_num = rand() % 5, a = rand() % blockNum, b = 0;
    if (perturb_num < 2) rotate(a);
    else if (perturb_num < 4)
    {
        do {b = rand() % blockNum;}
        while( a == b || tree[a]->parent == b || tree[b]->parent == a);
        //std::cout << "before swap\n";
        swap(a, b);
        //std::cout << "after swap\n";
    }
    else
    {
        b = a;
        while (tree[b]->left!=-1 || tree[b]->right!=-1)
        {
            int r = rand() % 2;
            b = (r) ? ((tree[b]->left != -1) ? tree[b]->left : tree[b]->right) : ((tree[b]->right != -1) ? tree[b]->right : tree[b]->left);
        }
        move (a, b);
    }

    setCur(false);
}

Contour::Contour (int outline)
{
    this->tryLength = outline<<2;
    this->outline = outline;
    this->horizontal = (int*)malloc(sizeof(int)*tryLength);
    memset (this->horizontal, 0, this->tryLength*sizeof(int));
}
Contour::~Contour() {delete [] horizontal, vertical;}
int Contour::_outline() {return outline;}
bool Contour::_isLegal(int l) {return l <= tryLength;}
int Contour::_H(int i) {return horizontal[i];}
void Contour::update(BStree &bStree) {clearContour(); bStree_update(bStree._getRoot(), bStree, false);}
void Contour::clearContour() {
    memset (horizontal, 0, tryLength*sizeof(int));
}
int Contour::_getY (int x, int width, int height)
{
    int maxH = 0, newH = 0;

    for (int i = x; i < x+width; ++i)
    {
        if (maxH < horizontal[i]) maxH = horizontal[i];
    }
    newH = maxH + height;
    //std::cout << "x = " << x << ", width = " << width <<  ", height = " << height << ", maxH = " << maxH <<", newH = "<< newH << '\n';
    for (int i = x; i < x+width; ++i) horizontal[i] = newH;
    return maxH;
}

void Contour::bStree_update (int cur, BStree &bStree, bool isRight)
{

    if (cur == bStree._getRoot()) bStree._getTree()[cur]->x = 0;
    else if (isRight) bStree._getNode(cur)->x = bStree._getNode(bStree._getNode(cur)->parent)->x;
    else bStree._getNode(cur)->x = bStree._getNode(bStree._getNode(cur)->parent)->x + bStree._getNode(bStree._getNode(cur)->parent)->width;

    bStree._getNode(cur)->y = _getY(bStree._getNode(cur)->x, bStree._getNode(cur)->width, bStree._getNode(cur)->height);

    if (bStree._getNode(cur)->left != -1) bStree_update(bStree._getNode(cur)->left, bStree, false);
    if (bStree._getNode(cur)->right != -1) bStree_update(bStree._getNode(cur)->right, bStree, true);
}