#ifndef FLATFILE_H
#define FLATFILE_H

#include <vector>
#include <map>
#include <string>
#include <fstream>
using namespace std;

typedef vector< vector<string> >::const_iterator lineIterator;

class FlatFileReader
{
public:

	FlatFileReader(string filename);

	map<string, int> headers;
	vector< vector<string> > lines;
};

#endif
