#ifndef __ADJACENCYGRAPH_H__
#define __ADJACENCYGRAPH_H__

#include <vector>
#include <format>

template <typename T>
class AdjacencyGraph
{
private:
	template <typename T>
	struct Vertex
	{
	public:
		explicit Vertex() = default;
		explicit Vertex(T name) : value(name) {};

		// Returns index of newly added destination
		int AddEdge(int destination)
		{
			// Check if destination is already on the array
			auto IsSameValue = [destination](int n) { return destination == n; };
			if (auto returnIt = std::ranges::find_if(edges, IsSameValue);
				returnIt != edges.end())
			{
				return std::distance(edges.begin(), returnIt);
			}

			// If it's not, add it
			edges.push_back(destination);
			return static_cast<int>(edges.size()) - 1;
		}

		T value;
		std::vector<int> edges;
	};

public:
	AdjacencyGraph() = default;
	~AdjacencyGraph() = default;

	// Add a vertex if it doesn't exist
	// Returns the position of the vertex
	virtual int AddVertex(T value) = 0;

	// Adds the destination in the origin vector
	// If any (or both) of the vertices don't exist, it creates them
	// Returns the index of the destination position in the origin vector
	virtual void AddEdge(T origin, T destination) = 0;

	// Returns reference to vertex in index n of graph.
	// Throws std::out_of_range when out of bounds.
	Vertex<T> &At(int n)
	{
		if(n >= 0 && n < graph.size())
			return graph[n];

		std::string errorMessage = std::format("Vertex with index {} does not exist in graph", n);
		throw std::out_of_range(errorMessage);
	}

	// Returns reference to value of vertex at index n of graph.
	// Throws std::out_of_range when out of bounds.
	T &ValueAt(int n)
	{
		if (n >= 0 && n < graph.size())
			return graph[n].value;

		std::string errorMessage = std::format("Vertex with index {} does not exist in graph", n);
		throw std::out_of_range(errorMessage);
	}

	// Returns iterator to element, iterator == graph.last() if no match
	std::vector<Vertex<T>>::iterator Find(T value)
	{
		auto IsSameValue = [value](Vertex<T> const& v) { return v.value == value; };

		if (auto returnIt = std::ranges::find_if(graph, IsSameValue);
			returnIt != graph.end())
		{
			return returnIt;
		}
		return graph.end();
	}

	size_t GetGraphSize() const
	{
		return graph.size();
	}

protected:

	// Returns index if action is on the array, -1 if no match
	int GetIndex(T value) const
	{
		auto IsSameValue = [value](Vertex<T> const& v) { return v.value == value; };

		if (auto result = std::ranges::find_if(graph, IsSameValue);
			result != graph.end())
		{
			return std::distance(graph.begin(), result);
		}

		return -1;
	}
	std::vector<Vertex<T>> graph;
};

// Directed Graph
template <typename T>
class DirectedGraph : public AdjacencyGraph<T>
{
public:
	DirectedGraph() = default;
	~DirectedGraph() = default;

	// Add a vertex if it doesn't exist
	// Returns the position of the vertex
	int AddVertex(T value)
	{
		if (auto i = this->GetIndex(value);
			i >= 0)
		{
			return i;
		}

		this->graph.emplace_back(value);
		return static_cast<int>(this->graph.size()) - 1;
	}

	// Adds the destination in the origin vector
	// If the any (or both) of the vertices don't exist, it creates them
	// Returns the index of the destination position in the origin vector
	void AddEdge(T origin, T destination)
	{
		auto originIndex = AddVertex(origin);
		auto destinationIndex = AddVertex(destination);
		
		// Add the edge to the Vertex array
		this->graph[originIndex].AddEdge(destinationIndex);
	}
};

// Undirected Graph
template <typename T>
class UndirectedGraph : public AdjacencyGraph<T>
{
public:
	UndirectedGraph() = default;
	~UndirectedGraph() = default;

	// Add a vertex if it doesn't exist
	// Returns the position of the vertex
	int AddVertex(T value)
	{
		if (auto i = this->GetIndex(value);
			i >= 0)
		{
			return i;
		}

		this->graph.emplace_back(value);
		return static_cast<int>(this->graph.size()) - 1;
	}

	// Adds the destination in the origin vector
	// If the any (or both) of the vertices don't exist, it creates them
	// Returns the index of the destination position in the origin vector
	void AddEdge(T origin, T destination)
	{
		auto originIndex = AddVertex(origin);
		auto destinationIndex = AddVertex(destination);

		this->graph[originIndex].AddEdge(destinationIndex);
		this->graph[destinationIndex].AddEdge(originIndex);
	}
};

#endif	//__ADJACENCYGRAPH_H__
