#include "TabIndentsNormalizer.h"
using namespace std;


TabIndentsNormalizer::~TabIndentsNormalizer()
{
}


vector<char> TabIndentsNormalizer::GetTextWithNormalizedIndents()
{
	vector<vector<char>> lines = this->GetLines();
	
	vector<pair<int,int>> lineIndents;
	set<int> possibleIndents;

	for (vector<char>& line : lines)
	{
		pair<int, int> virtualRealPair = this->GetLineIndent(line);
		lineIndents.push_back(virtualRealPair);
		possibleIndents.insert(virtualRealPair.first);
	}

	map<int,int> indentsMap;
	int newIndexCounter = 0;
	for (int pIndent : possibleIndents)
	{
		indentsMap.insert(std::pair<int,int>(pIndent, (newIndexCounter++) * (this->_finalTabIndent)));
	}

	vector<char> finalOutput;
	for (size_t i = 0; i < lines.size(); i++)
	{
		this->AppendLineWithIndent(finalOutput, lines.at(i), lineIndents.at(i), indentsMap);
	}

	return finalOutput;
}



pair<int, int> TabIndentsNormalizer::GetLineIndent(const vector<char>& line)
{
	int virtualIndent = 0;
	int realIndent = 0;
	for (char currChar : line)
	{
		if (currChar == ' ' || currChar == '\t')
		{
			virtualIndent += currChar == '\t' ? this->_tabIndent : 1;
			++realIndent;
		}
		else
		{
			break;
		}
	}
	return pair<int, int>(virtualIndent, realIndent);
}

vector<vector<char>> TabIndentsNormalizer::GetLines()
{
	vector<vector<char>> result;
	vector<char> currentLine;
	
	const char etalonBreak = '\n';
	for (char currentChar : this->_allText)
	{
		currentLine.push_back(currentChar);
		if (currentChar == etalonBreak)
		{
			result.push_back(std::move(currentLine));
			currentLine = vector<char>();
		}
	}
	if (currentLine.size() > 0)
		result.push_back(std::move(currentLine));
	return result;
}

void TabIndentsNormalizer::AppendLineWithIndent(std::vector<char>& finalOutput, const std::vector<char>& line, std::pair<int, int>& virtualRealLineIndent, std::map<int, int> const& indentMappings)
{
	size_t beginningSpaces = static_cast<size_t>(indentMappings.at(virtualRealLineIndent.first));
	for (size_t i = 0; i < beginningSpaces; i++)
	{
		finalOutput.push_back(' ');
	}
	auto iteratorCurrChar = line.cbegin();
	std::advance(iteratorCurrChar, virtualRealLineIndent.second);
	for (; iteratorCurrChar != line.cend();iteratorCurrChar++)
	{
		finalOutput.push_back(*iteratorCurrChar);
	}
}
