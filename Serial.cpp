#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include <cstdlib>
#include <algorithm>
using namespace std;

string removeSpaces(string str)
{
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

vector<string> taxa_info;
int taxa=-1;
long long sequence=0;
long long countt=1;
int main()
{
    string line;
    int quert=4;
    int counter=-1;
    char delim = ' ';
    int value;
    ofstream myfile1;
    myfile1.open ("serial_h3k27ac_1.txt");

  ifstream myfile ("overlap_datarepresentation_interesting_regions_c.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {

      counter++;
      if(counter==0)
      {
          line=line+"\n";
             myfile1 << line;
      }
      else
      {
        //string strr=removeSpaces(line);;
            stringstream convert;
            convert << counter;
            //cout<<"\niny "<<counter;
            string post;
            post = convert.str();
            string name="t"+post+"  ";
            //cout<<name;
            line=name+line+"\n";
            //if(counter==1)cout<<line;
             myfile1 << line;
      }
    }
    myfile.close();
  }
  else cout << "Unable to open file";

myfile1.close();
}
