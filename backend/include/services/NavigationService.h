#ifndef NAVIGATIONSERVICE_H
#define NAVIGATIONSERVICE_H

#include "../data/DataStore.h"
#include "../dsa/Graph.h"

class NavigationService
{
public:
    static vector<HospitalLocation>& getLocations()
    {
        return DataStore::getHospitalLocations();
    }

    static vector<int> findRoute(int start, int destination)
    {
        auto& locations = DataStore::getHospitalLocations();

        vector<int> route;
        vector<int> parent(locations.size() + 1, -1);
        vector<bool> visited(locations.size() + 1, false);
        queue<int> q;

        if (start < 1 || destination < 1 ||
            start >= parent.size() || destination >= parent.size())
            return route;

        q.push(start);
        visited[start] = true;

        while (!q.empty())
        {
            int current = q.front();
            q.pop();

            if (current == destination)
                break;

            for (const auto& location : locations)
            {
                if (location.id == current)
                {
                    for (int next : location.connectedLocationIds)
                    {
                        if (next > 0 && next < parent.size() && !visited[next])
                        {
                            visited[next] = true;
                            parent[next] = current;
                            q.push(next);
                        }
                    }
                }
            }
        }

        if (!visited[destination])
            return route;

        for (int current = destination; current != -1; current = parent[current])
            route.push_back(current);

        reverse(route.begin(), route.end());

        return route;
    }
};

#endif
