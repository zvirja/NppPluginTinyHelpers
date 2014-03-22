#pragma once
#include <memory>
#include <vector>
#include <set>
#include <map>

class TabIndentsNormalizer
{
public:
	TabIndentsNormalizer(int tabIndent, std::unique_ptr<char[]> allText, int allTextLen) :_tabIndent(tabIndent)
	{
		this->_allText = std::vector<char>();
		this->_allText.insert(this->_allText.end(), allText.get(), allText.get() + allTextLen);
	}
	virtual ~TabIndentsNormalizer();
	std::vector<char> GetTextWithNormalizedIndents();
private:
	int _tabIndent;
	const int _finalTabIndent = 2;
	std::vector<char> _allText;
	std::pair<int,int> GetLineIndent(const std::vector<char>& line);
	std::vector<std::vector<char>> GetLines();
	void AppendLineWithIndent(std::vector<char>& finalOutput, const std::vector<char>& line, std::pair<int,int>& virtualRealLineIndent, std::map<int, int> const& indentMappings);
};

