#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>

void topologicalSort(int vertices, const std::string& filename) {
    // Read the adjacency matrix from the file
    std::vector<std::vector<int>> adjMatrix(vertices, std::vector<int>(vertices, 0));
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    for (int i = 0; i < vertices; ++i) {
        for (int j = 0; j < vertices; ++j) {
            inputFile >> adjMatrix[i][j];
        }
    }
    inputFile.close();

    // Calculate in-degrees of all vertices
    std::vector<int> inDegree(vertices, 0);
    for (int i = 0; i < vertices; ++i) {
        for (int j = 0; j < vertices; ++j) {
            if (adjMatrix[i][j] != 0) {
                inDegree[j]++;
            }
        }
    }

    // Queue for vertices with no incoming edges
    std::queue<int> zeroInDegreeQueue;
    for (int i = 0; i < vertices; ++i) {
        if (inDegree[i] == 0) {
            zeroInDegreeQueue.push(i);
        }
    }

    // List to store the topological order
    std::vector<int> topologicalOrder;

    // Process vertices with zero in-degree
    while (!zeroInDegreeQueue.empty()) {
        int vertex = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        topologicalOrder.push_back(vertex);

        // Decrease in-degree of adjacent vertices
        for (int i = 0; i < vertices; ++i) {
            if (adjMatrix[vertex][i] != 0) {
                inDegree[i]--;
                // If in-degree becomes zero, add to queue
                if (inDegree[i] == 0) {
                    zeroInDegreeQueue.push(i);
                }
            }
        }
    }

    // Check if topological sort includes all vertices
    if (topologicalOrder.size() != vertices) {
        std::cerr << "Error: The graph contains a cycle." << std::endl;
    } else {
        // Write the topological order to "out.txt"
        std::ofstream outputFile("out.txt");
        if (!outputFile) {
            std::cerr << "Error: Unable to create output file." << std::endl;
            return;
        }
        for (int i = 0; i < topologicalOrder.size(); ++i) {
            outputFile << topologicalOrder[i] + 1 << std::endl; // +1 to match 1-based indexing
        }
        outputFile.close();
        std::cout << "Topological sorting completed. Check 'out.txt' for the result." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_vertices> <adjacency_matrix_file>" << std::endl;
        return 1;
    }

    int vertices = std::stoi(argv[1]);
    std::string filename = argv[2];

    topologicalSort(vertices, filename);

    return 0;
}
