#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

bool dominates(const vector<double>& a, const vector<double>& b) {
    bool better = false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] > b[i]) return false;
        if (a[i] < b[i]) better = true;
    }
    return better;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <file_name> <M> <N>" << endl;
        return 1;
    }

    string file_name = argv[1];
    int M = stoi(argv[2]);
    int N = stoi(argv[3]);

    ifstream infile(file_name);
    if (!infile) {
        cerr << "Unable to open file " << file_name << endl;
        return 1;
    }

    vector<vector<double>> points;
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        vector<double> point;
        string value;
        while (getline(ss, value, ',')) {
            point.push_back(stod(value));
        }
        if (point.size() == M) {
            points.push_back(point);
        } else {
            cerr << "Points.txt does not have " << M << " metrics." << endl;
            return 1;
        }
    }
    infile.close();

    vector<vector<double>> pareto_front;
    for (size_t i = 0; i < points.size(); ++i) {
        bool is_dominated = false;
        for (size_t j = 0; j < points.size(); ++j) {
            if (i != j && dominates(points[j], points[i])) {
                is_dominated = true;
                break;
            }
        }
        if (!is_dominated) {
            pareto_front.push_back(points[i]);
        }
    }

    ofstream outfile("out.txt");
    if (!outfile) {
        cerr << "Unable to create out.txt file." << endl;
        return 1;
    }

    for (const auto& point : pareto_front) {
        for (size_t i = 0; i < point.size(); ++i) {
            outfile << point[i];
            if (i < point.size() - 1) outfile << ",";
        }
        outfile << endl;
    }
    outfile.close();

    return 0;
}