#include "Parser.hpp"
#include "BStree.hpp"

void Parser::readARG (int argc, char *argv[])
{
    try
    {
        hardblocks_data.open(argv[1]);
        nets_data.open(argv[2]);
        term_data.open(argv[3]);
        floorplan.open(argv[4]);
        DSR = std::stod(argv[5]);
        drawfig.open(std::string(argv[4])+".fig");
        
        if (!hardblocks_data.good() || !nets_data.good() || !term_data.good() || !floorplan.good()) throw 1;
    }
    catch (int e)
    {
        if (e == 1)  std::cout << "File Opening Error.\n";
        else std::cout << "Parsing Error. Message Code: " << e << '\n';
        exit(-1);
    }
}

void Parser::readBlock(BStree &bStree)
{
    std::string _t, val[2];

    hardblocks_data >> _t >> _t >> blockNum;
    hardblocks_data >> _t >> _t >> termNum;
    bStree.setblockNum(blockNum);

    for (int i = 0; i < blockNum; ++i)
    {
        node *tmp = new node();
        hardblocks_data >> _t >> _t >> _t >> _t >> _t >> _t >> _t;
        hardblocks_data >> val[0] >> val[1];
        tmp->id = i;
        tmp->parent = tmp->left = tmp->right = -1;
        tmp->width = stoi(val[0].substr(1,val[0].length()-2));
        tmp->height = stoi(val[1].substr(0,val[1].length()-1));
        hardblocks_data >> _t >> _t;

        tmp->area = tmp->width * tmp->height;
        blockArea += tmp->area;
        bStree.push_back(*tmp);
        //if ((double)tmp->width/(double)tmp->height < initRotateRatio) bStree.rotate (tmp->id);
    }

    hardblocks_data.close();
    update_outline();
}

void Parser::readPin (std::vector<terminal*> &terms)
{
    std::string _t;
    terms.reserve(termNum);

    for (int i = 0; i < termNum; ++i)
    {
        terminal* tmp = new terminal();
        term_data >> _t >> tmp->x >> tmp->y;
        tmp->id = i;
        terms.push_back(tmp);
    }

    term_data.close();
}

void Parser::readNet (std::vector<net*> &nets)
{
    std::string _t, str;
    nets_data >> _t >> _t >> netNum;
    nets_data >> _t >> _t >> _t;
    nets.reserve(netNum);

    for (int i = 0; i < netNum; ++i)
    {
        net* tmp = new net();
        tmp->id = i;
        
        nets_data >> _t >> _t >> tmp->degree;
        for (int j = 0; j < tmp->degree; ++j)
        {
            nets_data >> str;
            if (str[0] - 'p' == 0) tmp->terminals.insert(stoi(str.substr(1, str.length()-1))-1);
            else tmp->blocks.insert(stoi(str.substr(2, str.length()-1)));
        }
        nets.push_back(tmp);
    }

    nets_data.close();
}

void Parser::update_outline()
{
    outline = sqrt(blockArea*(1.0 + DSR));
}

void Parser::visualize (BStree &bStree)
{
    if (!drawfig.is_open()) return;
    drawfig << "-1 0 0 " << outline << ' ' << outline << '\n';
    for (auto block: bStree._getTree())
        drawfig << block->id << ' ' << block->x << ' ' << block->y << ' '
                << block->width << ' ' << block->height << '\n';
    drawfig.close();
}

void Parser::finalAns (BStree &bStree, int wirelength)
{
    floorplan << "Wirelength " << wirelength << '\n' << "Blocks\n";
    for (auto block: bStree._getTree())
        floorplan << "sb" << block->id << ' ' << block->x << ' ' << block->y << ' ' << block->isRotated << '\n';
    floorplan.close();
}