#include <iostream>
using namespace std;
#include <string>
#include <sstream>
#include <fstream>
#include <time.h>
#include <cstdlib>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <cmath>

int main(int argc, char *argv[])
{
	string s, name;
	char buffer[200];	
	freopen(argv[1],"r", stdin);
	cin>>s;
	freopen(argv[2],"w", stdout);
	
	int i = 0,j, start = 0,len, length;
	j = s.find(',');
	while(j>0)
	{	
		len = j - start;
		length=s.copy(buffer,len,start);
		buffer[length]='\0';
		name.assign(buffer);
		
		cout<<i<<" "<<name<<endl;
		i++;
	
		s.assign(s.replace(0,j+1,""));

		j = s.find(',');

	}
	cout<<i<<" "<<s<<endl;
		
	 


	return 0;
}
