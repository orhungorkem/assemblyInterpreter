#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include "operate.h"
#include <cstdio>

using namespace std;

void execute(int linenum,operate*o,vector<vector<string>>&tokens,vector<pair<int,int>>realLines);
void parse(ifstream& myfile,vector<vector<string>>&tokens,int numberoflines,vector<pair<int,int>>&realLines,operate*o,bool&valid);




int main (int argc, char* argv[]) {

    int numberoflines=0;  //keeps the number of non empty lines in input
    operate* o=new operate();  //operator object that contains necessary functions,memory,registers...
    vector<vector<string>>tokens;   //tokens[i] holds the tokens in ith line of input
    vector<pair<int,int>>realLines;  //will hold the map from i in tokens to real line number
    ifstream myfile;
    myfile.open(argv[1]);
    if (!myfile) {
        cerr << "There was a problem opening \"" << argv[1] << "\" as input file" << endl;
        return 1;
    }
    bool valid=true;  //boolean that keeps the validity of input
    parse(myfile,tokens,numberoflines,realLines,o,valid);

    if(valid) {
      execute(2, o, tokens,realLines);  //start with second line,skipping code segment
    }



    delete o;
    return 0;
}


/*PARSE FUNCTION
 *
 * Gets file object to parse, fills tokens vector with seperated tokens, updates numberoflines, updates validity
 * Calls assignvar function to store variables and labels before execution
 */


void parse(ifstream& myfile,vector<vector<string>>&tokens,int numberoflines,vector<pair<int,int>>&realLines,operate*o,bool& valid){
    string line;

    vector<string>line0;
    tokens.push_back(line0);   //empty zeroth line
    if(myfile.is_open()) {
        int i=1;
        int k=1;
        //tokens[i] will not really hold, ith line,it will hold ith nonempty line
        //so we will have to map tokens[i] to its real line num to keep real line numbers
        while (getline(myfile, line)) {

            if (line[line.size() - 1] == '\r') {
                line.erase(line.size() - 1);
            }
            vector<string>v;
            string lex0="-";  //fill zeroth indexes with -
            v.push_back(lex0);
            tokens.push_back(v);  //add new line to tokens
            stringstream check1(line);
            string lexeme;



            pair<int,int>linemap;
            while(getline(check1, lexeme, ' '))
            {

                if(!lexeme.compare(" ")||lexeme.size()==0){   //ignore extra space characters
                    continue;
                }
                else {
                    for (int j = 0; j < lexeme.size(); j++) {   //make all characters lowercase to avoid case sensitivity
                        if(lexeme.size()>j+1){
                            if(lexeme[j-1]==39&&lexeme[j+1]==39){   //except chars
                                continue;
                            }
                        }
                        if (lexeme[j] >= 65 && lexeme[j] <= 90) {
                            lexeme[j] += 32;
                        }
                    }

                    if(!lexeme.compare("]")){
                        if(tokens[i][tokens[i].size()-1][0]=='['){   //[var ] conditions
                            tokens[i][tokens[i].size()-1].push_back(']');
                            continue;
                        }
                        else if(!tokens[i][tokens[i].size()-2].compare("[")){   // [ var ] conditions
                            string var=tokens[i][tokens[i].size()-1];
                            tokens[i].erase(tokens[i].begin()+tokens[i].size()-1);
                            tokens[i][tokens[i].size()-1]+=var;
                            tokens[i][tokens[i].size()-1].push_back(']');
                            continue;
                        }
                        else if(tokens[i][tokens[i].size()-2][1]=='['){   // w[ var ] conditions
                            string var=tokens[i][tokens[i].size()-1];
                            tokens[i].erase(tokens[i].begin()+tokens[i].size()-1);
                            tokens[i][tokens[i].size()-1]+=var;
                            tokens[i][tokens[i].size()-1].push_back(']');
                            continue;
                        }

                    }
                    else if(lexeme[lexeme.size()-1]==']'){  // w[ var] / [ var]
                        if(tokens[i][tokens[i].size()-1].find('[')<tokens[i][tokens[i].size()-1].size()) {  //to ignore ']' case
                            tokens[i][tokens[i].size() - 1] += lexeme;
                            continue;
                        }
                    }
                    else if(lexeme.find("],")<lexeme.size()){  // b[bp],
                        if(!tokens[i][tokens[i].size()-1].compare("[")){  //[ var],**
                            tokens[i][tokens[i].size() - 1] += lexeme.substr(0,lexeme.find("]")+1);
                            tokens[i].push_back(lexeme.substr(lexeme.find(",")));
                            continue;
                        }
                        else if(tokens[i][tokens[i].size()-2][1]=='[' || !tokens[i][tokens[i].size()-2].compare("[")){  // [ var ],**   w[ var ],**
                            string var=tokens[i][tokens[i].size()-1];
                            tokens[i].erase(tokens[i].begin()+tokens[i].size()-1);
                            tokens[i][tokens[i].size()-1]+=var;
                            tokens[i][tokens[i].size()-1].push_back(']');
                            tokens[i].push_back(lexeme.substr(lexeme.find(",")));
                            continue;
                        }
                        else if(tokens[i][tokens[i].size()-1][0]=='['){  //[var ],**
                            tokens[i][tokens[i].size()-1].push_back(']');
                            tokens[i].push_back(lexeme.substr(lexeme.find(',')));
                            continue;
                        }
                    }


                    if(!tokens[i][tokens[i].size()-1].compare("'")&&!lexeme.compare("'")){  //if a char is given as ' ', do not tokenize just assign space characters
                        tokens[i][tokens[i].size()-1]="32";   //space character
                    }
                    else {
                        tokens[i].push_back(lexeme);  //tokens[i] represents ith line, push token to ith line
                    }
                }

            }
            if(tokens[i].size()==1){  //ignore empty line
                i--;
            }
            else{   //for a nonempty line i, real linenumber is j
                linemap.first=i;
                linemap.second=k;
                realLines.push_back(linemap);
            }
            i++;
            k++;


        }
        numberoflines=i-1;

        if(tokens[1][1].compare("code")&&tokens[1][2].compare("segment")){   //first line must be code segment
            cout<<"syntax error";
            valid=0;  //do not accept as valid
            return;
        }
        if(tokens[numberoflines][1].compare("code")&&tokens[numberoflines][2].compare("ends")){ //last line must be code ends
            cout<<"syntax error";
            valid=0;
            return;
        }


    }

    myfile.close();  //parsing the input ends
    bool ending=0;   //keeps whether the ending is valid or not(int 20h exists)

    for(int i=1;i<tokens.size();i++){  //get the end of code

        vector<string>line=tokens[i];
        if(line.size()==1){
            continue;
        }
        if(!line[1].compare("int")){
            if(!line[2].compare("20h")){
                o->endingline=i;   //keeps the line of int 20h
                ending=1;
                break;
            }
        }

    }
    if(!ending){
        cout<<"no ending error";
        valid=0;
        return;
    }

    o->accessPermit=6*(o->endingline-1);   //lines of instructions can not be reached in memory so determine the line of permission (exclude code segment)
    o->memorycounter=6*(o->endingline-1);  //memorycounter keeps the available index for a new variable to be stored then this starts with accesspermit

    o->assignvar(tokens,valid);  //explanation is above the function definition
}

/*EXECUTE FUNCTION
 *
 * Simply takes the line number to be executed and calls redirect method that will parse the line token by token.
 */

void execute(int linenum,operate*o,vector<vector<string>>&tokens,vector<pair<int,int>>realLines){  //determines the line to be executed, calls redirect

    if(linenum==0){   //when the code must finish depending on an error or normally ends, redirect method returns 0 as linenum and execution stops
        if(o->lastExecuted!=o->endingline){
            int last=0;
            for(int i=0;i<realLines.size();i++){
                if(realLines[i].first==o->lastExecuted){
                    last=realLines[i].second;
                    break;
                }
            }
            cout<<endl<<"Line of error: "<<last;
        }
        return;
    }
    if(linenum>=tokens.size()){   //if returned linenum is larger than the number of existing lines, ending becomes unexpected
        cout<<"unexpected ending";
        return;
    }
    vector<string>v=tokens[linenum];  //redirect method just gets the tokens of current line for simplicity
    execute(o->redirect(linenum, v), o, tokens,realLines);

}



