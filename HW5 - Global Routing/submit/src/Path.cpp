#include "Path.hpp"

Path::Path(Node* endNode)
{
    Node *cur_grid = endNode;
    cost_ = cur_grid->cost();
    while (cur_grid != nullptr)
    {
        coords_.push_back(cur_grid->coord());
        edgeIDs_.push_back(cur_grid->edgeID());
        cur_grid = cur_grid->prev();
    }

    ASSERT (edgeIDs_.back() == -1);
    edgeIDs_.pop_back();

    std::reverse(coords_.begin(), coords_.end());
    std::reverse(edgeIDs_.begin(), edgeIDs_.end());
}