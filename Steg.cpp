#include "provided.h"
#include <string>
#include <iostream> //delete eventually
#include <cctype>
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut)
{
    if (hostIn == "")       //ensures there actually is input to append the message to
        return false;
    
    hostOut = "";
    
    vector<unsigned short> compressedNum;
    
    Compressor::compress(msg, compressedNum);
    string secretM = BinaryConverter::encode(compressedNum);        //contains the secret message in tabs
    int L = secretM.size();                                         // and spaces
    
    string subString;
    vector<string> lines;
    for (int k = 0; k < hostIn.size(); k++)
    {
        string checkNew;
        if (hostIn[k] == '\r')      //this is a form of a newline (that is followed by a newline) and thus
        {                           //is not counted.  K is moved forward to skip the newline char and
            lines.push_back(subString);     //the string is added to the lines vector
            subString = "";
            k++;
        }
        else if (hostIn[k] == '\n')     //full line is added to the lines vector
        {
            lines.push_back(subString);
            subString = "";
        }
        
        else
            subString += hostIn[k];     //otherwise there is not a full line and the char is appended
    }
    
    if (subString != "")
        lines.push_back(subString);
    
    for (int g = lines.size() - 1; g >= 0; g--)     //work backwards through lines vector
    {
        int count = 0;
        
        for (int p = lines[g].size() - 1; p >= 0; p--)
        {
            if (isblank(lines[g][p]))       //count how many spaces at the end are whitespace
            {
                count++;
                if (p == 0)             //this code checks to ensure the whole line is not whitespace
                    lines[g] = "";      //if it is, the line is simply set to the empty string
            }
            else
            {
                string temp = lines[g].substr(0, lines[g].size() - count);    //creates substring of
                lines[g] = temp;                                              //all meaningful chars
                break;
            }
            
        }
        
    }
    
    int N = lines.size();
    
    int firstSub = L % N;               //determines how many lines should append firstLength
    int firstLength = (L / N) + 1;
    int restLength = L / N;
    
    int runTotal = 0;
    
    for (int f = 0; f < firstSub; f++)    //appends firstLength chars of the secretMessage to firstSub lines
    {
        lines[f] += secretM.substr(runTotal, firstLength);
        runTotal += firstLength;
    }
    
    for (int k = firstSub; k < N; k++)  //appends restLength chars of the secretMessage to the rest
    {
        lines[k] += secretM.substr(runTotal, restLength);
        runTotal += restLength;
    }
    for (int h = 0; h < lines.size(); h++)
    {
        hostOut += lines[h];        //output string result of all lines appended with a newline between each
        hostOut += '\n';
    }
    
    return true;
}

bool Steg::reveal(const string& hostIn, string& msg)
{
    if (hostIn == "")       //ensures there is text to decode
        return false;

    msg = "";
    
    string subString;
    vector<string> lines;
    for (int k = 0; k < hostIn.size(); k++)
    {
        string checkNew;        //the following code splits hostIn into its appropriate lines
        if (hostIn[k] == '\r')
        {
            lines.push_back(subString);
            subString = "";
            k++;
        }
        else if (hostIn[k] == '\n')
        {
            lines.push_back(subString);
            subString = "";
        }
        
        else
            subString += hostIn[k];
    }
    
    if (subString != "")                //this code is necessary to ensure that the final line is added
        lines.push_back(subString);     //even if it does not contain a newline at the end.
    
    string concate;
    
    for (int g = 0; g < lines.size(); g++)
    {
        int count = 0;
        
        for (int p = lines[g].size() - 1; p >= 0; p--)
        {
            if (isblank(lines[g][p]))
            {
                count++;
                if (p == 0)
                    concate += lines[g];        //this is necessary if the entire line is whitespace
            }
            else
            {
                string temp = lines[g].substr(lines[g].size() - count, lines[g].size());
                concate += temp;        //full substrings of the secret message are appended
                break;
            }
        }
    }
    //the secrete message is then decoded and decompress to reveal the actual message.
    vector<unsigned short> decomp;
    if (BinaryConverter::decode(concate, decomp))
    {
        if (Compressor::decompress(decomp, msg))
            return true;
    }
    
    return false;
}