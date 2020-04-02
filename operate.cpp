#include <iostream>
#include "operate.h"
#include <math.h>
#include <stack>
using namespace std;


/*
 * CONSTRUCTOR
 */
operate::operate(){
    memory.resize(65536);  //memory size update
}


/*
 * Takes a string as input and returns whether it only contains digits or not
 */
bool operate::is_number(const string& s){
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


/*
 * Takes a string as label name and returns the line that contains that label declaration
 * returns 0 if label not found
 */
int operate::getLabelLine(string label){
    for(int i=0;i<labelLines.size();i++){
        if(!label.compare(labelLines[i].first)){
            return labelLines[i].second;
        }
    }
    cout<<"label not found";
    return 0;
}


/*
 * Takes a string as a potential varibale name and checks its validity
 */
bool operate::isValidVariable(string s){

    for(int i=0;i<s.size();i++){
        char x=s[i];
        bool val=(x>=48&&x<=57)||(x==95)||(x>=97&&x<=122); //variable names can include letters,numbers or underscore
        if(!val){
            return 0;
        }
        if(i==0&&(x>=48&&x<=57)){  //firs char can not be number
            return 0;
        }
    }
    return 1;

}


/*
 * Takes a string as hexadecimal number input and converts it into decimal with integer value
 */
int operate::hexToDec(string in){

    int sum=0;
    for(int i=in.size()-1;i>=0;i--){
        int j=in.size()-1-i;
        if(in[j]=='a'){
            sum+=10*pow(16,i);
        }
        else if(in[j]=='b'){
            sum+=11*pow(16,i);
        }
        else if(in[j]=='c'){
            sum+=12*pow(16,i);
        }
        else if(in[j]=='d'){
            sum+=13*pow(16,i);
        }
        else if(in[j]=='e'){
            sum+=14*pow(16,i);
        }
        else if(in[j]=='f'){
            sum+=15*pow(16,i);
        }
        else if(in[j]>=48&&in[j]<=57){
            sum+=(((int)in[j])-48)*pow(16,i);
        }
        else{
            return -1;   //means a non hex char exists in input
        }
    }
    return sum;
}



/*
 * Takes a string as binary number input and converts it into decimal with integer value
 * This helps in shift,rotation instructions especially
 */
int operate::binToDec(string in){

    int sum=0;
    for(int i=in.size()-1;i>=0;i--){
        int j=in.size()-1-i;
        if(in[j]=='1'){
            sum+=pow(2,i);
        }
        else if(in[j]=='0'){

        }
        else{
            return -1;   //means non binary character exists
        }
    }
    return sum;
}

/*
 * Takes a decimal integer and returns its string hexadecimal conversion(in 4 digits) (considerbigger inputs)
 */
string operate::decToHex(int in){
    stack<int>s;
    string sum="";
    while(in>0){
        s.push(in%16);
        in/=16;
    }
    for(int i=1;i<=4-s.size();i++) {
        sum.push_back('0');
    }
    while(!s.empty()){
        int num=s.top();
        s.pop();
        if(num<10){
            sum.push_back(num+48);
        }
        else{
            sum.push_back(num+87);
        }
    }
    return sum;
}

/*
 * Takes a decimal integer and returns its string hexadecimal conversion(in 8 digits)
 */
string decToHex32(int in){
    stack<int>s;
    string sum="";
    while(in>0){
        s.push(in%16);
        in/=16;
    }
    for(int i=1;i<=8-s.size();i++) {
        sum.push_back('0');
    }
    while(!s.empty()){
        int num=s.top();
        s.pop();
        if(num<10){
            sum.push_back(num+48);
        }
        else{
            sum.push_back(num+87);
        }
    }
    return sum;

}


/*
 * Takes a decimal integer and returns its string binary conversion
 */
string operate::decToBin(int in){
    stack<int>s;
    string sum="";
    while(in>0){
        s.push(in%2);
        in/=2;
    }
    for(int i=1;i<=16-s.size();i++) {
        sum.push_back('0');
    }
    while(!s.empty()){
        int num=s.top();
        s.pop();
            sum.push_back(num+48);

    }
    return sum;
}


/*
 * When 8 bits subregisters change, their 16 bits registers should be updated
 * Gets an unsigned short pointer which keeps the address of register changed, depending on the input, updates the involved register
 */
void operate::updateAxToAh(unsigned short *ptr) {
    if(ptr==&ah||ptr==&al){
        ax=256*ah+al;
    }
    else if(ptr==&bh||ptr==&bl){
        bx=256*bh+bl;
    }
    else if(ptr==&ch||ptr==&cl){
        cx=256*ch+cl;
    }
    else if(ptr==&dh||ptr==&dl){
        dx=256*dh+dl;
    }
}

/*
 * When 16 bits registers change, their 8 bits subregisters should be updated
 * Gets an unsigned short pointer which keeps the address of register changed, depending on the input, updates the involved registers
 */
void operate::updateAhToAx(unsigned short *ptr) {

    string num=decToHex(*ptr);
    string hpart=num.substr(0,2);  //significant bits
    string lpart=num.substr(2,4); //least significant bits
    short num1=hexToDec(hpart); //(goes to *h registers)
    short num2=hexToDec(lpart); //(goes to *l registers)
    if(ptr==&ax){
        ah=num1;
        al=num2;
    }
    else if(ptr==&bx){
        bh=num1;
        bl=num2;
    }
    else if(ptr==&cx){
        ch=num1;
        cl=num2;
    }
    else if(ptr==&dx){
        dh=num1;
        dl=num2;
    }

}

/*
 * Gets a pointer that  holds an address in memory and returns the index
 * This works for reaching the next index also when we are dealing with word size data
 */
int operate::getMemoryAddress(unsigned short* ptr){
    for(int i=0;i<memory.size();i++){
        if(ptr==&memory[i].second){
            return i;
        }
    }
    cout<<"out of memory";
    return 0;
}

/*
 * Gets an integer as memory index and returns the combined data in that index and the next one
 * ex.. memory[100]->ab memory[101]->12    returns 12abh
 */
unsigned short operate::loadWordSizeData(int index){
    return 256*memory[index+1].second+memory[index].second;
}

/*
 * Gets an unsigned short as value and integer as index
 * Puts the value combined in the index and the next index
 */
void operate::writeWordSizeData(int index, unsigned short data){
    string num=decToHex(data);
    memory[index+1].second=hexToDec(num.substr(0,2));
    memory[index].second=hexToDec(num.substr(2,2));

}


/*MAIN JOBE IS DONE IN THIS FUNCTION
 * In the parse function, program reads the file and divides it into lines and tokens
 * In redirect function, program reads the given line and groups the tokens.
 * Then the groups yields pointers that points to memory addresses or registers by calling arriveptr and departptr methods
 * These pointers are given into instruction cases and necessary instruction functions are called
 * Then the line to be executed next is called by returning its line number
 */
int operate::redirect(int linenum, vector<string> line) {

    lastExecuted=linenum;

    if(!line[1].compare("int")&&!line[2].compare("20h")){  //check exit condition
        return 0;
    }
    if(!line[1].compare("int")&&!line[2].compare("21h")){ //check interrupt

        if(ah==1){
            char x;
            cin>>x;
            al=x;
            return linenum+1;
        }
        else if(ah==2){
            char x=dl;
            cout<<x;
            al=x;
            return linenum+1;
        }

    }



    for(int i=0;i<labelLines.size();i++){    //if line has label, just skip to next line
        if(linenum==labelLines[i].second){
            return linenum+1;
        }
    }

    unsigned short*arrive;  //pointer that will point first part of operand
    unsigned short*depart; //pointer that will point second part of operand
    unsigned short possiblevar=0;  //imidiate data storage
    string instr=line[1];  //first token is instruction command (line[0] is -)

    if(!instr.compare("nop")){  //nop command
        return linenum+1;
    }


    //CHECKING JUMPS!!!!!
    if(!instr.compare("jmp")){
        if (line.size() == 3) {
            return getLabelLine(line[2]);  //the new line to be executed is the line of the given label(line[2])
        }
        cout<<"syntax error";  //we are expecting just a label
        return 0;
    }

    if(!instr.compare("jz")||!instr.compare("je")){
        if (line.size() == 3) {
            return (zf)?getLabelLine(line[2]):linenum+1;   //redirect to line to be jumped,if no label found, 0 will be returned with error message}
        }
        cout<<"syntax error";  //we are expecting just a label
        return 0;
    }
    else if(!instr.compare("jnz")||!instr.compare("jne")){
        if (line.size() == 3) {
            return (!zf)?getLabelLine(line[2]):linenum+1;   //redirect to line to be jumped,if no label found, 0 will be returned with error message}
        }
        cout<<"syntax error";
        return 0;
    }
    else if(!instr.compare("ja")||!instr.compare("jnbe")){
        if (line.size() == 3) {
            return (!zf&&!cf)?getLabelLine(line[2]):linenum+1;   //redirect to line to be jumped,if no label found, 0 will be returned with error message}
        }
        cout<<"syntax error";
        return 0;
    }
    else if(!instr.compare("jae")||!instr.compare("jnb")||!instr.compare("jnc")){
        if (line.size() == 3) {
            return (!cf)?getLabelLine(line[2]):linenum+1;   //redirect to line to be jumped,if no label found, 0 will be returned with error message}
        }
        cout<<"syntax error";
        return 0;
    }
    else if(!instr.compare("jb")||!instr.compare("jnae")||!instr.compare("jc")){
        if (line.size() == 3) {
            return (cf)?getLabelLine(line[2]):linenum+1;   //redirect to line to be jumped,if no label found, 0 will be returned with error message}
        }
        cout<<"syntax error";
        return 0;
    }
    else if(!instr.compare("jbe")){
        if (line.size() == 3) {
            return (cf||zf)?getLabelLine(line[2]):linenum+1;   //redirect to line to be jumped,if no label found, 0 will be returned with error message}
        }
        cout<<"syntax error";
        return 0;
    }


//from now on arrival=first part of operand /departure=second part of operand   ex. mov ax,bx ax->arrival  bx->departure
    string operand=line[2];
    string arrival="";  //keeps the string group showing the first part of operand
    string departure="";  //keeps the string group showing the second part of operand   (does not exist for some instructions)
    bool duoinput=false;  //if line is in format mov ax,bx
    bool a8=false;  //means arrival is 16bit
    bool d8=false; //mean departure is 16 bit   will be used for compatibility of instruction
    bool arf=false; //mean arrival has free bits ex. 13d or [1100]
    bool depf=false; //mean departure has free bits
    bool arMemoryAccess=false; //mean memory will be accessed by arrival   ex. arrival is variable or memory address
    bool depMemoryAccess= false;    //mean memory will be accessed by departure
    bool aroffset=false; //keep the offset option since we will behave differently to offset var and just var
    bool depoffset=false;

    for(int i=1;i<line.size();i++){   //if input has two parts in operand, it must contain ','
        string cur=line[i];
        if(cur.find(",")<cur.size()){
            duoinput=true;
        }
    }

    if(duoinput) {  //grouping the tokens to give valid inputs to arriveptr and departptr functions

        if(line.size() == 7){   //mov w var , w var2   -> arrival= w var departure = w var2
            if(!line[4].compare(",")){
                arrival=arrival+line[2];
                arrival.push_back(' ');
                arrival=arrival+line[3];
                if(!line[5].compare("offset")){
                    depoffset=true;
                    departure=line[6];
                }
                else {
                    departure = departure + line[5];
                    departure.push_back(' ');
                    departure = departure + line[6];
                }
            }
        }

        else if (line.size() == 6) {
            if (!line[4].compare("offset") && !line[3].compare(",")) {
                depoffset = true;
                arrival = line[2];
                departure = line[5];    //departure is variable name but it will be redirected with an offset flag!!!
            }
            else if(!line[3].compare(",")){  //mov al , w var
                arrival=line[2];
                if(!line[4].compare("offset")){
                    depoffset=true;
                    departure=line[5];
                }
                else {
                    departure = departure + line[4];
                    departure.push_back(' ');
                    departure = departure + line[5];
                }
            }
            else if(!line[4].compare(",")){  //mov w var , var2
                arrival=arrival+line[2];
                arrival.push_back(' ');
                arrival=arrival+line[3];
                departure=line[5];
            }
            else if(line[3][line[3].size()-1]==','){  //mov w var, w var2
                arrival=arrival+line[2];
                arrival.push_back(' ');
                arrival=arrival+line[3].substr(0,line[3].size()-1);
                if(!line[4].compare("offset")){
                    depoffset=true;
                    departure=line[5];
                }
                else {
                    departure = departure + line[4];
                    departure.push_back(' ');
                    departure = departure + line[5];
                }
            }
            else if(line[4][0]==','){  //mov w var ,w var2
                arrival=arrival+line[2];
                arrival.push_back(' ');
                arrival=arrival+line[3];
                if(!line[4].substr(1).compare("offset")){
                    depoffset=true;
                    departure=line[5];
                }
                else {
                    departure.push_back(line[4][1]);
                    departure.push_back(' ');
                    departure = departure + line[5];
                }
            }

        }
        else if (line.size() == 5) {

            if (!line[3].compare("offset") || !line[3].compare(",offset")) {  //mov al, offset var  or mov al ,offset var
                depoffset = true;
                arrival = (!line[3].compare("offset")) ? line[2].substr(0, line[2].size() - 1) : line[2];
                departure = line[4];
            }
            else if (!line[3].compare(",")) { //mov al , var
                arrival = line[2];
                departure = line[4];
            }
            else if(line[2][line[2].size()-1]==','){  //mov al, w var
                arrival=line[2].substr(0,line[2].size()-1);
                departure.push_back(line[3][0]);
                departure.push_back(' ');
                departure=departure+line[4];
            }
            else if(line[3][0]==','){  //mov al ,w var
                arrival=line[2];
                departure.push_back(line[3][1]);
                departure.push_back(' ');
                departure=departure+line[4];
            }
            else if(line[3][line[3].size()-1]==','){  //mov w var, var2
                arrival=arrival+line[2];
                arrival.push_back(' ');
                arrival=arrival+line[3].substr(0,line[3].size()-1);
                departure=departure+line[4];
            }
            else if(line[4][0]==',') {  //mov w var ,var2
                arrival = arrival + line[2];
                arrival.push_back(' ');
                arrival = arrival + line[3];
                departure = line[4].substr(1);
            }
            else if(line[3].find(',')>0&&line[3].find(',')<line[3].size()){  //mov w var,w var2
                int x=line[3].find(',');
                arrival=arrival+line[2];
                arrival.push_back(' ');
                arrival=arrival+line[3].substr(0,x);
                departure=departure+line[3].substr(x+1);
                departure.push_back(' ');
                departure=departure+line[4];
            }

        }
        else if (line.size() == 4) {   //, must be together with arrival or departure

            if (line[2].find("offset") > 0 && line[2].find("offset") <
                                              line[2].size()) {  //line[2] is not offset itself but contains ((we will use offset itself case in unary ops)
                for (int i = 1; i < line[2].size(); i++) {
                    if (line[2][i] == ',') {
                        depoffset = true;
                        arrival = line[2].substr(0, i);
                        departure = line[3];
                    }
                }
            }
            else {

                if(line[2][line[2].size()-2]==','){  //mov al,w var  or mov al,w [1234h] conditions
                    departure.push_back(line[2][line[2].size()-1]);
                    departure.push_back(' ');
                    departure=departure+line[3];
                    arrival=line[2].substr(0,line[2].size()-2);
                }
                else if(line[3].find(',')>0&&line[3].find(',')<line[3].size()){  //mov w var,var2
                    int x=line[3].find(',');
                    arrival=arrival+line[2];
                    arrival.push_back(' ');
                    arrival=arrival+line[3].substr(0,x);
                    departure=line[3].substr(x+1);
                }
                else {  //mov al, var or mov al ,var condditions
                    arrival = line[2];
                    departure = line[3];
                    if (arrival[arrival.size() - 1] == ',') {
                        arrival = arrival.substr(0, arrival.size() - 1);

                    }
                    else if (departure[0] == ',') {
                        departure = departure.substr(1, departure.size());
                    }
                    else {
                        cout << "syntax error";
                        return 0;
                    }
                }
            }
        }
        else if (line.size() == 3) {

            for (int i = 1; i <operand.size(); i++) {    //if operand has both arrival and departure stick together with , in a binary operation.
                if (operand[i] == ',') {
                    arrival = operand.substr(0, i);
                    departure = operand.substr(i + 1, operand.size());
                    duoinput = true;
                    break;
                }
            }
        }

    }

    else{    //then operand has a single part

        if(line.size()==4) {
            if (!line[2].compare("offset")){
                aroffset=1;
                arrival=line[3];
            }
            else{// div w [1100]
                arrival=line[2];
                arrival.push_back(' ');
                arrival+=line[3];
            }
        }
        else if(line.size()==3){
            arrival=line[2];
        }

    }

/*
 * arrive is a pointer to target address or register
 * also the information like size of data, memory access details are updated in function
 */

    arrive=arriveptr(arrival,a8,arf,possiblevar,arMemoryAccess,aroffset);
    if(duoinput) {
        depart = departptr(departure, a8, d8, depf, possiblevar,depMemoryAccess,depoffset);
    }

    if(arrive== nullptr){  //if the target can not be acquired then return 0(stop execution)
        return 0;
    }



    if(!instr.compare("mov")){  //mov instruction is executed after compatibility tests are done

        if(depart== nullptr){
            return 0;
        }

        if((a8==d8)||(a8&depf)||(d8&&arf)|| (arf||depf)){ //ex.  mov ax,bx || mov ax,12 || mov [1000],bx
            if(arf&&depf){ //ex. mov [1000],12
                cout<<"byte or word?";
                return 0;
            }
            return mov(arrive,depart,linenum,a8||d8,arMemoryAccess,depMemoryAccess);  //call the instruction function
        }
        cout<<"incompatible";
        return 0;
    }
    else if(!instr.compare("add")){ //same procedure with mov

        if(depart== nullptr){
            return 0;
        }

        if((a8==d8)||(a8&depf)||(d8&&arf)|| (arf||depf)){
            if(arf&&depf){
                cout<<"byte or word?";
                return 0;
            }
            return add(arrive,depart,linenum,a8||d8,arMemoryAccess,depMemoryAccess);
        }
        cout<<"incompatible";
        return 0;

    }
    else if(!instr.compare("sub")||!instr.compare("cmp")){

        if(depart== nullptr){
            return 0;
        }

        if((a8==d8)||(a8&depf)||(d8&&arf)|| (arf||depf)){
            if(arf&&depf){
                cout<<"byte or word?";
                return 0;
            }
            if(!instr.compare("sub")) {
                return sub(arrive, depart, linenum, a8 || d8, arMemoryAccess, depMemoryAccess);
            }
            else{
                if(!arMemoryAccess && arrive!=&ax&&arrive!=&bx&&arrive!=&cx&&arrive!=&ah&&arrive!=&bh&&arrive!=&ch&&arrive!=&al&&arrive!=&bl&&arrive!=&cl&&
                        arrive!=&dx&&arrive!=&dh&&arrive!=&dl&&arrive!=&bp&&arrive!=&sp&&arrive!=&si&&arrive!=&di){
                    cout<<"first operand can not be immidiate data"; //testing if pointer points to a register or memory address
                    return 0;
                }
                return cmp(arrive,depart,linenum,a8||d8,arMemoryAccess,depMemoryAccess);
            }
        }
        cout<<"incompatible";
        return 0;
    }

    else if(!instr.compare("mul")){
        if(!arMemoryAccess && arrive!=&ax&&arrive!=&bx&&arrive!=&cx&&arrive!=&ah&&arrive!=&bh&&arrive!=&ch&&arrive!=&al&&arrive!=&bl&&arrive!=&cl&&
           arrive!=&dx&&arrive!=&dh&&arrive!=&dl&&arrive!=&bp&&arrive!=&sp&&arrive!=&si&&arrive!=&di){
            cout<<"operand can not be immidiate data";
            return 0;
        }
        if(arf){   // mul [1000]
            cout<<"byte or word?";
            return 0;
        }
        return mul(arrive,linenum,a8,arMemoryAccess);
    }
    else if(!instr.compare("div")){
        if(!arMemoryAccess && arrive!=&ax&&arrive!=&bx&&arrive!=&cx&&arrive!=&ah&&arrive!=&bh&&arrive!=&ch&&arrive!=&al&&arrive!=&bl&&arrive!=&cl&&
           arrive!=&dx&&arrive!=&dh&&arrive!=&dl&&arrive!=&bp&&arrive!=&sp&&arrive!=&si&&arrive!=&di){
            cout<<"operand can not be immidiate data";
            return 0;
        }
        if(arf){
            cout<<"byte or word?";
            return 0;
        }
        return div(arrive,linenum,a8,arMemoryAccess);
    }

    else if(!instr.compare("not")){
        if(!arMemoryAccess && arrive!=&ax&&arrive!=&bx&&arrive!=&cx&&arrive!=&ah&&arrive!=&bh&&arrive!=&ch&&arrive!=&al&&arrive!=&bl&&arrive!=&cl&&
           arrive!=&dx&&arrive!=&dh&&arrive!=&dl&&arrive!=&bp&&arrive!=&sp&&arrive!=&si&&arrive!=&di){
            cout<<"operand can not be immidiate data";
            return 0;
        }
        if(arf){
            cout<<"byte or word?";
            return 0;
        }
        return notfunc(arrive,linenum,a8,arMemoryAccess);
    }


    else if(!instr.compare("and")||!instr.compare("or")||!instr.compare("xor")){

        if(depart== nullptr){
            cout<<"error";
            return 0;
        }

        if((a8==d8)||(a8&depf)||(d8&&arf)|| (arf||depf)){
            if(arf&&depf){
                cout<<"byte or word?";
                return 0;
            }
            //implemented and , or , xor in same function thats why options(last parameter) determine the operation to be made
            else if(!instr.compare("and")){
                return andorxorfunc(arrive,depart,linenum,a8||d8,arMemoryAccess,depMemoryAccess,1);
            }
            else if(!instr.compare("or")){
                return andorxorfunc(arrive,depart,linenum,a8||d8,arMemoryAccess,depMemoryAccess,2);
            }
            else if(!instr.compare("xor")){
                return andorxorfunc(arrive,depart,linenum,a8||d8,arMemoryAccess,depMemoryAccess,3);
            }
        }
        cout<<"incompatible";
        return 0;


    }

    else if(!instr.compare("rcl")||!instr.compare("rcr")||!instr.compare("shr")||!instr.compare("shl")){

        if(depart==nullptr){
            return 0;
        }
        if(*depart>=32){  //bad operand
            return 0;
        }
        bool validdep=false;  //instruction only works with cl or immidiate data as departure
        if(depart==&cl){
            validdep=true;
        }
        if(is_number(departure) ||
                (departure[departure.size()-1]=='d'&&is_number(departure.substr(0,departure.size()-1))) ||
                (departure[departure.size()-1]=='h'&&hexToDec(departure.substr(0,departure.size()-1))!=-1)){  //departure is immidiate data
            validdep=true;
        }
        if(!validdep){
            cout<<"bad operand";
            return 0;
        }

        if(!instr.compare("rcr")){
            return rcrrclshrshl(arrive,depart,linenum,a8,arMemoryAccess,1);
        }
        else if(!instr.compare("rcl")){
            return rcrrclshrshl(arrive,depart,linenum,a8,arMemoryAccess,2);
        }
        else if(!instr.compare("shr")){
            return rcrrclshrshl(arrive,depart,linenum,a8,arMemoryAccess,3);
        }
        else{
            return rcrrclshrshl(arrive,depart,linenum,a8,arMemoryAccess,4);
        }
    }



    else if(!instr.compare("inc")){
        if(arf){
            cout<<"byte or word";
            return 0;
        }
        bool temp=cf;
        unsigned short a=1;
        depart=&a;
        int res=add(arrive,depart,linenum,a8,arMemoryAccess,0);  //just like add but we add 1
        cf=temp;   //in addition, cf is affected but we need to keep cf same in inc instruction
        return res;

    }
    else if(!instr.compare("dec")){
        if(arf){
            cout<<"byte or word";
            return 0;
        }
        bool temp=cf;
        unsigned short a=1;
        depart=&a;
        int res=sub(arrive,depart,linenum,a8,arMemoryAccess,0);  //just like sub but we sub 1
        cf=temp;   //inc keeps cf
        return res;

    }
    else if(!instr.compare("push")||!instr.compare("pop")){

        //works with word size operands ,if variable is called without w or b, validity depends on initialization of variable(dw)

        if(!a8&&!arf){    //can not get operand in byte size
            cout<<"incompatible";
            return 0;
        }
        if(!instr.compare("push")){
            return pushpop(arrive,linenum,arMemoryAccess,1);
        }
        else{
            if(!arMemoryAccess&&arf){
                cout<<"incompatible";
                return 0;
            }
            return pushpop(arrive,linenum,arMemoryAccess,2);
        }
    }
    else{
        cout<<"syntax error";
        return 0;
    }



}



/*
 * INSTRUCTION FUNCTIONS
 */



/*MOV INSTRUCTION
 * Gets pointers to departure and arrival, depending on their sizes, updates the arrival value
 * Does not affect flags
 * Returns linenum+1 since there is no jump
 */
int operate::mov(unsigned short*ar,unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess) {

    if(bytes2){
        int num;
        if(depmemoryaccess){
            num=loadWordSizeData(getMemoryAddress(dep));  //gets the memory index of departure and combines the data of 2 indexes
        }
        else{
            num=*dep;  //just gets the data in pointed address
        }
        if(armemoryaccess){
            writeWordSizeData(getMemoryAddress(ar),num);  //updates the arrival index and the next index with num
        }
        else{
            *ar=num;
            if(ar==&ax||ar==&bx||ar==&cx||ar==&dx){
                updateAhToAx(ar);
            }

        }
    }
   else if(ar==&ah||ar==&al||ar==&bh||ar==&bl||ar==&ch||ar==&cl||ar==&dh||ar==&dl){
        *ar=*dep;
        updateAxToAh(ar);
    }
    else{    //variable conditions
        *ar=*dep;
    }

    return linenum+1;
}


/*ADD INSTRUCTION
 * Gets pointers to departure and arrival, depending on their sizes, updates the arrival value
 * Returns linenum+1 since there is no jump
 */
int operate::add(unsigned short*ar,unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess){  //memory de testler yap

    int add1;
    int add2;


    if(armemoryaccess&&bytes2){    //gets the memory index of arrival and combines the data of 2 indexes
        add1=loadWordSizeData(getMemoryAddress(ar));
    }
    else{
        add1=*ar;
    }
    if(depmemoryaccess&&bytes2){  //gets the memory index of departure and combines the data of 2 indexes
        add2=loadWordSizeData(getMemoryAddress(dep));
    }
    else{
        add2=*dep;
    }


    int num=add1+add2;   //the result
    string nums=decToHex(num);

    if(bytes2){    //if arrival is 2 bytes

        if(nums.size()>4) {  //do not get the most significant bit
            nums = nums.substr(1, 5);
            cf=1;    //carry flag set
        }
        else{
            cf=0;
        }
        short fin=hexToDec(nums);
        zf=(fin==0); //zero flag set
        of=ofSet(add1,add2,fin,1,1);  //overflow flag set
        sf=sfSet(fin,1);   //sign flag set
        char fir=decToHex(add1)[3];  //auxiliary flag is determined by least significant digit in hex
        char sec=decToHex(add2)[3];
        char res=nums[3];
        af= res<fir&&res<sec;  //auxiliary flag set

        if(armemoryaccess){
            writeWordSizeData(getMemoryAddress(ar),fin);    //write the final result 2 two indexes combined
        }
        else {
            *ar = fin;
            updateAhToAx(ar);
        }
    }
    else {
        cf=(nums[1]!='0');  //carry flag set
        nums=nums.substr(2,4);
        short fin=hexToDec(nums);
        zf==(fin==0);  //zero flag set
        of=ofSet(add1,add2,fin,0,1);  //overflow flag set
        sf=sfSet(fin,0);   //sign flag set
        char fir=decToHex(add1)[3];
        char sec=decToHex(add2)[3];
        char res=nums[1];
        af= res<fir&&res<sec;  //auxiliary flag set
        *ar=fin;
        updateAxToAh(ar);
    }

    return linenum+1;
}


/*SUB INSTRUCTION
 * Gets pointers to departure and arrival, depending on their sizes, updates the arrival value
 * Returns linenum+1 since there is no jump
 */
int operate::sub(unsigned short*ar,unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess){

    int num1;
    int num2;


    if(armemoryaccess&&bytes2){
        num1=loadWordSizeData(getMemoryAddress(ar));
    }
    else{
        num1=*ar;
    }
    if(depmemoryaccess&&bytes2){
        num2=loadWordSizeData(getMemoryAddress(dep));
    }
    else{
        num2=*dep;
    }

    char fir=decToHex(num1)[3];
    char sec=decToHex(num2)[3];
    af=(sec>fir);

    if(bytes2){
        unsigned short unsnum=num1-num2;
        cf=(num1<num2);
        of=ofSet(num1,num2,unsnum,1,2);
        sf=sfSet(unsnum,1);
        zf=(unsnum==0);

        if(armemoryaccess){
            writeWordSizeData(getMemoryAddress(ar),unsnum);
        }
        else {
            *ar = *ar - *dep;
            updateAhToAx(ar);
        }

    }
    else{
        short num=*ar-*dep;
        int unsnum=(num<0)?256+num:num;  //unsigned 8 bit
        of=ofSet(num1,num2,unsnum,0,2);
        sf=sfSet(unsnum,0);
        zf=(unsnum==0);
        if(num<0){
            cf=1;
            *ar=256+num;
        }
        else{
            cf=0;
            *ar=num;
        }
        updateAxToAh(ar);
    }
    return linenum+1;
}


/*MUL INSTRUCTION
 * gets the pointer to arrival(in this case, the secon operand of multiplication)
 * updates ax or dx:ax
 * returns linenum+1 since there is no jump
 */
int operate::mul(unsigned short* ar,int linenum,bool bytes2,bool armemoryaccess){

    int num=(bytes2&&armemoryaccess)?loadWordSizeData(getMemoryAddress(ar)):*ar;
    if(bytes2){
        int result=ax*num;  //to be put in dx:ax
        string reshex=decToHex32(result); //need 8 digit hex representation to divide into dx and ax
        if(reshex.size()>4){
            string toax=reshex.substr(reshex.size()-4,4);  //last 4 digits of hex representation goes to ax
            string todx=reshex.substr(0,reshex.size()-4);
            ax=hexToDec(toax);
            dx=hexToDec(todx);
            updateAhToAx(&ax);
            updateAhToAx(&dx);
            cf=1,of=1;  //upper half is not 0
        }
        else{
            dx=0;
            updateAhToAx(&dx);
            ax=hexToDec(reshex);
            updateAhToAx(&ax);
            cf=0,of=0;
        }
    }
    else{
        int result=al*num;
        ax=result;
        updateAhToAx(&ax);
        of=(result>=256);
        cf=of;
    }

    sf=0,zf=0,af=0;   //since undefined, made them 0
    return linenum+1;

}

/*DIV INSTRUCTION
 * gets the pointer to arrival(in this case, the divisor)
 * updates ax,dx or ah,al
 * returns linenum+1 since there is no jump
 */
int operate::div(unsigned short* ar,int linenum,bool bytes2,bool armemoryaccess){

    int num=(bytes2&&armemoryaccess)?loadWordSizeData(getMemoryAddress(ar)):*ar;
    if(bytes2){
        int big=65536*dx+ax;  //get dx:ax
        int quotient=big/num;
        int rem=big%num;
        if(quotient>=65536){
            cout<<"divide overflow";
            return 0;
        }
        ax=quotient;
        dx=rem;
        updateAhToAx(&ax);
        updateAhToAx(&dx);
    }
    else{
        int quotient=ax/num;
        int rem=ax%num;
        if(quotient>=256){
            cout<<"divide overflow";
            return 0;
        }
        ah=rem;
        al=quotient;
        updateAxToAh(&ah);
    }

    cf=0, of=0, sf=0,zf=0,af=0;   //since undefined, made them 0
    return linenum+1;

}


/*NOT INSTRUCTION
 * gets pointer to arrival and updates it to its ones complement value
 * returns linenum+1 since there is no jump
 */
int operate::notfunc(unsigned short *ar,int linenum,bool bytes2,bool armemoryaccess){

    int num=(armemoryaccess&&bytes2)?loadWordSizeData(getMemoryAddress(ar)):*ar;
    int result=(bytes2)?65535-num:255-num;  //not = 1's complement
    if(armemoryaccess&&bytes2){
        writeWordSizeData(getMemoryAddress(ar),result);
    }
    else{
        *ar=result;
    }
    if(bytes2){
        updateAhToAx(ar);
    }
    else{
        updateAxToAh(ar);
    }
    return linenum+1;

}

/*AND OR XOR INSTRUCTION
 * gets pointers to arrival and departure, updates arrival depending on the option
 * option defines the operation and=1 or=2 xor=3
 * returns linenum+1 since there is no jump
 */
int operate::andorxorfunc(unsigned short *ar, unsigned short *dep, int linenum, bool bytes2,bool armemoryaccess,bool depmemoryaccess,int option) {

    int num;
    if(bytes2&&depmemoryaccess){
        num=loadWordSizeData(getMemoryAddress(dep));
    }
    else{
        num=*dep;
    }

    int result;

    if(option==1) {   //and instr
        if(armemoryaccess&&bytes2){
            int num1=loadWordSizeData(getMemoryAddress(ar));
            result=num1&num;
            writeWordSizeData(getMemoryAddress(ar),result);
        }
        else {
            *ar = *ar & num;
            result=*ar;
        }
    }
    else if(option==2){ //or
        if(armemoryaccess&&bytes2){
            int num1=loadWordSizeData(getMemoryAddress(ar));
            result=num1|num;
            writeWordSizeData(getMemoryAddress(ar),result);
        }
        else {
            *ar = *ar | num;
            result=*ar | num;
        }
    }
    else{
        if(armemoryaccess&&bytes2){
            int num1=loadWordSizeData(getMemoryAddress(ar));
            result= num1^num;
            writeWordSizeData(getMemoryAddress(ar),result);
        }
        else {
            *ar = *ar ^ num;
            result=*ar;
        }
    }

    zf=(result==0);
    sf=(bytes2)?(result>=32768):(result>=128);

    if(bytes2){
        updateAhToAx(ar);
    }
    else{
        updateAxToAh(ar);
    }
    af=0,of=0,cf=0;     //af,of,cf are cleared!!!


    return linenum+1;
}


/*RCR RCL SHR SHL INSTRUCTION
 * gets pointers to arrival and departure, updates arrival depending on the option
 * option defines the operation rcr=1 rcl=2 shr=3 shl=4
 * returns linenum+1 since there is no jump
 */
int operate::rcrrclshrshl(unsigned short*ar, unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,int option){

    int count=*dep;  //times of rotation
    unsigned short num=(bytes2&&armemoryaccess)?loadWordSizeData(getMemoryAddress(ar)):*ar;
    for(int i=0;i<count;i++){  //each iteration is one rotation
        string numbin=decToBin(num);
        if(option==1){   //rcr
            string res="";
            res.push_back(cf+48);
            res=(bytes2)?res+numbin:res+numbin.substr(8,numbin.size()); //concatinate cf to value
            cf=res[res.size()-1]-48;   //last digit goes to carry
            if(count==1) {  //just 1 bit rotates
                of = (res[0]-48) ^ (res[1]-48);   //xor of two most significant digits
            }
            //zf,af,sf are unaffected!!!!

            res=res.substr(0,res.size()-1);
            num=binToDec(res);
        }
        else if(option==2){  //rcl
            numbin.push_back(cf+48);
            cf=(bytes2)?numbin[0]-48:numbin[8]-48;
            numbin=(bytes2)?numbin.substr(1,numbin.size()):numbin.substr(9,numbin.size());
            if(count==1) {
                of = (numbin[0]-48) ^ cf;  //xor of most significant digit and cf
            }
            //zf,af,sf are unaffected!!!!
            num=binToDec(numbin);
        }
        else if(option==3){  //shr
            string numbin=decToBin(num);
            cf=numbin[numbin.size()-1]-48;
            num=num>>1;
        }
        else{  //shl
            string numbin = decToBin(num);
            if(bytes2) {
                cf = numbin[0] - 48;
                num = num << 1;  //unsigned short already limits the number
            }
            else{
                cf=numbin[8]-48;
                numbin=numbin.substr(9);   //dectobin returns 16 bit number
                numbin.push_back('0');
                num=binToDec(numbin);
            }
        }
    }
    if(option==3||option==4){   //handle flags
        int first=(bytes2&&armemoryaccess)?loadWordSizeData(getMemoryAddress(ar)):*ar;  //first condition of arrival
        string firstbin=decToBin(first);
        int result=num;  //final condition
        string resbin=decToBin(result);
        bool firstpos=(bytes2)?(firstbin[0]=='0'):(firstbin[8]=='0');  //if given operand were positive
        bool respos=(bytes2)?(resbin[0]=='0'):(resbin[8]=='0');  //if result is positive
        if(count==1) {
            of = (firstpos != respos); //sign change
        }
        else{
            //of is undefined,no change
        }
        sf=!respos;
        zf=(result==0);
        af=0; //undefined then make it zero
        //cf has already been arranged
    }
    if(bytes2&&armemoryaccess){
        writeWordSizeData(getMemoryAddress(ar),num);
    }
    else{
        *ar=num;
    }
    if(bytes2){
        updateAhToAx(ar);
    }
    else{
        updateAxToAh(ar);
    }
    return linenum+1;

}


/*CMP INSTRUCTION
 *  same as sub but no value change just updates flags
 */
int operate::cmp(unsigned short*ar, unsigned short*dep,int linenum,bool bytes2,bool armemoryaccess,bool depmemoryaccess){

    int num1;
    int num2;
    if(armemoryaccess&&bytes2){   //if memory is accessed with word size, get total of 2 indexes
        num1=loadWordSizeData(getMemoryAddress(ar));
    }
    else{
        num1=*ar;
    }
    if(depmemoryaccess&&bytes2){
        num2=loadWordSizeData(getMemoryAddress(dep));
    }
    else{
        num2=*dep;
    }
    char fir=decToHex(num1)[3];
    char sec=decToHex(num2)[3];
    af=(sec>fir);

    if(bytes2){
        unsigned short unsnum=num1-num2;
        cf=(num1<num2);
        of=ofSet(num1,num2,unsnum,1,2);
        sf=sfSet(unsnum,1);
        zf=(unsnum==0);
    }
    else{
        short num=*ar-*dep;
        int unsnum=(num<0)?256+num:num;
        of=ofSet(num1,num2,unsnum,0,2);
        sf=sfSet(unsnum,0);
        zf=(unsnum==0);
        cf=(num<0);

    }
    return linenum+1;

}


/*PUSH POP INSSTRUCTION
 * gets arrival pointer which means the operand to be written in case of push, and the operand where top of the stack be written in in case of pop
 * options: push=1 pop=2
 */
int operate::pushpop(unsigned short*ar,int linenum,bool armemoryaccess, int option){

    if(option==1){   //push
        int num=(armemoryaccess)?loadWordSizeData(getMemoryAddress(ar)):*ar;
        writeWordSizeData(sp,num);
        sp-=2; //stack works word size and backwards in memory
    }
    else{
        sp+=2;
        int num=loadWordSizeData(sp);
        if(armemoryaccess){
            writeWordSizeData(getMemoryAddress(ar),num);
        }
        else{
            *ar=num;
        }
    }
    updateAhToAx(ar);


    return linenum+1;
}

/*
 * FLAG UPDATE FUNCTIONS
 */


/*
 * Used in add and sub instructions
 * Gets values of first and second operands and result of operation.
 * According to those, sets overflow flag
 * optype: add=1 sub=2
 */
bool operate::ofSet(unsigned short first, unsigned short second, unsigned  short result,bool bytes2,int optype){  //NOT IN SUBTRACTION!!!

    string fir=decToBin(first);
    string sec=decToBin(second);
    string res=decToBin(result);
    char sig1=(bytes2)?fir[0]:fir[8];
    char sig2=(bytes2)?sec[0]:sec[8];
    char sig3=(bytes2)?res[0]:res[8];

    if(optype==1){//addition
        if(sig1==sig2&&sig2!=sig3){
            return 1;
        }
        else return 0;
    }
    else if(optype==2){ //subtraction
        if(sig1=='1'&&sig2=='0'&&sig3=='0'){
            return 1;
        }
        else if(sig1=='0'&&sig2=='1'&&sig3=='1'){
            return 1;
        }
        else{
            return 0;
        }
    }
    return 0;

}



bool operate::sfSet(unsigned short end,bool bytes2){  //sets sign flag

    if(bytes2){
        return end>=32768;
    }
    else{
        return end>=128;
    }

}

/*
 * HELPER FUNCTIONS
 */



/*
 * Gets a string representing the arrival group of tokens, boolean references of size, unsgigned short data, memoryaccess and offset
 * a8,f8,possiblevar,memoryaccess are given to be updated
 * offset is given to pass information of offset condition
 * a8=1 if arrival represents word size data or memory or register
 * f8=1 if arrival does not have a fixed size
 * possiblevar will keep if an immidiate data is given as arrival
 * memoryaccess=1 if arrival represents an index in memory (variable or memory addressing)
 * offset=1 shows offset condition should be applied
 * function returns the appropriate pointer for given token group
 */
unsigned short *operate::arriveptr(string& arrival, bool& a8, bool& f8, unsigned short &possiblevar,bool& memoryaccess,bool&offset) {

    unsigned short* arrive;

    if(offset){
        for(int i=accessPermit;i<memory.size();i++){
            if(!memory[i].first.compare(arrival)){  //arrival string should be a variable name
                possiblevar=i;  //return the index holding that variable
                if(i>=256){
                    a8=1;
                }
                else{
                    f8=1;  //can be word size or byte size
                }
                return &possiblevar;
            }
        }
        cout<<"error";  //in case of no such variable found
        return nullptr;
    }


    bool free=1;    //if arrival does not include w or b
    if(arrival[1]==' '){   //ex. w var/ b [1000]
        if(arrival[0]=='w'){
            a8=1;
        }
        else if(arrival[0]=='b'){
            a8=0;
        }
        else{
            cout<<"syntax error";
            return nullptr;
        }
        arrival=arrival.substr(2);  //we got the size data so remove w and b parts
        free=0;  //means arrival is defined with size holder (w,b)
    }

    for(int i=0;i<memory.size();i++){   //check if arrival is variable name

        if(!memory[i].first.compare(arrival)){
            if(free) { //if not declared with w or b, we will infer its definition data(dw or db)
                a8 = (memory[i + 1].first.size() == 0 && memorycounter > i + 1);   //dw if i+1th memory has no other variable and memory counter is not just one more of i
            }
            memoryaccess=1;
            return &memory[i].second;
        }
    }


    //CHECK IF ARRIVAL IS DIRECTLY A REGISTER
    if(!arrival.compare("ax")){
        a8=1;
        return &ax;
    }
    else if(!arrival.compare("bx")){
        a8=1;
        return &bx;
    }
    else if(!arrival.compare("cx")){
        a8=1;
        return &cx;
    }
    else if(!arrival.compare("dx")){
        a8=1;
        return &dx;
    }
    else if(!arrival.compare("ah")){
        return &ah;
    }
    else if(!arrival.compare("al")){
        return &al;
    }
    else if(!arrival.compare("bh")){
        return &bh;
    }
    else if(!arrival.compare("bl")){
        return &bl;
    }
    else if(!arrival.compare("ch")){
        return &ch;
    }
    else if(!arrival.compare("cl")){
        return &cl;
    }
    else if(!arrival.compare("dh")){
        return &dh;
    }
    else if(!arrival.compare("dl")){
        return &dl;
    }
    else if(!arrival.compare("di")){
        a8=1;
        return &di;
    }
    else if(!arrival.compare("sp")){
        a8=1;
        return &sp;
    }
    else if(!arrival.compare("si")){
        a8=1;
        return &si;
    }
    else if(!arrival.compare("bp")){
        a8=1;
        return &bp;
    }
    else{
        if((arrival[0]=='['||arrival[1]=='[')&&arrival[arrival.size()-1]==']'){  //[] definitions ex. mov ax,w[1000]
            memoryaccess=true;  //brackets mean memory access
            if(arrival[0]=='[') {
                arrival = arrival.substr(1, arrival.size() - 2);  //examine between brackets
                f8=free;
            }
            else{
                if(arrival[0]=='w'){
                    arrival = arrival.substr(2, arrival.size() - 3);
                    a8=1;

                }
                else if(arrival[0]=='b'){
                    arrival = arrival.substr(2, arrival.size() - 3);
                }
            }
            for(int i=0;i<memory.size();i++){   //variable check in []
                if(!memory[i].first.compare(arrival)){
                    memoryaccess=1;
                    bool w=(memory[i+1].first.size()==0&&memorycounter>i+1);   //if variable is dw
                    int num=(w)?loadWordSizeData(i):memory[i].second; //if dw, get word size data else byte size
                    if(num<accessPermit){   //do not allow lower addresses than accesspermit
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    return &memory[num].second;  //pointer to memory index
                }
            }

            if(!arrival.compare("ax")||!arrival.compare("cx")||!arrival.compare("dx")||!arrival.compare("ah")
            ||!arrival.compare("al")||!arrival.compare("bh")||!arrival.compare("bl")||!arrival.compare("ch")||!arrival.compare("cl")||!arrival.compare("dh")
            ||!arrival.compare("dl")||!arrival.compare("sp")){   //bad register
                cout<<"error";
                return nullptr;
            }
            else if(!arrival.compare("bx")){
                arrive=&memory[bx].second;
                if(bx<accessPermit){
                   cout<<"access not allowed";
                   return nullptr;
                }
            }


            else if(!arrival.compare("di")){
                arrive=&memory[di].second;
                if(di<accessPermit){
                    cout<<"access not allowed";
                    return nullptr;
                }
            }

            else if(!arrival.compare("si")){
                arrive=&memory[si].second;
                if(si<accessPermit){
                    cout<<"access not allowed";
                    return nullptr;
                }
            }
            else if(!arrival.compare("bp")){
                arrive=&memory[bp].second;
                if(bp<accessPermit){
                    cout<<"access not allowed";
                    return nullptr;
                }
            }
            else{       //  then immediate data must be given in brackets
                if(arrival[arrival.size()-1]=='h') {//hexadecimal memory address
                    arrival=arrival.substr(0,arrival.size()-1);  //exclude h
                    if(arrival[0]>57||arrival[0]<48){ //first char in hex must be a number digit (abch ->0abch)
                        cout<<"syntax error";
                        return nullptr;
                    }
                    int mem=hexToDec(arrival);
                    if(mem==-1){  //means string is not a hex number or a very big number
                        cout<<"syntax error";
                        return nullptr;
                    }
                    if(mem>65535){  //memory size is limited
                        cout<<"error";
                        return nullptr;
                    }
                    if(mem<accessPermit){
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    arrive=&memory[mem].second;
                }
                else if(arrival[arrival.size()-1]=='d'){
                    arrival=arrival.substr(0,arrival.size()-1); //exclude d
                    if(!is_number(arrival)){ //means string is not a number
                        cout<<"syntax error";
                        return nullptr;
                    }
                    int mem=stoi(arrival);
                    if(mem>65535){
                        cout<<"error";
                        return nullptr;
                    }
                    if(mem<accessPermit){
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    arrive=&memory[mem].second;
                }
                else{
                    if(!is_number(arrival)){
                        cout<<"syntax error";
                        return nullptr;
                    }
                    int mem=stoi(arrival);
                    if(mem>65535){
                        cout<<"error";
                        return nullptr;
                    }
                    if(mem<accessPermit){
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    arrive=&memory[mem].second;
                }
            }
        }
        else{    //immidiate data without brackets
            if(arrival[arrival.size()-1]=='h') {//hexadecimal memory address
                arrival=arrival.substr(0,arrival.size()-1);
                if(arrival[0]>57||arrival[0]<48){
                    cout<<"syntax error";
                    return nullptr;
                }
                int mem=hexToDec(arrival);
                if(mem==-1){  //means string is not a hex number
                    cout<<"syntax error";
                    return nullptr;
                }
                if(mem>65535){
                    cout<<"error";
                }
                possiblevar=mem;
                arrive=&possiblevar; //return immidiate data pointer
                f8=free;
            }
            else if(arrival[arrival.size()-1]=='d'){
                arrival=arrival.substr(0,arrival.size()-1);
                if(!is_number(arrival)){
                    cout<<"syntax error";
                    return nullptr;
                }
                int mem=stoi(arrival);
                if(mem>65535){
                    cout<<"error";
                }
                possiblevar=mem;
                arrive=&possiblevar;
                f8=free;
            }
            else if(arrival[0]==39&&arrival[arrival.size()-1]==39&&arrival.size()==3){  //operand is given as char
                int num=arrival[1];
                possiblevar=num;
                arrive=&possiblevar;
                f8=free;
            }
            else {
                if(!is_number(arrival)){
                    cout<<"syntax error";
                    return nullptr;
                }
                int mem = stoi(arrival);
                if (mem > 65535) {
                    cout << "error";
                    return nullptr;
                }
                possiblevar=mem;
                arrive=&possiblevar;
                f8=free;

            }
        }
        if(*arrive>=256){
            a8=1;
            f8=0;
        }

    }

    return arrive;
}



/*
 * Gets a string representing the departure group of tokens, boolean references of size, unsgigned short data, memoryaccess and offset
 * d8,f8,possiblevar,memoryaccess are given to be updated
 * offset is given to pass information of offset condition
 * d8=1 if departure represents word size data or memory or register
 * f8=1 if departure does not have a fixed size
 * possiblevar will keep if an immidiate data is given as departure
 * memoryaccess=1 if arrival represents an index in memory (variable or memory addressing)
 * offset=1 shows offset condition should be applied
 * function returns the appropriate pointer for given token group
 * returns nullptr if there is an error
 */
unsigned short *operate::departptr(string departure,bool&a8, bool &d8, bool &f8, unsigned short &possiblevar,bool& memoryaccess,bool&offset) {
    //no pass by reference since we should keep original departure in redirect method
    //procedure is almost same with arriveptr
    //just called for instructions that have 2 parts of operands

    if(offset){
        for(int i=accessPermit;i<memory.size();i++){
            if(!memory[i].first.compare(departure)){
                possiblevar=i;
                if(i>=256){
                    d8=1;
                }
                else{
                    f8=1;
                }
                return &possiblevar;
            }
        }
        cout<<"error";
        return nullptr;   //offset must be used with variable
    }

    bool free=1;    //not declared with w or b
    if(departure[1]==' '){   //w var form   also goes for mov al,w [1234h]...
        if(departure[0]=='w'){
            d8=1;
        }
        else if(departure[0]=='b'){
            d8=0;
        }
        else{
            cout<<"syntax error";
            return nullptr;
        }
        departure=departure.substr(2);
        free=0;
    }


    for(int i=0;i<memory.size();i++){   //check if operand is variable
        if(!memory[i].first.compare(departure)){
            if(free) {
                d8 = (memory[i + 1].first.size() == 0 && memorycounter > i + 1);   //if operand is not declared with w, or b, determine by its initialization
            }
            memoryaccess=1;
            return &memory[i].second;
        }
    }

    unsigned short* depart;


    if(!departure.compare("ax")){
        d8=1;
        return &ax;
    }
    else if(!departure.compare("bx")){
        d8=1;
        return &bx;
    }
    else if(!departure.compare("cx")){
        d8=1;
        return &cx;
    }
    else if(!departure.compare("dx")){
        d8=1;
        return &dx;
    }
    else if(!departure.compare("ah")){
        return &ah;
    }
    else if(!departure.compare("al")){
        return &al;
    }
    else if(!departure.compare("bh")){
        return &bh;
    }
    else if(!departure.compare("bl")){
        return &bl;
    }
    else if(!departure.compare("ch")){
        return &ch;
    }
    else if(!departure.compare("cl")){
        return &cl;
    }
    else if(!departure.compare("dh")){
        return &dh;
    }
    else if(!departure.compare("dl")){
        return &dl;
    }
    else if(!departure.compare("di")){
        d8=1;
        return &di;
    }
    else if(!departure.compare("sp")){
        d8=1;
        return &sp;
    }
    else if(!departure.compare("si")){
        d8=1;
        return &si;
    }
    else if(!departure.compare("bp")){
        d8=1;
        return &bp;
    }
    else{  //for non register addresses
        if((departure[0]=='['||departure[1]=='[')&&departure[departure.size()-1]==']'){
            memoryaccess=true;
            if(departure[0]=='[') {
                departure = departure.substr(1, departure.size() - 2);  //examine between brackets
                f8=free;
            }
            else{
                if(departure[0]=='w'){
                    departure = departure.substr(2, departure.size() - 3);
                    d8=1;

                }
                else if(departure[0]=='b'){
                    departure = departure.substr(2, departure.size() - 3);
                }
            }
            for(int i=0;i<memory.size();i++){   //variable check in []
                if(!memory[i].first.compare(departure)){
                    memoryaccess=1;
                    bool w=(memory[i+1].first.size()==0&&memorycounter>i+1);   //if variable is dw
                    int num=(w)?loadWordSizeData(i):memory[i].second; //if dw, get word size data else byte size
                    if(num<accessPermit){
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    return &memory[num].second;  //reach to data th address
                }
            }


            if(!departure.compare("ax")||!departure.compare("cx")||!departure.compare("dx")||!departure.compare("ah")||!departure.compare("al")||
                    !departure.compare("bh")||!departure.compare("bl")||!departure.compare("ch")||!departure.compare("cl")||!departure.compare("dh")||
                    !departure.compare("dl")||!departure.compare("sp")){
                cout<<"error";
                return nullptr;

            }
            else if(!departure.compare("bx")){
                depart=&memory[bx].second;
                if(bx<accessPermit){
                    cout<<"access not allowed";
                    return nullptr;
                }
            }

            else if(!departure.compare("di")){
                depart=&memory[di].second;
                if(di<accessPermit){
                    cout<<"access not allowed";
                    return nullptr;
                }
            }

            else if(!departure.compare("si")){
                depart=&memory[si].second;
                if(si<accessPermit){
                    cout<<"access not allowed";
                    return nullptr;
                }
            }
            else if(!departure.compare("bp")){
                depart=&memory[bp].second;
                if(bp<accessPermit){
                    cout<<"access not allowed";
                    return nullptr;
                }
            }

            else{
                if(departure[departure.size()-1]=='h') {//hexadecimal memory address
                    departure=departure.substr(0,departure.size()-1);
                    if(departure[0]>57||departure[0]<48){
                        cout<<"syntax error";
                        return nullptr;
                    }
                    int mem=hexToDec(departure);
                    if(mem==-1){
                        cout<<"syntax error";
                        return nullptr;
                    }
                    if(mem>65535){
                        cout<<"error";
                        return nullptr;
                    }
                    if(mem<accessPermit){
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    depart=&memory[mem].second;

                }
                else if(departure[departure.size()-1]=='d'){
                    departure=departure.substr(0,departure.size()-1);
                    if(!is_number(departure)){
                        cout<<"syntax error";
                        return nullptr;
                    }
                    int mem=stoi(departure);
                    if(mem>65535){
                        cout<<"error";
                        return nullptr;
                    }
                    if(mem<accessPermit){
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    depart=&memory[mem].second;

                }
                else{
                    if(!is_number(departure)){
                        cout<<"syntax error";
                        return nullptr;
                    }
                    int mem=stoi(departure);
                    if(mem>65535){
                        cout<<"error";
                        return nullptr;
                    }
                    if(mem<accessPermit){
                        cout<<"access not allowed";
                        return nullptr;
                    }
                    depart=&memory[mem].second;

                 }
            }
        }
        else{    //test immediate data
            if(departure[departure.size()-1]=='h') {//hexadecimal memory address
                departure=departure.substr(0,departure.size()-1);
                if(departure[0]>57||departure[0]<48){
                    cout<<"syntax error";
                    return nullptr;
                }
                int mem=hexToDec(departure);
                if(mem==-1){
                    cout<<"syntax error";
                    return nullptr;
                }
                if(mem>65535){
                    cout<<"error";
                    return nullptr;
                }
                possiblevar=mem;
                depart=&possiblevar;
                f8=free;
            }
            else if(departure[departure.size()-1]=='d'){
                departure=departure.substr(0,departure.size()-1);
                if(!is_number(departure)){
                    cout<<"syntax error";
                    return nullptr;
                }
                int mem=stoi(departure);
                if(mem>65535){
                    cout<<"error";
                    return nullptr;
                }
                possiblevar=mem;
                depart=&possiblevar;
                f8=free;
            }
            else if(departure[0]==39&&departure[departure.size()-1]==39&&departure.size()==3){  //operand is given as char
                int num=departure[1];
                possiblevar=num;
                depart=&possiblevar;
                f8=free;
            }
            else{
                if(!is_number(departure)){
                    cout<<"syntax error";
                    return nullptr;
                }
                int mem=stoi(departure);
                if(mem>65535){
                    cout<<"error";
                    return nullptr;
                }
                possiblevar=mem;
                depart=&possiblevar;
                f8=free;


            }
        }
    }
    if(*depart>255){
        d8=1;
        f8=0;
    }

    return depart;
}

/*
 * gets a string in '***' form end exclude the char in it
 */
int excludeChar(string s){
    s=s.substr(1,s.size()-2);
    if(!s.size()==1){
        return -1;  //means syntax error
    }
    return (int)s[0];
}

/*
 * checks the value of a variable is valid or not
 * if valid, updates the value val
 * if not returns false
 */
bool checkval(string s,int&val,operate*o){
    if(s[0]==39&&s[s.size()-1]==39){   //checks if value is given as char
        val=excludeChar(s);
        if(val==-1){
            return false;
        }
    }
    else{
        if(s[s.size()-1]=='h'){ //value given in hex
            s=s.substr(0,s.size()-1);
            val=o->hexToDec(s);
            if(val==-1){
                return false;
            }
        }
        else if(s[s.size()-1]=='d') {  //value given in decimal
            s = s.substr(0, s.size() - 1);
            if(!o->is_number(s)){
                return false;
            }
            val = stoi(s);
        }
        else if(o->is_number(s)){    //checks if value is given as number
            val=stoi(s);

        }
        else{   //value can not be anything else
            return false;
        }
        if(val>65535){
            cout<<"error";
            return false;
        }
    }
    return true;
}



/*
 * gets a string as variable name, an integer as value, a string as byteholder to determine the size
 * stores the variable with its value in memory according to its defined size
 * updates memorycounter to point the new available memory address
 * if byteholder is not valid, returns false
 */
bool addValToMemory(operate* o,string byteholder,string var,int val){
    if(!byteholder.compare("db")){
        if(val>=256){  //byte size data needed
            cout<<"incompatible";
            return false;
        }
        o->memory[o->memorycounter].first=var;
        o->memory[o->memorycounter].second=val;
        o->memorycounter++;
    }
    else if(!byteholder.compare("dw")){
        o->memory[o->memorycounter].first=var;
        o->writeWordSizeData(o->memorycounter,val);  //value is stored in 2 indexes
        o->memorycounter+=2;
    }
    else{
        return false;
    }
    return true;
}

/*
 * Before execution, stores the variables in memory and navigates the label lines
 * checks validity of label and variable definitions and updates valid
 */
void operate::assignvar(vector<vector<string>> &tokens,bool& valid) {

    for(int i=2;i<tokens.size();i++){
        vector<string>line=tokens[i];
        if(i>endingline) {   //variable definitions are after int 20h
            if(!line[1].compare("code")&&!line[2].compare("ends")){ //ignore after
                break;
            }

            if(line.size()==1){  //ignore empty lines
                continue;
            }

            if(line.size()==5){    // var : db 12
                if(line[2].compare(":")){
                    cout<<"syntax error";
                    valid=0;
                    return;
                }
                int val=0;
                string var=line[1];
                if(!isValidVariable(var)){   //checks whether the variable name is valid or not
                    cout<<"syntax error";
                    valid=0;
                    return;
                }
                if(!checkval(line[4],val,this)){    //checks the value to assign and updates val
                    valid=0;
                    return;
                }
                bool toMemory=addValToMemory(this,line[3],var,val);
                if(!toMemory){
                    valid=0;
                    return;
                }
            }
            else if(line.size()==4){

                int val=0;
                int colon=0;
                string var;
                if(line[1][line[1].size()-1]==':'){  // var: db 12 /
                    var=line[1].substr(0,line[1].size()-1);
                    colon=1;  //place of colon
                }
                else {  //var db 12
                    var = line[1];
                }
                if(!isValidVariable(var)){
                    cout<<"syntax error";
                    valid=0;
                    return;
                }
                if(!checkval(line[3],val,this)){
                    valid=0;
                    return;
                }
                if(colon==1&&line[2][0]==':'){ //2 colons
                    cout<<"syntax error";
                    valid=0;
                    return;
                }
                if(line[2][0]==':'){
                    line[2]=line[2].substr(1,line[2].size()); //ignore :
                    colon=2;
                }
                bool toMemory=addValToMemory(this,line[2],var,val);
                if(!toMemory){
                    valid=0;
                    return;
                }

            }
            else if(line.size()==3){

                if(!line[2].compare(":")){ // 2line definition
                    int val=0;
                    string var=line[1];
                    if(!isValidVariable(var)){   //checks whether the variable name is valid or not
                        cout<<"syntax error";
                        valid=0;
                        return;
                    }
                    line=tokens[i+1];  //check other line
                    if(line.size()!=3){   //variable declared upper line has no mean anymore
                        continue;
                    }
                    if(!checkval(line[2],val,this)){
                        continue;    //maybe another declaration so give a chance to next line
                    }
                    bool toMemory=addValToMemory(this,line[1],var,val);
                    if(!toMemory){
                        continue;
                    }
                    i++;  //2 lines are used for declaration so skip

                }
                else{
                    int val=0;
                    string var="";
                    string byteholder="";
                    for(int i=0;i<line[1].size();i++){
                        if(line[1][i]==':'){
                            var=line[1].substr(0,i);
                            byteholder=line[1].substr(i+1,line[1].size());
                        }
                    }
                    if(!var.compare("")){  //means no :
                        cout<<"syntax error";
                        valid=0;
                        return;
                    }
                    if(!isValidVariable(var)){   //checks whether the variable name is valid or not
                        cout<<"syntax error";
                        valid=0;
                        return;
                    }
                    if(!checkval(line[2],val,this)){
                        valid=0;
                        return;
                    }
                    bool toMemory=addValToMemory(this,byteholder,var,val);
                    if(!toMemory){
                        valid=0;
                        return;
                    }

                }

            }
            else if(line.size()==2){
                if(line[1][line[1].size()-1]!=':'){
                    valid=0;
                    return;
                }
                string var=line[1].substr(0,line[1].size()-1);
                int val=0;
                if(!isValidVariable(var)){
                    cout<<"syntax error";
                    valid=0;
                    return;
                }
                line=tokens[i+1]; //check other line
                if(line.size()!=3){   //variable declared upper line has no mean anymore
                    continue;
                }
                if(!checkval(line[2],val,this)){
                    continue;
                }
                bool toMemory=addValToMemory(this,line[1],var,val);
                if(!toMemory){   //check how to manage return or continue
                    continue;
                }
                i++; //2 lines are used for declaration
            }

        }

        else{  //means label

            pair<string,int>p;
            p.second=i;
            if(line.size()==3&&!line[2].compare(":")){ //label :
                p.first=line[1];
                labelLines.push_back(p);
                memorycounter-=6;  //labels are not instructions, they do not occupy memory
                accessPermit-=6;
            }
            else if(line.size()==2&&line[1][line[1].size()-1]==':'){  //label:
                p.first=line[1].substr(0,line[1].size()-1);
                labelLines.push_back(p);
                memorycounter-=6;
                accessPermit-=6;
            }

        }
    }
}
