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


    int vertexCount() const
    {
        return (int)adj.size();
    }

    vector<int> shortestPathBFS(int start, int destination) const
    {
        vector<int> path;
        int n = (int)adj.size();

        if (start < 0 || destination < 0 || start >= n || destination >= n)
            return path;

        vector<bool> visited(n, false);
        vector<int> parent(n, -1);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty())
        {
            int current = q.front();
            q.pop();

            if (current == destination)
                break;

            for (int neighbor : adj[current])
            {
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    parent[neighbor] = current;
                    q.push(neighbor);
                }
            }
        }

        if (!visited[destination])
            return path;

        vector<int> backwards;
        for (int v = destination; v != -1; v = parent[v])
            backwards.push_back(v);

        for (int i = (int)backwards.size() - 1; i >= 0; i--)
            path.push_back(backwards[i]);

        return path;
    }

    void DFSCollect(int vertex, vector<bool>& visited, vector<int>& order) const
    {
        visited[vertex] = true;
        order.push_back(vertex);

        for (int neighbor : adj[vertex])
        {
            if (!visited[neighbor])
                DFSCollect(neighbor, visited, order);
        }
    }

     vector<int> DFSOrder(int start) const
    {
        vector<int> order;

        if (start < 0 || start >= (int)adj.size())
            return order;

        vector<bool> visited(adj.size(), false);
        DFSCollect(start, visited, order);
        return order;
    }
};

#endif
