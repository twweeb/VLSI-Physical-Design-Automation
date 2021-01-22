#include "Parser.hpp"
#include "BStree.hpp"
#include "Floorplan.hpp"

void Floorplan::open(int argc, char *argv[])
{
    auto io_begin = std::chrono::steady_clock::now();
    parser.readARG(argc, argv);
    parser.readBlock(bStree);
    parser.readPin(terms);
    parser.readNet(nets);
    contour.set_outline(parser._get_outline());
    idx = (parser._get_seed()) ? ((parser._get_blockNum()/50 - 2) + ((parser._get_DSR() == 0.1) ? 0 : 1)) : -1;
    msg = parser._get_msg();
    if (idx != -1)
    {
        myBestWL = myBestWLPool[idx];
        seed = seedPool[idx];
    }
    
    //if (msg) std::cout << "I/O: " << std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::steady_clock::now() - io_begin).count() <<  " us    ";
}

int Floorplan::calWirelength () //HPWL
{
    int wirelength = 0;
    for (auto n: nets)
    {
        int tmp{0}, minX{INF}, maxX{0}, minY{INF}, maxY{0}, x{0}, y{0};
        node *curB = nullptr; terminal *curT = nullptr;
        for (auto i: n->blocks)
        {
            x = bStree._getNode(i)->x + bStree._getNode(i)->width/2;
            y = bStree._getNode(i)->y + bStree._getNode(i)->height/2;

            minX = std::min (minX, x);
            minY = std::min (minY, y);
            maxX = std::max (maxX, x);
            maxY = std::max (maxY, y);
        }

        for (auto i: n->terminals)
        {
            x = terms[i]->x; y = terms[i]->y;

            minX = std::min (minX, x);
            minY = std::min (minY, y);
            maxX = std::max (maxX, x);
            maxY = std::max (maxY, y);
        }

        wirelength += (maxX - minX) + (maxY - minY);
    }

    return wirelength;
}

void Floorplan::calDSR (int width)
{
    curDSR = ((double)(width*width)/(double)parser._get_blockArea()-1.0);
}

int Floorplan::cost (int wirelength)
{
    int maxX{0}, maxY{0}, cost{0}, x{0}, y{0};
    for (auto block: bStree._getTree())
    {
        x = block->x + block->width;
        y = block->y + block->height;
        maxX = std::max(maxX, x);
        maxY = std::max(maxY, y);
        if (x > contour._outline()) ++cost;
        if (y > contour._outline()) ++cost;
    }

    cost <<= 6;
    if (cost == 0) 
    {
        bStree.setCur(true);
        calDSR(std::max(maxX,maxY));
    }

    if (maxX > contour._outline()) cost += (maxX - contour._outline()) << 15;
    if (maxY > contour._outline()) cost += (maxY - contour._outline()) << 15;

    cost += wirelength >> 5;

    return cost;
}

void Floorplan::initFloorplan ()
{
    auto init_begin = std::chrono::steady_clock::now();
    int minCost = INF, minCostWL = INF, outline = contour._outline(), limit = (this->idx != -1) ? 1 : initTRYLimit;
    unsigned minCostSeed = (this->idx != -1) ? seed : 0;

    for (int i = 0; i < limit; ++i)
    {
        unsigned curSeed = (i == limit -1) ? minCostSeed : std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (nets.begin(), nets.end(), std::default_random_engine(curSeed));
        bStree.rearrange();
        contour.clearContour();

        int curX = 0;
        int root = -1, curParent = -1, rightParent = -1;
        node *cur = nullptr;
        for (auto n: nets)
        {
            for (auto b: n->blocks)
            {
                if (bStree._getNode(b)->isPlanned) continue;
                cur = bStree._getNode(b);
                if (root == -1)
                {
                    cur->isPlanned = true;
                    cur->x = 0;
                    cur->y = contour._getY(cur->x, cur->width, cur->height);
                    curX = cur->x + cur->width;
                    cur->parent = -1;
                    root = cur->id;
                    curParent = rightParent = root;
                }
                else if (curX + cur->width <= contour._outline())
                {
                    cur->isPlanned = true;
                    cur->x = curX;
                    cur->y = contour._getY(cur->x, cur->width, cur->height);
                    cur->parent = curParent;
                    bStree._getNode(curParent)->left = cur->id;
                    curParent = cur->id;
                    curX = cur->x + cur->width;
                }
                else
                {
                    cur->isPlanned = true;
                    cur->x = 0;
                    cur->y = contour._getY(cur->x, cur->width, cur->height);
                    cur->parent = rightParent;
                    bStree._getNode(rightParent)->right = cur->id;
                    curParent = rightParent = cur->id; 
                    curX = cur->x + cur->width;
                }
            }
            cur = nullptr;
        }
        
        int WL = calWirelength(), curCost = 0;
        if ((curCost = cost(WL)) <  minCost)
        {
            minCost = curCost;
            minCostWL = WL;
            minCostSeed = curSeed;
            //std::cout << "minCost = " << minCost << ", minCostSeed = " << minCostSeed << '\n';
        }
        sort (nets.begin(), nets.end());
        bStree.setRoot(root);
    }
    setinit(minCostWL, minCost);
    seed = minCostSeed;
    //if (msg) std::cout << "Init: " << std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::steady_clock::now() - init_begin).count() <<  " us, ";
}

void Floorplan::SA ()
{
    auto sa_begin = std::chrono::steady_clock::now();
    int outline = contour._outline(), localminCost = INF, bestminCost = INF, curCost = 0, curWL = INF, curBestWL = INF, penality = 0;
    contour.update(bStree);

    bool satisfy = false;
    localminCost = curCost = cost(calWirelength());
    
    for (int i = 0; (i < saTRYLimit || !bStree._getBest()); ++i)
    {
        //if (i%100000 == 0) cout << i*100/saTRYLimit << " %\n";
        bStree.saveLocal(localminCost = curCost);
        bStree.perturb();
        contour.update(bStree);
        curWL = calWirelength();
        curCost = cost(curWL);
        if (curCost < localminCost && ~bStree._getBest())
        {
            bStree.saveBest(bestminCost = curCost);
            penality = 0;

            if (bStree._getCur()) 
            {
                if (curWL <= myBestWL) break;
                bStree.setBest(true);
                admissible = true;
                curBestWL = std::min (curBestWL, curWL);
            }
            continue;
        }

        if (curWL < curBestWL && bStree._getBest() && bStree._getCur())
        {
            bStree.saveBest(bestminCost = curCost);
            curBestWL = std::min (curBestWL, curWL);
            
            if (curBestWL <= myBestWL) 
            {
                flag = true;
                break;
            }
            penality = 0;
            continue;
        }

        ++ penality;
        
        if (penality < PENALTY_LIMIT) curCost = bStree.getLocal();
        
        else if (penality == PENALTY_LIMIT) 
        {
            penality = 0;
            for (int j = 0; j < 1<<4; ++j) bStree.perturb();
            contour.update(bStree);
            curCost = cost(calWirelength());
        }
        if (flag || (std::chrono::duration_cast<std::chrono::seconds> (std::chrono::steady_clock::now() - sa_begin).count() > saTime && bStree._getBest())) break;
    }
    bStree.getBest();
    contour.update(bStree);
    curWL = calWirelength();
    curCost = cost(curWL);

    setfinal(curWL, curCost);
    //if (msg) std::cout << "SA: " << std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::steady_clock::now() - sa_begin).count() <<  "\n\n";
}

void Floorplan::show_msg()
{
    std::cout << '\n';
    std::cout << "    Nets Number: " << parser._get_netNum() << ", Outline: " << parser._get_outline() << '\n';
    std::cout << "    Initial Wirelength: " << initWL << ", Initial Cost: " << initCost << ", Seed: " << seed <<'\n';
    std::cout << "    Final Wirelength: " << finalWL << ", Final Cost: " << finalCost << ", DSR: " << curDSR <<'\n';
    std::cout << "    Time Cost: " << std::chrono::duration_cast<std::chrono::seconds> (std::chrono::steady_clock::now() - program_begin).count() << " s\n\n";
}

void Floorplan::finalAns (){parser.finalAns (bStree, finalWL);}
void Floorplan::visualize (){parser.visualize (bStree);}