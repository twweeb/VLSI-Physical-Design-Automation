#ifndef ARGHANDLER_H
#define ARGHANDLER_H

#include <map>
#include <string>

using namespace std;

class CArgHandler
{

public:
    CArgHandler();
    ~CArgHandler();

    void Init(const int argc, char *argv[]);

    bool CheckExist(string caption);
    bool IsDev() { return m_isDev; };

private:
    char **m_argv;
    int m_argc;
    bool m_isDev;
    //map< string, string > m_override;

private:
    int FindCaptionIndex(const string caption);
};

extern CArgHandler gArg; // global variable

#endif
