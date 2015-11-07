#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
    string temp;
    string result;
    for (int k = 0; k < numbers.size(); k++)
    {
        temp += convertNumberToBitString(numbers[k]);       //converts each number to correct binary string
    }
    
    for (int j = 0; j < temp.size(); j++)
    {
        if (temp[j] == '0')     //0's turn into spaces
            result += " ";
        
        else if (temp[j] == '1')        //1's turn into tab characters
            result +=  "\t";
    }
    
    return result;
}

bool BinaryConverter::decode(const string& bitString,
                             vector<unsigned short>& numbers)
{
    unsigned short add = 0;
    string temp;
    if (bitString.size() % 16 != 0)     //ensures that the size of the bitString is a multiple of 16
        return false;

    numbers.clear();        //empties previous data in the numbers vector
    
    for (int k = 0; k < bitString.size(); k++)
    {
        if (temp.size() != 0 && temp.size() % 16 == 0)
        {
            convertBitStringToNumber(temp, add);        //converts each complete strand of 16 into numbers
            numbers.push_back(add);
            temp = "";
        }
        
        if (bitString[k] == ' ')        //appends 0's onto the running temp string
            temp += '0';
            
        else if (bitString[k] == '\t')      //appends 1's onto the running temp string
            temp += '1';
        else
            return false;
    }

    if (temp.size() != 0 && temp.size() % 16 == 0) //this code is needed for the final complete temp string
    {
        convertBitStringToNumber(temp, add);
        numbers.push_back(add);
        temp = "";
    }
    
    return true;
}

string convertNumberToBitString(unsigned short number)
{
    string result(BITS_PER_UNSIGNED_SHORT, '0');
    for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
    {
        k--;
        if (number % 2 == 1)
            result[k] = '1';
    }
    return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
    if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
        return false;
    number = 0;
    for (size_t k = 0; k < bitString.size(); k++)
    {
        number *= 2;
        if (bitString[k] == '1')
            number++;
        else if (bitString[k] != '0')
            return false;
    }
    return true;
}
