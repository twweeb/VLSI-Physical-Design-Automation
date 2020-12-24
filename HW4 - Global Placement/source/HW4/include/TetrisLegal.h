#ifndef _CTETRISLEGAL_
#define _CTETRISLEGAL_

#include "Placement.h"
#include "Row.h"
#include "Util.h"
#include <math.h>
#include <stdio.h>

using namespace std;
namespace Jin
{
    class CLegalLocation
    {
    public:
        CLegalLocation(const int &set_site_index, const double &set_xcoor) : m_site_index(set_site_index),
                                                                             m_xcoor(set_xcoor),
                                                                             m_shift(0), m_wirelength(0) {}
        int m_site_index; //Site index for the legal location (in y direction)
        double m_xcoor;   //x coordinate

        //Used for sort
        double m_shift;      //Record the shift from the original location of a cell
        double m_wirelength; //Record the wirelength when cell is placed to the location
    };

    //Compare m_shift of class 'CLegalLocation'
    class LessShift
    {
    public:
        bool operator()(const CLegalLocation &l1, const CLegalLocation &l2)
        {
            return l1.m_shift < l2.m_shift;
        }
    };

    //Compare m_wirelength of class 'CLegalLocation'
    class LessWirelength
    {
    public:
        bool operator()(const CLegalLocation &l1, const CLegalLocation &l2)
        {
            return l1.m_wirelength < l2.m_wirelength;
        }
    };

    //Used to keep the terminal information for each free site interval
    class CTerminal
    {
    public:
        enum Type
        {
            Left,
            Right
        };

        CTerminal(const double &set_xcoor, const Type &set_type, const int &set_row) : m_xcoor(set_xcoor), m_type(set_type), m_row(set_row) {}

        double m_xcoor; //X coordinate of this terminal
        Type m_type;    //Terminal direction (left or right)
        int m_row;      //Index of row which contains this terminal
    };

    class LessXCoor
    {
    public:
        static Placement *_placement;

        //Used to raise the priority of macros
        static double m_macro_factor;

        //Compare two cells (first key is x coordinate and second key is width, third is height)
        bool operator()(const int &mid1, const int &mid2);

        bool operator()(const CTerminal &t1, const CTerminal &t2)
        {
            return t1.m_xcoor < t2.m_xcoor;
        }
    };

    class LessXCoorMacroFirst
    {
    public:
        static Placement *_placement;

        //Compare two cells (first key is x coordinate and second key is width, third is height)
        bool operator()(const int &mid1, const int &mid2);

        bool operator()(const CTerminal &t1, const CTerminal &t2)
        {
            return t1.m_xcoor < t2.m_xcoor;
        }
    };

    //Macros have higher priority than cells with the same x coordinate
    class LessXCoorMacroPrior
    {
    public:
        static Placement *_placement;
        bool operator()(const int &mid1, const int &mid2);

        static bool BL(const int &mid1, const int &mid2);
        static bool BR(const int &mid1, const int &mid2);
        static bool TL(const int &mid1, const int &mid2);
        static bool TR(const int &mid1, const int &mid2);
    };

    inline double Rounding(const double &d1)
    {
        return floor(d1 + 0.5);
    }

} // namespace Jin

class CTetrisLegal
{
public:
    CTetrisLegal(Placement &placement);
    ~CTetrisLegal(void) {}

    Placement &_placement;

    //Solve the Tetris legalization, "true" indicates success
    bool Solve(const double &stop_prelegal_factor = 0.85); //Legalizer will stop at this factor

private:
    //******************************
    //* Functions for legalization *
    //******************************

    //MacroShifter all macros with height >= macroFactor*rowHeight
    //to a legal location without overlapping with other macro or preplaced modules
    //Parameter 'makeFixed' distinguishes to fixed all processed macros or not
    bool MacroShifter(const double &macroFactor, const bool &makeFixed);
    bool DoLeftRightUntil(const double &stop_prelegal_factor); //user can add DoRight oneself
    bool DoLeft(const double &prelegal_factor);                //Legalize to left

    bool DoLeftMacroFirst(const double &prelegal_factor); //Legalize to left and

    //***************
    //*   setting   *
    //***************

    //Prepare for left and right free sites
    void PrepareNonMacroLeftRightFreeSites(const vector<int> &macro_ids);
    void PrepareLeftRightFreeSites(void);
    void SetLeftFreeSites(void) { m_free_sites = m_left_free_sites; }

    //Refactoring for m_process_list
    //Put all movable module ids into m_process_list
    void SetProcessList(void);
    void SetNonMacroProcessList(const vector<int> &macro_ids);

    //Expand module width to site step
    void ExpandModuleWidthToSiteStep(void);

    //*****************************
    //*  Find the Legal Location  *
    //*****************************

    //Legalize all cells by cell order, "true" indicates success
    bool LegalizeByCellOrder(void);
    //Place a cell to legal location, "ture" indicates success
    bool PlaceCellToLegalLocation(const int &cellid);
    //Get all candidate legalization locations for a given cellid and return the best location
    void GetLegalLocations(const int &cellid, std::vector<Jin::CLegalLocation> &legalLocations);

    //*******************************************************
    //* Functions for legalization to certain vertical line *
    //*******************************************************
    void CalculateCellOrder(void); //Calculate legalization order by new cell locations

    void CalculateNewLocation(const double &prelegal_factor); //Calculate new cell locations to obtain legalization order

    void CalculateCellOrderMacroFirst(void); //Calculate the cell order for legalization and
                                             //macros have higher priority

    //******************************************
    //* Aggressively search for Macro and Cell *
    //******************************************

    //Aggressively diamond search the legal locations for a macro
    //If success, return 'true'
    bool AggressiveMacroDiamondSearch(const int &cellid,
                                      std::vector<Jin::CLegalLocation> &legalLocations);

    //Aggressively search cell legal locations
    //The search row limit will be expanded until the cell shift
    //is smaller than the search row limit
    //If it is impossible to find a legal location, it will return false
    bool AggressiveCellLegalLocationsSearch(const int &cellid,
                                            std::vector<Jin::CLegalLocation> &legalLocations);

    //Return the index of the best location of given locations for a given cell
    int ReturnBestLocationIndex(const int &cellid,
                                std::vector<Jin::CLegalLocation> &legalLocations);

    //Returns the minimu shift between the original cell location and
    //the locations in 'legalLocations'
    double ReturnMinimumShift(const int &cellid,
                              const std::vector<Jin::CLegalLocation> &locations);

    //For a given y coordinate (ycoor), return the site index it locates in
    //Each site covers [bottom, top) range
    int GetSiteIndex(const double &ycoor);

    //********************************
    //* Functions for Diamond Search *
    //********************************

    //Get a diamond free sites
    void GetDiamondRows(const CPoint &center,     //center of diamond
                        const double &radius,     //radius of diamond
                        std::vector<Row> &sites); //put the resulting diamond sites into "sites"

    //**************************************************************************
    //* Get legal locations for a macro and a set of given free sites          *
    //* The difference between 'TowardOrig' and 'TowardLeft' is that           *
    //* 'TowardOrig' finds the closest location to original location           *
    //* for each interval (multiple locations may be return for one row) while *
    //* 'TowardLeft' only returns the leftest legal location for each row      *
    //**************************************************************************
    //Note that since the return row index of legalLocations is based on the
    //input 'sites', and thus you have to update the index to original m_free_sites
    void GetMacroLegalLocationsTowardOrig(const int &cellid,                               //Target macro id
                                          const std::vector<Row> &sites,                   //Given free sites
                                          std::vector<Jin::CLegalLocation> &legalLocations //Put the resulting locations into legalLocations
    );

    //Get the legal locations for a given cellid within a given range
    //[start_site_index, end_site_index] of free sites
    void GetCellLegalLocationsTowardLeft(const int &cellid,
                                         const int start_site_index,
                                         const int end_site_index,
                                         std::vector<Jin::CLegalLocation> &legalLocations, //Put the resulting locations into legalLocations
                                         const double &left_bound);                        //Search left boundary

    //********************************
    //*   Update, Save, and Restore  *
    //********************************
    void MacroShifterSaveOrigPosition(void);
    void MacroShifterRestoreOrigPosition(void);
    void MacroShifterSaveBestPosition(void);
    void MacroShifterRestoreBestPosition(void);

    void SaveOrig(void);                                                        //Save original solution
    void SaveBest(const double &best_prelegal_factor, const double &best_cost); //Save best solution
    void RestoreOrig(void);                                                     //Restore orignal solution
    void RestoreBest(void);                                                     //Restore best solution

    //Remove the m_free_sites under a given module
    void UpdateFreeSite(const int &cellid);
    //Remove ocupied free site in m_free_sites[rowId] by the given xstart and width
    void UpdateFreeSite(const int &rowId, const double &xstart, const double &width);

    void SaveGlobalResult(void);
    void RestoreGlobalResult(void);
    void SaveModuleWidth(void);
    void RestoreModuleWidth(void);

    //To reapply MacroShifter, you have to execute RestoreFreeSite() first
    void RestoreFreeSite(void);

    //**********************************
    //* Information of Macros or Cells *
    //**********************************

    vector<int> m_cell_order; //Used as legalization order
    vector<Row> m_free_sites; //Record the unused sites
    vector<Row> m_left_free_sites;

    vector<CPoint> m_origLocations; //Record the original locations
    vector<CPoint> m_bestLocations; //Record the best locations
    vector<Row> m_best_sites;       //Record the resulting sites of the best result

    vector<CPoint> m_bestLocations_left;
    vector<Row> m_best_sites_left;
    vector<CPoint> m_globalLocations;

    vector<int> m_macro_ids;
    vector<double> m_orig_widths;

    //Variables and functions used for 4-directional macro shifter
    vector<CPoint> m_macro_shifter_orig_positions; //Record the original macro positions (left-bottom)
                                                   //before macro shifter
    vector<CPoint> m_macro_shifter_best_positions; //Record the best macro positions (left-bottom)
                                                   //in 4-directional macro shifter

    vector<int> m_process_list; //CalculateCellOrder() and CalculateNewLocation()
                                //only act on modules in the m_process_list

    //********************************
    //*           parameter          *
    //********************************
    int m_tetrisDir; // 0 both    1 left  2 right
    int m_resultTetrisDir;
    double m_resultTetrisDiff;
    double m_max_module_height;
    double m_max_module_width;
    double m_left_factor;                //X search by is calculated by the original cell location
                                         //Typical value is between 1.0 and 2.0
    double m_average_cell_width;         //Average standard cell width
    int m_row_limit;                     //Search for the m_row_limit upward and downward rows for
                                         //legal positions. -1 means the row_limit is determined by
                                         //m_row_factor * cell height
    double m_site_bottom, m_site_height; //Keep the bottom y coordinate of the sites and the
                                         //height of each site
    int m_unlegal_count;                 //Number of unlegalized cells
    double m_prelegal_factor;            //Prelegal factor
    double m_best_prelegal_factor;       //Record the best prelegal factor
    double m_best_cost;                  //Record the best cost
    double m_chip_left_bound;            //Record the chip left boundary
                                         //(for CalculateCellNewLocation())
                                         //best results for left (1) and right (2)
    double m_best_prelegal_factor_left;
    double m_best_cost_left;
    double m_row_factor;   //Search row limit = m_row_factor*(current cell's height)
    double m_macro_factor; //In LessXCoor, macro has higher priority by m_macro_factor
};

#endif
