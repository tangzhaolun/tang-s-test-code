/* This file shows how to modify a text-based file with standard fstream operations
 * One important thing: don't read and write file at one time, the best way is to
 * read the whole file into memory, modify in memory, then write back to file.
 */
#include <string>
#include <iostream>
#include <iterator>
#include <fstream>
#include <assert.h>
#include <string.h>
using namespace std;

void replaceString (string &modifyStr, 
        const string &findMe, const string &newStr) 
{
    int index = modifyStr.find (findMe);
    modifyStr.replace (index, findMe.size(), newStr);
}

/*int main()//unit test for replaceString
{
    string s("It is a good good weather.");
    cout << s << endl;
    string findme = "good";
    string newstr = "bad";
    replaceString(s, findme, newstr);
    cout << s << endl;
}*/


int main(int argc, char *argv[])
{
    assert (argc == 2);

    size_t filesize;
    string str;

    //ifstream only for input/read, ofstream only for output/write
    //fstream for both reading and writing
    ifstream infile(argv[1], ios::in);
    if (!infile.is_open())
    {
        cout << "file open failed" << endl;
        return -1;
    }

    infile.seekg(0, ios::end);
    filesize = infile.tellg();
    //tellg returns int value of the number of characters between 
    //the beginning of input sequence (file) and current position

    //reserve enough capacity in the string to store an entire file
    str.reserve (filesize);

    //set the get pointer at beginning of file
    //or you may call file.seekg(0) to do same job
    infile.seekg(0, ios::beg);    
    
    //read file into a string by stream iterator, you can also use
    //infile.get(char) or file.getline to write to the string, e.g.
        //char line[256];
        //file.getline (line, 256); //ok
        //file.getline (str, 256); //error, because
        //getline's first argument is char *, not a string
        //getline (file, str); //ok
    /*istream_iterator<string>DataBegin(infile);
    istream_iterator<string>DataEnd;
    while (DataBegin != DataEnd) {
        str += *DataBegin;
        DataBegin++;
    }*/

    char c;
    while(!infile.eof()){
        infile.get(c);
        str += c;
    }
    cout << "File content:" << endl << str << endl;
    infile.close();
        
        
   /*     //remove comment mark at start of each line
        if (str.find("//") == 0)//don't use find_first_of here
        {
            replaceString (str, "//", "  ");
            file.write (str.c_str(), str.size());
            file.put ('\n');
            break;
        }*/ 
    
    
    //execl("cat", argv[1]);

    return 0;
}
