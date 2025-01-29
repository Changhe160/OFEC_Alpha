
#pragma once
#include <vector>

namespace custom_fun {

    class VisitedLazyList {
    public:
        VisitedLazyList() = default;
        VisitedLazyList(int size) :visited(size, 0) {}
        ~VisitedLazyList() = default;

        void initialize() {
            mark_ = 1;
            visited.clear();
        }
        void resize(int numSize) {
            visited.resize(numSize);
        }
        void insert(int number = 1) {
            visited.resize(visited.size() + number, 0);
        }
        inline bool Visited(unsigned int index) const { return visited[index] == mark_; }
        inline bool NotVisited(unsigned int index) const { return visited[index] != mark_; }
        inline void MarkAsVisited(unsigned int index) {
            visited[index] = mark_;
        }
        inline void Reset() {
            if (++mark_ == 0) {
                mark_ = 1;
                std::fill(visited.begin(), visited.end(), 0);
                //memset(visited_, 0, sizeof(unsigned int) * size_);
            }
        }
     //   inline unsigned int* GetVisited() { return visited_; }
        inline unsigned long long GetVisitMark() { return mark_; }

    private:
        std::vector<unsigned long long> visited;
        unsigned long long mark_ = 0;
    };

} // namespace n2
