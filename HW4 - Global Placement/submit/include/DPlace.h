#ifndef _DPLACE_
#define _DPLACE_

#include "Placement.h"
#include <assert.h>
#include <climits>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <set>
#include <vector>

class CDetailPlacer
{
public:
    CDetailPlacer(Placement &placement) : _placement(placement) {}

    ~CDetailPlacer(void) {}

    Placement &_placement;
    void DetailPlace(void);
};

class de_Point
{
public:
    de_Point()
    {
    }

    double x;
    double y;
};

template <class T>
class Array2D
{
private:
    T *data;
    int n;

public:
    Array2D(int num)
    {
        n = num;
        data = new T[n * n];
    }

    ~Array2D()
    {
        delete[] data;
    }
    void put(int i, int j, T d)
    {
        data[i * n + j] = d;
    }
    T get(int i, int j)
    {
        return data[i * n + j];
    }
};

class bimatching
{

public:
    bimatching(int num) : zeros(num), covers(num), costs(num)
    {

        n = num;
        next = 0;
        starcol = new bool[n];
        starrow = new bool[n];
        covercol = new bool[n];
        coverrow = new bool[n];

        for (int i = 0; i < n; i++)
        {
            starcol[i] = false;
            starrow[i] = false;
            covercol[i] = false;
            coverrow[i] = false;
            for (int j = 0; j < n; j++)
            {
                zeros.put(i, j, 0);
                covers.put(i, j, 0);
                costs.put(i, j, 0);
            }
        }
    }
    ~bimatching()
    {
        delete[] starcol;
        delete[] starrow;
        delete[] covercol;
        delete[] coverrow;
    }

    void find();
    void step3();
    void step4();
    void step5();
    void step6(double low);

    int findstar(int y); // find a star Sn in column y , return row id

    int findprime(int x); // find a prime Pn in row x,return col id

    int n;
    int next;
    double s6in;
    int p0x;
    int p0y;
    Array2D<int> zeros;
    Array2D<int> covers;
    Array2D<double> costs;
    bool *starcol;
    bool *starrow;
    bool *covercol;
    bool *coverrow;

    //vector<int> getresult();
    void getresult(vector<int> &v);
    //void show();
};

class de_Row
{
public:
    de_Row(double m_x = 0, double m_y = 0, double m_length = 0)
    {
        this->m_x = m_x;
        this->m_y = m_y;
        this->m_length = m_length;
    }
    bool insert_module(double, double, int);
    bool add_empty(double, double);    // x,w
    bool remove_empty(double, double); // x,w

    map<double, double> m_empties; //x-coordinate,length
    map<double, int> m_rowmodule;  //x-coordinate,module ID
    double m_x, m_y, m_length;
};
class lap2
{
public:
    lap2(int deg)
    {
        m_deg = deg;
        m_cost.resize(m_deg);
        for (unsigned int i = 0; i < m_cost.size(); ++i)
        {
            m_cost[i].resize(m_deg, 0);
        }
        m_assignment.resize(deg, 0);
        INF = INT_MAX;
        verbose = false;
    }
    ~lap2(void) {}

    void put(const int &i, const int &j, double wl)
    {
        m_cost[i][j] = static_cast<int>(wl);
    }

    int INF;
    int m_deg;
    vector<vector<int> > m_cost;
    vector<int> m_assignment;
    int lap_solve();
    void getresult(vector<int> &v)
    {
        v = this->m_assignment;
    }
    bool verbose;
};
class CNetLengthCalc
{
public:
    CNetLengthCalc(Placement &placement, int mID)
    {
        _placement = &placement;
        m_mID = mID;
    }

    void init();
    double calc();

    //for find multiple module's opt window (find X coordinate only!!)
    CNetLengthCalc(Placement &placement, const set<int> &mset)
    {
        assert(mset.size() > 0);
        _placement = &placement;
        m_moduleSet = mset;
    }
    void initS(const set<int> cellsInRow, const double &left, const double &right);
    void calcOptWindowS(double &maxX, double &minX, const set<int> cellsInRow, const double &left, const double &right);

private:
    vector<double> m_bottom;
    vector<double> m_top;
    vector<double> m_left;
    vector<double> m_right;
    vector<vector<int> > m_pinID;
    vector<bool> m_validNet;
    set<int> m_moduleSet;

    unsigned m_mID;
    Placement *_placement;
};
class de_Detail
{
public:
    de_Detail(Placement &place)
    {
        direction = 0;
        MAXWINDOW = 64;
        MAXMODULE = MAXWINDOW;
        pIndepent = false;
        pRW = false;
        _placement = &place;
        ROWHEIGHT = (double)place.getRowHeight();
        num_rows = (int)place.m_sites.size();

        bottom = place.rectangleChip().bottom();
        double row_length = place.rectangleChip().right() - place.rectangleChip().left();
        // initailize de_Row
        for (int i = 0; i < num_rows; i++)
        {
            de_Row row(place.rectangleChip().left(), place.m_sites[i].y(), row_length);
            m_de_row.push_back(row);
        }

        // build row empty map
        for (unsigned int i = 0; i < place.m_sites.size(); i++)
        {
            int pos = (int)((place.m_sites[i].y() - place.rectangleChip().bottom()) / ROWHEIGHT);
            for (unsigned int j = 0; j < place.m_sites[i].m_interval.size(); j = j + 2)
            {
                m_de_row[pos].m_empties[place.m_sites[i].m_interval[j]] = place.m_sites[i].m_interval[j + 1] - _placement->m_sites[i].m_interval[j];
            }
        }

        //insert cells
        for (unsigned int i = 0; i < (unsigned int)place.numModules(); i++)
        {
            Module &curModule = place.module(i);
            if (curModule.isFixed() == false)
            {
                int height = 1;
                if (curModule.height() > place.getRowHeight())
                {
                    height = int(curModule.height() / place.getRowHeight());
                    if (curModule.height() * height < place.getRowHeight())
                        height++;
                }
                int pos = y2rowID(curModule.y());
                for (int j = 0; j < height; j++)
                {
                    m_de_row[pos + j].insert_module(curModule.x(), curModule.width(), i);
                }
            }
        }
    }
    int y2rowID(double y)
    {
        return (int)((y - _placement->rectangleChip().bottom()) / _placement->getRowHeight());
    }

    void detail(double, double, double, double); //start x, start y, width, height
    void grid_run(int, int);                     //int window,  times of rowheight,int overlap
    bool isConnection(int, int);                 //mID1,mID2, return true if no connection
    void remove_module(int, int);                // mid,rowID

    double ROWHEIGHT;
    double bottom;
    int num_rows;
    int MAXWINDOW;
    int MAXMODULE;
    int direction;
    bool pIndepent;
    bool pRW;
    Placement *_placement;
    vector<de_Row> m_de_row;
};

class deRunDetail
{
public:
    deRunDetail(Placement &placement) : _placement(placement), m_stop(0.2) {}
    ~deRunDetail() {}
    //void runDetail(const CParamPlacement& param, CPlaceDB& placedb);
    void runDetail();
    Placement &_placement;
    // 2006-09-30 (donnie)
    double m_stop;
};

#endif
