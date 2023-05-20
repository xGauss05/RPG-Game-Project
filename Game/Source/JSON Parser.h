#pragma once

#include <fstream>
#include <string>
#include <ranges>
#include <stack>

class JSON_Parser
{
public:

	JSON_Parser &load_file(std::string_view name)
	{
		read_file(name);
		std::erase_if(content, [](char i) { return i == ' ' || i == '\n'; });
		currentNode.node = content;
		currentNode.name = "root";
		return *this;
	}

	JSON_Parser &parent()
	{
		if (!previousNode.empty())
		{
			currentNode = previousNode.top();
			previousNode.pop();
		}

		return *this;
	}

	JSON_Parser &child(std::string_view name)
	{
		if (currentNode.name != "root")
		{
			previousNode.emplace(currentNode);
		}
		currentNode.name = name;
		auto startIndex = currentNode.node.find(name) + name.size() + 2;
	   
		currentNode.node = currentNode.node.substr(startIndex);
		currentNode.node.remove_prefix(currentNode.node.find_first_not_of(":{"));

		auto endIndex = IndexToMatchingBracers();

		currentNode.node = currentNode.node.substr(0, endIndex);
		is_child_array();
		return *this;
	}

	bool is_child_array()
	{
		if (currentNode.node.starts_with("["))
		{
			currentArray = currentNode.node;
			currentArray.remove_prefix(1);
			return true;
		}
		return false;
	}

	// I ask for forgiveness. I just woke up.
	void next_array_element()
	{
		is_child_array();
		if (currentArrayIndex == 1)
		{
			auto currentElementEnd = IndexToMatchingBracers(-1);
			currentArray.remove_suffix(currentElementEnd);
			currentElementEnd = IndexToMatchingBracers(-1);
			currentArray.remove_suffix(currentElementEnd);

			currentArray.remove_prefix(currentArray.find_first_of("{") + 1);
			currentArray.remove_suffix(currentArray.size() - currentArray.find_last_of("}"));
		}
		else if (currentArrayIndex == 2)
		{
			auto currentElementEnd = IndexToMatchingBracers(-1);
			currentArray.remove_prefix(currentElementEnd);
			currentElementEnd = IndexToMatchingBracers(-1);
			currentArray.remove_suffix(currentElementEnd);

			currentArray.remove_prefix(currentArray.find_first_of("{") + 1);
			currentArray.remove_suffix(currentArray.size() - currentArray.find_last_of("}"));
		}
		else if (currentArrayIndex == 3)
		{
			auto currentElementEnd = IndexToMatchingBracers(-1);
			currentArray.remove_prefix(currentElementEnd);
			currentElementEnd = IndexToMatchingBracers(-1);
			currentArray.remove_prefix(currentElementEnd);

			currentArray.remove_prefix(currentArray.find_first_of("{") + 1);
			currentArray.remove_suffix(currentArray.size() - currentArray.find_last_of("}"));

			if (currentArray.ends_with("}]"))
				currentArray.remove_suffix(2);
		}
		else
		{
			currentArrayIndex = 0;
		}

		currentArrayIndex++;
	}

	JSON_Parser &next_sibling()
	{
		if (!has_more_siblings())
			return *this;
			
		previousNode.emplace(currentNode);

		std::stack<NodeInfo> tree;
		while (!previousNode.empty())
		{
			tree.emplace(previousNode.top());
			previousNode.pop();
		}

		currentNode.node = content;
		currentNode.name = "root";

		while (tree.size() > 1)
		{
			child(tree.top().name);
			previousNode.emplace(tree.top());
			tree.pop();
		}
		auto startIndex = currentNode.node.find(tree.top().name);

		currentNode.node.remove_prefix(startIndex);

		auto endIndex = IndexToMatchingBracers(-1);

		currentNode.node.remove_prefix(endIndex);

		currentNode.node.remove_prefix(currentNode.node.find_first_of("\""));

		currentNode.name = currentNode.node.substr(0, currentNode.node.find_first_of(":"));
		currentNode.name.remove_prefix(1);
		currentNode.name.remove_suffix(1);

		return *this;

	}

	bool has_more_siblings() const
	{
		auto i = ReverseIndexToMatchingBracers(-1);
		std::string_view lastChildName = currentNode.node.substr(0, currentNode.node.size() - i);
		lastChildName.remove_suffix(lastChildName.size() - lastChildName.find_last_of("\""));
		lastChildName.remove_prefix(lastChildName.find_last_of("\"") + 1);
		return !(lastChildName == currentNode.name);
	}

	JSON_Parser &attribute(std::string_view name)
	{
		currentAttributeValue = currentNode.node.substr(currentNode.node.find(name) + name.size() + 2);

		currentAttributeValue.remove_prefix(currentNode.node.find_first_not_of(":{"));
		currentAttributeValue = currentAttributeValue.substr(0, currentAttributeValue.find_first_of(",{}"));

		return *this;
	}

	bool as_bool() const
	{
		return currentAttributeValue == "true";
	}

	int as_int()
	{
		int sign = 1;
		if (currentAttributeValue.starts_with('-'))
		{
			sign = -1;
			currentAttributeValue.remove_prefix(1);
		}

		int res = 0;
		for (auto const& num : currentAttributeValue)
		{
			res = res * 10 + (num - '0');
		}

		return res * sign;
	}

	std::string as_string() const
	{
		return std::string(currentAttributeValue);
	}

private:
   // Function read_file taken from https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c
   // Thanks to Konrad Rudolph for updating the answer for current C++
	void read_file(std::string_view path)
	{
		constexpr std::size_t read_size = 4096;
		auto stream = std::ifstream(path.data());
		stream.exceptions(std::ios_base::badbit);

		if (!stream) {
			throw std::ios_base::failure("File does not exist");
		}

		content = "";
		auto buf = std::string(read_size, '\0');

		while (stream.read(&buf[0], read_size)) {
			content.append(buf, 0, stream.gcount());
		}

		content.append(buf, 0, stream.gcount());

		stream.close();
	}

	int ReverseIndexToMatchingBracers(int startingIndex = 0) const
	{
		int endIndex = 0;

		for (int i = currentNode.node.size() - 1, openingBrackets = -2; i >= 0; i--)
		{
			endIndex++;
			if (currentNode.node[i] == '}')
			{
				openingBrackets++;
			}

			if (currentNode.node[i] == '{')
			{
				if (openingBrackets == 0)
					break;
				else
					openingBrackets--;
			}
			std::cout<< openingBrackets<< std::endl;
		}

		return endIndex;
	}

	int IndexToMatchingBracers(int startingIndex = 0) const
	{
		int endIndex = 0;

		for (int openingBrackets = startingIndex; auto const& elem : currentNode.node)
		{
			endIndex++;
			if (elem == '{')
			{
				openingBrackets++;
			}

			if (elem == '}')
			{
				if (openingBrackets == 0)
					break;
				else
					openingBrackets--;
			}
		}

		return endIndex;
	}

	struct NodeInfo
	{
		std::string_view node;
		std::string_view name;
	};

	std::string_view currentArray;
	int currentArrayIndex = 1;
	std::string content;
	NodeInfo currentNode;
	std::string_view currentAttributeValue;
	std::stack<NodeInfo> previousNode;
};