#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <limits>

using namespace std;

struct Net {
    vector<int> cells;
};

struct Cell {
    int gain = 0;
    bool locked = false;
    int partition = 0;
};

void compute_gains(vector<Cell>& cells, vector<Net>& nets, unordered_map<int, unordered_set<int>>& cell_to_nets) {
    for (size_t i = 0; i < cells.size(); ++i) {
        int gain = 0;
        for (int net_idx : cell_to_nets[i + 1]) {
            int F = 0, T = 0;
            for (int c : nets[net_idx].cells) {
                if (cells[c - 1].partition == cells[i].partition) F++;
                else T++;
            }
            if (F == 1) gain++;
            if (T == 0) gain--;
        }
        cells[i].gain = gain;
    }
}

void update_gains(vector<Cell>& cells, vector<Net>& nets, unordered_map<int, unordered_set<int>>& cell_to_nets, int moved_cell) {
    int moved_partition = cells[moved_cell].partition;
    int old_partition = 1 - moved_partition;

    for (int net_idx : cell_to_nets[moved_cell + 1]) {
        int F = 0, T = 0;
        int only_T_cell = -1, only_F_cell = -1;

        for (int c : nets[net_idx].cells) {
            if (cells[c - 1].partition == old_partition) {
                F++;
                only_F_cell = c - 1;
            } else {
                T++;
                only_T_cell = c - 1;
            }
        }

        if (T == 0) {
            for (int c : nets[net_idx].cells)
                if (!cells[c - 1].locked) cells[c - 1].gain++;
        } else if (T == 1 && only_T_cell != -1 && !cells[only_T_cell].locked) {
            cells[only_T_cell].gain--;
        }

        F--, T++;

        if (F == 0) {
            for (int c : nets[net_idx].cells)
                if (!cells[c - 1].locked) cells[c - 1].gain--;
        } else if (F == 1 && only_F_cell != -1 && !cells[only_F_cell].locked) {
            cells[only_F_cell].gain++;
        }
    }
}

void fm_partition(vector<Cell>& cells, vector<Net>& nets, int N) {
    unordered_map<int, unordered_set<int>> cell_to_nets;
    for (size_t i = 0; i < nets.size(); ++i)
        for (int c : nets[i].cells) cell_to_nets[c].insert(i);

    for (int i = 0; i < N; i++)
        cells[i].partition = i % 2;

    compute_gains(cells, nets, cell_to_nets);

    vector<list<int>> gain_buckets(2 * N + 1);
    vector<list<int>::iterator> positions(N);
    int max_gain = -N;

    for (int i = 0; i < N; i++) {
        gain_buckets[cells[i].gain + N].push_back(i);
        positions[i] = --gain_buckets[cells[i].gain + N].end();
        max_gain = max(max_gain, cells[i].gain);
    }

    vector<Cell> best_cells = cells;
    int best_cut_size = numeric_limits<int>::max();
    bool improved = true;

    while (improved) {
        improved = false;
        int move_count = 0;
        int current_cut_size = best_cut_size;
        
        while (move_count < N) {
            while (gain_buckets[max_gain + N].empty() && max_gain > -N) max_gain--;
            if (max_gain == -N) break;

            int best_move_cell = gain_buckets[max_gain + N].back();
            gain_buckets[max_gain + N].pop_back();

            cells[best_move_cell].partition = 1 - cells[best_move_cell].partition;
            cells[best_move_cell].locked = true;
            update_gains(cells, nets, cell_to_nets, best_move_cell);

            int cut_size = 0;
            for (const auto& net : nets) {
                unordered_set<int> partitions;
                for (int c : net.cells) partitions.insert(cells[c - 1].partition);
                if (partitions.size() > 1) cut_size++;
            }

            if (cut_size < best_cut_size) {
                best_cut_size = cut_size;
                best_cells = cells;
                improved = true;
            }
            move_count++;
        }
    }
    cells = best_cells;
}

int calculate_cut_size(const vector<Cell>& cells, const vector<Net>& nets) {
    int cut_size = 0;
    for (const auto& net : nets) {
        unordered_set<int> partitions;
        for (int c : net.cells) partitions.insert(cells[c - 1].partition);
        if (partitions.size() > 1) cut_size++;
    }
    return cut_size;
}

void write_output(const vector<Cell>& cells, int cut_size) {
    ofstream output("out.txt");
    output << "Partition A: ";
    for (size_t i = 0; i < cells.size(); ++i)
        if (cells[i].partition == 0) output << (i + 1) << " ";
    output << "\nPartition B: ";
    for (size_t i = 0; i < cells.size(); ++i)
        if (cells[i].partition == 1) output << (i + 1) << " ";
    output << "\nThe cut size is " << cut_size << ".\n";
    output.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: FM-partition <num_vertices> <num_nets> <input_file>\n";
        return 1;
    }

    int num_vertices = atoi(argv[1]);
    int num_nets = atoi(argv[2]);
    string filename = argv[3];

    vector<Cell> cells(num_vertices);
    vector<Net> nets(num_nets);

    ifstream input(filename);
    if (!input) {
        cerr << "Error: Unable to open file " << filename << "\n";
        return 1;
    }

    string line;
    int net_index = 0;
    while (getline(input, line)) {
        if (!line.empty()) {
            size_t colon_pos = line.find(":");
            if (colon_pos != string::npos) {
                string net_part = line.substr(colon_pos + 1);
                while (!net_part.empty()) {
                    size_t pos = net_part.find(",");
                    nets[net_index].cells.push_back(stoi(net_part.substr(0, pos)));
                    if (pos == string::npos) break;
                    net_part.erase(0, pos + 1);
                }
                net_index++;
            }
        }
    }
    input.close();

    fm_partition(cells, nets, num_vertices);
    int cut_size = calculate_cut_size(cells, nets);
    write_output(cells, cut_size);
    cout << "Output written to out.txt\n";
    return 0;
}