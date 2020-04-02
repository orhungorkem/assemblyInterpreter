#ifndef ASSEMBLYINTERPRETER_OPERATE_H
#define ASSEMBLYINTERPRETER_OPERATE_H

#include <string>
#include <vector>
#include <stack>

using namespace std;

class operate {
public:

    vector<pair<string,unsigned short>>memory;  //memory will keep a pair of string(for variables) and unsigned short(for simplicity) although an index of memory will never keep word size data
    int endingline;  //holds the line of int 20h(defined in parse method)
    int memorycounter=0;   //holds the index for variables to be stored (updated in parse and assignvar methods)
    int accessPermit; //keeps the minimum index available that does not keep instructions

    vector<pair<string,int>>labelLines; //keeps the labels' names and their line numbers(which will ease the jumps)

    //registers are initiated with 0, and as unsigned short for operational simplicity but 8 bit registers will never store word size data
    unsigned short ax=0;
    unsigned short bx=0;
    unsigned short cx=0;
    unsigned short dx=0;
    unsigned short di=0;
    unsigned short sp=65534;  //stack starts to be stored from FFFE
    unsigned short si=0;
    unsigned short bp=0;
    unsigned short ah=0;
    unsigned short al=0;
    unsigned short bh=0;
    unsigned short bl=0;
    unsigned short ch=0;
    unsigned short cl=0;
    unsigned short dh=0;
    unsigned short dl=0;

    //flags
    bool zf=0;  //zefo flag
    bool af=0;  //auxilary
    bool cf=0;  //carry
    bool sf=0;  //sign
    bool of=0;  //overflow



    /*
     * FUNCTIONS ARE EXPLAINED IN ABOVE COMMENTS OF THEIR DEFINITION
     */

    //HELPER FUNCTIONS
    operate();
    void assignvar(vector<vector<string>>&tokens,bool& valid);
    bool is_number(const string& s);
    bool isValidVariable(string s);
    int redirect(int linenum, vector<string> line);
    unsigned short* arriveptr(string& arrival,bool& a8,bool& f8,unsigned short &possiblevar,bool& memoryaccess,bool&offset);
    unsigned short* departptr(string departure,bool&a8,bool& d8,bool& f8,unsigned short &possiblevar,bool& memoryaccess,bool&offset);
    int getLabelLine(string label);
    int hexToDec(string in);
    string decToHex(int in);
    string decToBin(int in);
    int binToDec(string in);
    void updateAxToAh(unsigned short*ptr);
    void updateAhToAx(unsigned short*ptr);
    int getMemoryAddress(unsigned short* ptr);
    unsigned short loadWordSizeData(int index);
    void writeWordSizeData(int index,unsigned short data);


    //INSTRUCTION FUNCTIONS
    int mov(unsigned short*ar,unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess);
    int add(unsigned short*ar,unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess);
    int sub(unsigned short*ar,unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess);
    int mul(unsigned short* ar,int linenum,bool bytes2,bool armemoryaccess);
    int div(unsigned short* ar,int linenum,bool bytes2,bool armemoryaccess);
    int notfunc(unsigned short *ar,int linenum,bool bytes2,bool armemoryaccess);
    int andorxorfunc(unsigned short*ar,unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess,int option);
    int rcrrclshrshl(unsigned short*ar, unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,int option);
    int cmp(unsigned short*ar, unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess);
    int pushpop(unsigned short*ar,int linenum,bool armemoryaccess,int option);


    //FLAG FUNCTIONS
    bool ofSet(unsigned short first, unsigned short second,unsigned short result,bool bytes2,int optype);
    bool sfSet(unsigned short end,bool bytes2);
};


#endif //ASSEMBLYINTERPRETER_OPERATE_H