#ifndef NAVIGATIONSERVICE_H
#define NAVIGATIONSERVICE_H

#include <string>
#include <vector>
#include "../data/DataStore.h"
#include "../dsa/Graph.h"
using namespace std;

class NavigationService
{
public:
    static vector<HospitalLocation>& getLocations()
    {
        return DataStore::getHospitalLocations();
    }

    static bool locationExists(int id)
    {
        for (const auto& l : DataStore::getHospitalLocations())
        {
            if (l.id == id)
                return true;
        }
        return false;
    }

    static string getLocationName(int id)
    {
        for (const auto& l : DataStore::getHospitalLocations())
        {
            if (l.id == id)
                return l.name;
        }
        return "Unknown location";
    }

    static Graph buildGraph()
    {
        auto& locations = DataStore::getHospitalLocations();

        int maxId = 0;
        for (const auto& l : locations)
        {
            if (l.id > maxId)
                maxId = l.id;
        }

        Graph graph(maxId + 1);
        vector<vector<bool>> added(maxId + 1, vector<bool>(maxId + 1, false));

        for (const auto& l : locations)
        {
            for (int next : l.connectedLocationIds)
            {
                if (next < 1 || next > maxId || next == l.id)
                    continue;
                if (!locationExists(next))
                    continue;
                if (added[l.id][next])
                    continue;

                graph.addEdge(l.id, next);
                added[l.id][next] = true;
                added[next][l.id] = true;
            }
        }

        return graph;
    }

    // BFS: shortest route (fewest corridors). Empty vector = no route.
    static vector<int> findRoute(int start, int destination)
    {
        if (!locationExists(start) || !locationExists(destination))
            return vector<int>();

        Graph graph = buildGraph();
        return graph.shortestPathBFS(start, destination);
    }

    // DFS: all locations reachable from start.
    static vector<int> getReachable(int start)
    {
        if (!locationExists(start))
            return vector<int>();

        Graph graph = buildGraph();
        return graph.DFSOrder(start);
    }
};

#endif