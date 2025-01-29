#include "graph.h"
#include "../../core/exception.h"

namespace ofec {
    void Graph::updateAdjByMatrix(const std::vector<std::vector<bool>>& mat) {
        m_adj.clear();
        m_adj.resize(mat.size());
        for (int i = 0; i < mat.size(); ++i) {
            for (int j = 0; j < mat[i].size(); ++j) {
                if (mat[i][j]) {
                    m_adj[i].push_back(j);
                }
            }
        }
    }

    void Graph::setNumVertex(size_t num) {
        m_adj.clear();
        m_adj.resize(num);
    }

    void Graph::addEdge(int src, int dest) {
        m_adj[src].push_back(dest);
        m_adj[dest].push_back(src);
    }

    std::vector<int> Graph::getShortestPath(int src, int dest) {
        size_t num = m_adj.size();
        std::vector<int> pred(num), dist(num);
        if (BFS(src, dest, m_adj.size(), pred, dist) == false) {
            throw Exception("Given source and destination are not connected");
        }
        std::vector<int> path;
        int crawl = dest;
        path.push_back(crawl);
        while (pred[crawl] != -1) {
            path.push_back(pred[crawl]);
            crawl = pred[crawl];
        }
        return path;
    }
	
	bool Graph::BFS(int src, int dest, int v, std::vector<int> &pred, std::vector<int> &dist) {
        // a queue to maintain queue of vertices whose
           // adjacency list is to be scanned as per normal
           // DFS algorithm
        std::list<int> queue;

        // boolean array visited[] which stores the
        // information whether ith vertex is reached
        // at least once in the Breadth first search
        std::vector<bool> visited(v);

        // initially all vertices are unvisited
        // so v[i] for all i is false
        // and as no path is yet constructed
        // dist[i] for all i set to infinity
        for (int i = 0; i < v; i++) {
            visited[i] = false;
            dist[i] = 100000;
            pred[i] = -1;
        }

        // now source is first to be visited and
        // distance from source to itself should be 0
        visited[src] = true;
        dist[src] = 0;
        queue.push_back(src);

        // standard BFS algorithm
        while (!queue.empty()) {
            int u = queue.front();
            queue.pop_front();
            for (int i = 0; i < m_adj[u].size(); i++) {
                if (visited[m_adj[u][i]] == false) {
                    visited[m_adj[u][i]] = true;
                    dist[m_adj[u][i]] = dist[u] + 1;
                    pred[m_adj[u][i]] = u;
                    queue.push_back(m_adj[u][i]);

                    // We stop BFS when we find
                    // destination.
                    if (m_adj[u][i] == dest)
                        return true;
                }
            }
        }

        return false;
	}
}