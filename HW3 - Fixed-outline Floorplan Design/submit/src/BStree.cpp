#include "BStree.hpp"

BStree::BStree() 
{
    blockNum = 0;
    localCost = bestCost = INF;
    root = localRoot = bestRoot = -1;
    curStatus = bestStatus = false;
}

BStree::~BStree() {}

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
    if (a == b) return;
    int aPar = tree[a]->parent, bPar = tree[b]->parent, 
        aLeft = tree[a]->left, aRight = tree[a]->right,
        bLeft = tree[b]->left, bRight = tree[b]->right;
    
    if (aPar == -1)  root = b;
    else if (tree[aPar]->left == a) tree[aPar]->left = b;
    else tree[aPar]->right = b;

    if (bPar == -1) root = a;
    else if (tree[bPar]->left == b) tree[bPar]->left = a;
    else tree[bPar]->right = a;

    if (aLeft != -1) tree[aLeft]->parent = b; if (aRight != -1) tree[aRight]->parent = b;
    if (bLeft != -1) tree[bLeft]->parent = a; if (bRight != -1) tree[bRight]->parent = a;

    tree[a]->parent = bPar; tree[b]->parent = aPar;
    tree[a]->left = bLeft; tree[b]->left = aLeft;
    tree[a]->right = bRight; tree[b]->right = aRight;

    int r = rand() % 4;
    if (r == 0) rotate(a);
    else if (r == 1) rotate(b);
    else if (r == 3) {rotate(a); rotate(b);}
}

void BStree::perturb ()
{
    int perturb_num = rand() % 5, a = rand() % blockNum, b = 0;
    if (perturb_num < 2) rotate(a);
    else if (perturb_num < 5)
    {
        do {b = rand() % blockNum;}
        while( a == b || tree[a]->parent == b || tree[b]->parent == a);
        swap(a, b);
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