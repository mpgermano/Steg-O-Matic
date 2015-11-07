#include "HashTable.h"
#include "provided.h"
#include <iostream>
#include <string>
using namespace std;


unsigned int computeHash(string key)
{
    unsigned int h = 2166136261U;           //code provided by Professor Smallberg for efficient
    for (int k = 0; k < key.size(); k++)    //usigned ints from strings
    {
        h += key[k];
        h *= 16777619;
    }
    return h;
}

unsigned int computeHash(unsigned short key)
{
    unsigned int h = key;
    return h;
}



void Compressor::compress(const std::string& s, std::vector<unsigned short>& numbers)
{
    numbers.clear();        //numbers vector emptied out
    
    unsigned int a = s.size()/2 + 512;
    unsigned int b = 16384;
    
    unsigned int cap;
    if (a <= b)
        cap = a;
    else
        cap = b;
    
    HashTable<string, unsigned short> H(2 * cap, cap);      //buckets size is twice that of the capacity
                                                            //this sets the load factor at 0.5
    for (int j = 0; j < 256; j++)
    {
        string temp(1, static_cast<char>(j));
        H.set(temp, j, true);
        
    }
    
    int nextFreeID = 256;
    string runSoFar;
    vector<unsigned short> V;
    unsigned short x;
    
    for (int k = 0; k < s.size(); k++)
    {
        string expandedRun = runSoFar + s[k];
        
        if (H.get(expandedRun, x))      //checks to see if expandedRun is already contained in HashTable
        {
            runSoFar = expandedRun;
            continue;       //if so next character of the string is processed
        }
        
        H.get(runSoFar, x);
        V.push_back(x);
        H.touch(runSoFar);      //runSoFar string is moved to the top of the recently-updated list
        runSoFar = "";
        
        string currentC (1, s[k]);
        unsigned short cv;
        H.get(currentC, cv);
        V.push_back(cv);
        
        if (!H.isFull())        //if there is an open space in the HashTable, then expandedRun is inserted
        {                       //with the next available ID number
            H.set(expandedRun, nextFreeID);
            nextFreeID++;
        }
        
        else            //otherwise the least recently updated spot is discard, and new data is inserted
        {               //with that same ID number
            string dummy;
            unsigned short discardSpot;
            H.discard(dummy, discardSpot);
            H.set(expandedRun, discardSpot);
        }
    }
    
    
    if (runSoFar != "")
    {
        unsigned short x;
        H.get(runSoFar, x);
        V.push_back(x);
    }
    
    V.push_back(cap);       //the capacity is pushed onto the end of the numbers vector
    numbers = V;

}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
    if (numbers.empty())        //ensures there are numbers to actually decompress
    {
        return false;
    }
    
    unsigned int cap = numbers.back();
    
    HashTable<unsigned short, string> H(2 * cap, cap);
    
    for (int j = 0; j < 256; j++)
    {
        string temp(1, static_cast<char>(j));
        H.set(j, temp, true);
    }
    
    int nextFreeID = 256;
    string runSoFar;
    string output;
    
    for (int n = 0 ; n < numbers.size() - 1; n++)
    {
        unsigned short us = numbers[n];
        if (us <= 255)      //if us <= 255, the string is a 1 digit character
        {
            string temp;
            H.get(us, temp);        //since all one digit chars exist in H, that char is found and appended
            output += temp;
            
            if (runSoFar == "")
            {
                runSoFar = temp;
                continue;
            }
            
            string expandedRun = runSoFar + temp;
            if (!H.isFull())        //if the Hashtable is not full, the nextFreeID key can be associated
            {                       //without any discarding
                string x;
                H.set(nextFreeID, expandedRun);
                nextFreeID++;
            }
            
            else        //otherwise a discard must be made to create space for new associations
            {
                string y;
                unsigned short discardSpot;
                H.discard(discardSpot, y);
                H.set(discardSpot, expandedRun);
            }
            
            runSoFar = "";
            continue;
        }
        
        else
        {
            string s;
            if (!H.touch(us))
            {
                return false;
            }
            
            H.touch(us);
            H.get(us, s);
            output += s;
            runSoFar = s;
        }
        
    }
    s = output;     //the string s is finally set to the output string
    return true;
}



