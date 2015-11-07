#include "provided.h"
#include "http.h"
#include <string>
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
    host = "";
    string page;
	if (!HTTP().get(url, page))     //ensures that the webpage is loaded correctly
        return false;
    
    if (Steg::hide(page, msg, host))        //ensures that the message can be hidden correctly in the page
        return true;
    
    return false;
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
	msg = "";
    string page;
    
    if (!HTTP().get(url, page))  //ensures that the webpage is loaded correctly
        return false;
    
    if (Steg::reveal(page, msg))        //ensures that there is in fact a secret message, formatted
        return true;                    //correctly that can be decoded
    
    return false;
}
