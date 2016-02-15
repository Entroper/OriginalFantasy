#include <sstream>
#include "FlatFile.h"

FlatFileReader::FlatFileReader(string filename)
{
	ifstream infile(filename.c_str());
	
	string headerLine;
	getline(infile, headerLine);
	istringstream headerISS(headerLine);	
	string header;
	int headerIndex = 0;
	while (getline(headerISS, header, '\t'))
	{
		headers[header] = headerIndex;
		headerIndex++;
	}
	
	string line;
	while (getline(infile, line))
	{
		istringstream lineISS(line);
		vector<string> tokens;
		string token;

		while (getline(lineISS, token, '\t'))
			tokens.push_back(token);

		lines.push_back(tokens);
	}
}
