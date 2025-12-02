#include <iostream>
#include <vector>

using namespace std;

class GraphPathFinder {
private:
    vector<vector<int>> matrix;
    int vertexCount;
    int startVertex;
    int endVertex;
    vector<vector<int>> allPaths;

    void findAllPathsDFS(int current, vector<int>& path, vector<bool>& visited) {
        path.push_back(current);
        visited[current] = true;

        if (current == endVertex && path.size() == vertexCount) {
            allPaths.push_back(path);
        } else {
            for (int next = 0; next < vertexCount; next++) {
                if (matrix[current][next] == 1 && !visited[next]) {
                    findAllPathsDFS(next, path, visited);
                }
            }
        }

        path.pop_back();
        visited[current] = false;
    }

public:
    GraphPathFinder(const vector<vector<int>>& matrix, int start, int end)
        : matrix(matrix), startVertex(start), endVertex(end) {
        vertexCount = matrix.size();
    }

    void findAllPaths() {
        vector<int> path;
        vector<bool> visited(vertexCount, false);
        allPaths.clear();

        findAllPathsDFS(startVertex, path, visited);
    }

    void printResults() {
        cout << "Матрица смежности графа:" << endl;
        for (int i = 0; i < vertexCount; i++) {
            for (int j = 0; j < vertexCount; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;

        cout << "Входная вершина: " << startVertex << endl;
        cout << "Выходная вершина: " << endVertex << endl;
        cout << "Количество вершин: " << vertexCount << endl << endl;

        if (allPaths.empty()) {
            cout << "Не найдено маршрутов." << endl;
        } else {
            cout << "Найдено маршрутов: " << allPaths.size() << endl << endl;

            for (size_t i = 0; i < allPaths.size(); i++) {
                cout << "Маршрут " << (i + 1) << ": ";
                for (size_t j = 0; j < allPaths[i].size(); j++) {
                    cout << allPaths[i][j];
                    if (j < allPaths[i].size() - 1) {
                        cout << " -> ";
                    }
                }
                cout << endl;
            }
        }
        cout << endl;
    }
};

int main() {
    vector<vector<int>> graph1 = {
        {0, 1, 1, 0, 0},  // 0 -> 1 и 2
        {0, 0, 1, 1, 0},  // 1 -> 2 и 3
        {0, 0, 0, 1, 1},  // 2 -> 3 и 4
        {0, 0, 0, 0, 1},  // 3 -> 4
        {0, 0, 0, 0, 0}   // 4
    };
    GraphPathFinder test1(graph1, 0, 4);
    test1.findAllPaths();
    test1.printResults();

    vector<vector<int>> graph2 = {
        {0, 1, 1, 0, 0, 0},  // 0 -> 1, 2
        {0, 0, 0, 1, 0, 0},  // 1 -> 3
        {0, 1, 0, 1, 0, 0},  // 2 -> 1, 3
        {0, 0, 0, 0, 1, 0},  // 3 -> 4
        {0, 0, 0, 0, 0, 1},  // 4 -> 5
        {0, 0, 0, 0, 0, 0}   // 5
    };
    GraphPathFinder test2(graph2, 0, 5);
    test2.findAllPaths();
    test2.printResults();

    vector<vector<int>> graph3 = {
        {0, 1, 0, 0},  // 0 -> 1
        {0, 0, 1, 0},  // 1 -> 2
        {0, 0, 0, 0},  // 2 -> нет выхода
        {1, 0, 0, 0}   // 3 -> 0
    };
    GraphPathFinder test3(graph3, 0, 3);
    test3.findAllPaths();
    test3.printResults();

    vector<vector<int>> graph4 = {
        {0, 1, 1, 0},  // 0 -> 1, 2
        {0, 0, 1, 1},  // 1 -> 2, 3
        {0, 1, 0, 1},  // 2 -> 1, 3 (создает возможность разных путей)
        {0, 0, 0, 0}   // 3
    };
    GraphPathFinder test4(graph4, 0, 3);
    test4.findAllPaths();
    test4.printResults();

    vector<vector<int>> graph5 = {
        {0, 1, 1},  // 0 -> 1, 2
        {0, 0, 1},  // 1 -> 2
        {0, 0, 0}   // 2
    };
    GraphPathFinder test5(graph5, 0, 2);
    test5.findAllPaths();
    test5.printResults();

    return 0;
}
