#ifndef BSTREE_H
#define BSTREE_H

#include <vector>
struct node
{
    int id, width, height, area, x, y, parent, left, right;
    bool isRotated = false, isPlanned = false;
};

class BStree 
{
    private:
        std::vector<node*> tree, local, best;
        int blockNum, localCost, bestCost, root, localRoot, bestRoot;
        bool curStatus, bestStatus;

    public:
        BStree();
        ~BStree();

        node* _getNode(int id);
        bool _getCur ();
        bool _getBest ();
        int _getRoot();
        std::vector<node*>& _getTree();
        void rearrange ();
        void printTree ();
        void setblockNum (int blockNum);
        void push_back (node &ptr);
        void saveLocal (int cost);
        int getLocal ();
        void saveBest (int cost);
        void getBest ();
        void setCur (bool set);
        void setBest (bool set);
        void setRoot(int id);
        void flip ();
        void rotate (int p);
        void move (int a, int b);
        void swap (int a, int b);
        void perturb ();
};

class Contour
{
    private:
        int *horizontal, *vertical;
        int tryLength, outline;

    public:
        Contour (int outline);
        ~Contour ();
        int _getY (int x, int width, int height);
        int _outline ();
        bool _isLegal (int l);
        int _H (int i);
        void bStree_update (int cur, BStree &bStree, bool isRight);
        void update (BStree &bStree);
        void clearContour ();
};

#endif