#ifndef OFEC_GRAPH_H
#define OFEC_GRAPH_H

#include <list>
#include <vector>

namespace ofec {
    class Graph {
    public:
        Graph() {}
        void updateAdjByMatrix(const std::vector<std::vector<bool>> &mat);
        void setNumVertex(size_t num);
        void addEdge(int src, int dest);
        std::vector<int> getShortestPath(int src, int dest);

    private:
        std::vector<std::vector<int>> m_adj;

        bool BFS(int src, int dest, int v, std::vector<int> &pred, std::vector<int> &dist);
    };
}

#endif // !OFEC_GRAPH_H