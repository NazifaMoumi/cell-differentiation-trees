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
void combinationUtil(int data[], int start, int end, int index, int r)
{
    // Current combination is ready to be printed, print it
    if (index == r)
    {
        stringstream convert;
        convert << countt;
        string post;
        post = convert.str();
        countt++;

        string name="Quert"+post+".phy";
        const char * c = name.c_str();
        ofstream myfile1;
        myfile1.open (c);

        stringstream seq;
        seq << sequence;
        string len;
        len = seq.str();

        string line="4 "+len+"\n";

        myfile1 << line;

        for (int j=0; j<r; j++)
        {
             //printf("%d ", data[j]);
             int position=data[j];
             //cout<<position;
             string row=taxa_info.at(position-1);
             //cout<<row.length()<<"\n";
             stringstream headr;
             headr << position;
             string header="t";
             header = header+headr.str()+"    ";
             //cout<<header;
             row=row+"\n";
             header=header+row;
          //   cout<<header;
             myfile1<<header;
             //cout<<header<"\n";
        }
        myfile1.close();
        return;
    }

    for (int i=start; i<=end && end-i+1 >= r-index; i++)
    {
        data[index] = i+1;
        //if(countt==20){return;}
        combinationUtil( data, i+1, end, index+1, r);
    }
}
void printCombination(int n, int r)
{
    int data[r];
    combinationUtil( data, 0, n-1, 0, r);
}

int main()
{

    string line;

    int quert=4;
    int counter=0;
    char delim = ' ';
    int value;
  ifstream myfile ("overlap_datarepresentation_interesting_regions_c.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {

      counter++;
      if(counter==1)
      {
        //cout <<" hv"<< line<<" hv" << '\n';
        stringstream ss(line);
        string token;
        while (std::getline(ss, token, delim)) {
            //cout<<token<<endl;
            value = std::atoi(token.c_str());
            //cout<<value<<endl;
            if(taxa==-1)
                taxa = value;
            else
                sequence = value;
        }
      }
      else
      {
        string strr=removeSpaces(line);;

        taxa_info.push_back(strr);
      }
    }
    myfile.close();
  }
  else cout << "Unable to open file";
    //cout<<taxa;
    string len=taxa_info.at(0);
    sequence=len.length();
    cout<<sequence;
    printCombination( taxa, 4);


}
