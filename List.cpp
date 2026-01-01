#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <set>
#include <algorithm>

struct Operation {
    int id;
    int type;
    std::vector<int> successors;
    int startTime = -1;
    int endTime = -1;
    int assignedResource = -1;
};

struct Resource {
    int type;
    int count;
    int delay;
};

std::unordered_map<int, std::vector<int>> buildPredecessors(const std::vector<Operation>& ops) {
    std::unordered_map<int, std::vector<int>> pred;
    for (size_t i = 0; i < ops.size(); ++i) {
        const Operation& op = ops[i];
        for (size_t j = 0; j < op.successors.size(); ++j) {
            int succ = op.successors[j];
            pred[succ].push_back(op.id);
        }
    }
    return pred;
}

void listSchedule(std::vector<Operation>& ops, const std::unordered_map<int, Resource>& resources) {
    std::unordered_map<int, std::vector<int>> predecessors = buildPredecessors(ops);
    std::unordered_map<int, int> unscheduledDeps;
    for (std::unordered_map<int, std::vector<int>>::const_iterator it = predecessors.begin(); it != predecessors.end(); ++it) {
        unscheduledDeps[it->first] = it->second.size();
    }

    std::unordered_map<int, std::queue<int>> resourceQueues;
    for (std::unordered_map<int, Resource>::const_iterator it = resources.begin(); it != resources.end(); ++it) {
        resourceQueues[it->first] = std::queue<int>();
    }

    std::queue<int> readyQueue;
    std::unordered_map<int, Operation*> opMap;
    for (size_t i = 0; i < ops.size(); ++i) {
        opMap[ops[i].id] = &ops[i];
        if (unscheduledDeps.find(ops[i].id) == unscheduledDeps.end()) {
            readyQueue.push(ops[i].id);
        }
    }

    int currentTime = 1;
    std::vector<std::pair<int, int>> resourceInUse;

    while (!readyQueue.empty() || !resourceInUse.empty()) {
        for (std::vector<std::pair<int, int>>::iterator it = resourceInUse.begin(); it != resourceInUse.end();) {
            if (it->first <= currentTime) {
                Operation* doneOp = opMap[it->second];
                int resType = doneOp->type;
                if (resType != 0) {
                    resourceQueues[resType].push(1);
                }
                for (size_t j = 0; j < doneOp->successors.size(); ++j) {
                    int succ = doneOp->successors[j];
                    unscheduledDeps[succ]--;
                    if (unscheduledDeps[succ] == 0) {
                        readyQueue.push(succ);
                    }
                }
                it = resourceInUse.erase(it);
            } else {
                ++it;
            }
        }

        std::queue<int> tempQueue;
        while (!readyQueue.empty()) {
            int opId = readyQueue.front();
            readyQueue.pop();
            Operation* op = opMap[opId];

            if (op->type == 0) {
                op->startTime = currentTime;
                op->endTime = currentTime;
                op->assignedResource = -1;

                for (size_t j = 0; j < op->successors.size(); ++j) {
                    int succ = op->successors[j];
                    unscheduledDeps[succ]--;
                    if (unscheduledDeps[succ] == 0) {
                        tempQueue.push(succ);
                    }
                }
            } else {
                const Resource& res = resources.at(op->type);
                if (resourceQueues[res.type].size() < (size_t)res.count) {
                    op->startTime = currentTime;
                    op->endTime = currentTime + res.delay;
                    op->assignedResource = res.type;
                    resourceInUse.push_back(std::make_pair(op->endTime, opId));
                } else {
                    tempQueue.push(opId);
                }
            }
        }

        if (!tempQueue.empty()) {
            readyQueue = tempQueue;
        } else {
            currentTime++;
        }
    }

    std::cout << "\nOperation Schedule:\n";
    for (size_t i = 0; i < ops.size(); ++i) {
        const Operation& op = ops[i];
        std::cout << "OP " << op.id
                  << ": Start = " << op.startTime
                  << ", End = " << op.endTime
                  << ", Resource = " << (op.assignedResource == -1 ? "None" : std::to_string(op.assignedResource))
                  << "\n";
    }
}

std::vector<Operation> parseDFG(const std::string& filename) {
    std::vector<Operation> operations;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t colonPos = line.find(':');
        std::string left = line.substr(0, colonPos);
        std::string right = line.substr(colonPos + 1);

        size_t opPos = left.find("OP");
        size_t commaPos = left.find(',', opPos);
        int id = std::stoi(left.substr(opPos + 2, commaPos - opPos - 2));
        int type = std::stoi(left.substr(commaPos + 1));

        std::vector<int> successors;
        std::stringstream ss(right);
        std::string token;
        while (std::getline(ss, token, ',')) {
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            if (!token.empty()) {
                successors.push_back(std::stoi(token));
            }
        }

        Operation op;
        op.id = id;
        op.type = type;
        op.successors = successors;
        operations.push_back(op);
    }

    return operations;
}

std::unordered_map<int, Resource> parseResources(const std::string& filename) {
    std::unordered_map<int, Resource> resourceMap;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t spacePos = line.find(' ');
        std::string data = line.substr(spacePos + 1);

        std::stringstream ss(data);
        std::string token;
        std::vector<int> values;

        while (std::getline(ss, token, ',')) {
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
            values.push_back(std::stoi(token));
        }

        Resource res;
        res.type = values[0];
        res.count = values[1];
        res.delay = values[2];
        resourceMap[res.type] = res;
    }

    return resourceMap;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " DFG.txt Resource.txt\n";
        return 1;
    }

    std::vector<Operation> ops = parseDFG(argv[1]);
    std::unordered_map<int, Resource> resources = parseResources(argv[2]);

    listSchedule(ops, resources);
    return 0;
}
