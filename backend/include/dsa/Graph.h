#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Graph
{
private:
    vector<vector<int>> adj;

public:
    Graph(int vertices)
    {
        adj.resize(vertices);
    }

    void addEdge(int source, int destination)
    {
        adj[source].push_back(destination);
        adj[destination].push_back(source);
    }

    void BFS(int start)
    {
        vector<bool> visited(adj.size(), false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty())
        {
            int current = q.front();
            q.pop();

            cout << current << " ";

            for (int neighbor : adj[current])
            {
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }

        cout << endl;
    }

    void DFSHelper(int vertex, vector<bool>& visited)
    {
        visited[vertex] = true;
        cout << vertex << " ";

        for (int neighbor : adj[vertex])
        {
            if (!visited[neighbor])
                DFSHelper(neighbor, visited);
        }
    }

    void DFS(int start)
    {
        vector<bool> visited(adj.size(), false);
        DFSHelper(start, visited);
        cout << endl;
    }
};

#endif
