#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

const int MAX_LINES = 1000;

// Função para remover linhas redundantes
void removeRedundantLines(vector<vector<int>> connections[], int &lines) {
    set<int> redundantLines;

    // Comparar todas as linhas
    for (int i = 1; i <= lines; ++i) {
        if (connections[i].empty() || find(redundantLines.begin(), redundantLines.end(), i) == redundantLines.end()) continue;

        for (int j = 1; j <= lines; ++j) {
            if (i == j || connections[j].empty() || find(redundantLines.begin(), redundantLines.end(), j) == redundantLines.end()) continue;

            // Verificar se todas as conexões de i estão contidas em j
            bool isRedundant = true;
            for (auto &conn : connections[i]) {
                if (find(connections[j].begin(), connections[j].end(), conn) == connections[j].end()) {
                    isRedundant = false;
                    break;
                }
            }

            if (isRedundant) {
                redundantLines.insert(i);
                break;
            }
        }
    }

    // Remover as linhas redundantes
    for (int line : redundantLines) {
        connections[line].clear();
    }

    lines -= redundantLines.size();
}

// Função para verificar conectividade entre as estações
bool areStationsConnected(const vector<vector<int>> &stationGraph, int stations) {
    vector<bool> visited(stations + 1, false);
    queue<int> q;

    // Começar a BFS a partir da estação 1
    q.push(1);
    visited[1] = true;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (int neighbor : stationGraph[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    // Verificar se todas as estações foram visitadas
    for (int i = 1; i <= stations; ++i) {
        if (!visited[i]) {
            return false; // Alguma estação não foi alcançada
        }
    }

    return true;
}

// Função para verificar conexões e se uma linha cobre todas as estações
int verify_connections(const map<int, set<int>> &stationToLines, int lines, int stations) {
    set<int> connectedStations;

    // Marcar as estações conectadas
    for (const auto &entry : stationToLines) {
        connectedStations.insert(entry.first); // Marcar a estação conectada
    }

    // Verificar se todas as estações estão conectadas
    if ((int)connectedStations.size() != stations) {
        return -1; // Alguma estação não está conectada
    }

    // Verificar se alguma linha cobre todas as estações
    for (int line = 1; line <= lines; ++line) {
        set<int> coveredStations;
        for (const auto &entry : stationToLines) {
            if (entry.second.count(line)) {
                coveredStations.insert(entry.first);
            }
        }

        if ((int)coveredStations.size() == stations) {
            return 1; // Uma linha cobre todas as estações
        }
    }

    return 0; // Todas as estações estão conectadas, mas nenhuma linha cobre todas
}

// Função para construir o grafo de linhas
void buildLineGraph(const map<int, set<int>> &stationToLines, int lines, map<int, set<int>> &lineGraph) {
    for (const auto &entry : stationToLines) {
        const auto &linesAtStation = entry.second;

        // Adicionar arestas entre linhas que passam pela mesma estação
        for (int line1 : linesAtStation) {
            for (int line2 : linesAtStation) {
                if (line1 != line2) {
                    lineGraph[line1].insert(line2);
                }
            }
        }
    }
}

// Função para BFS no grafo de linhas
int bfsLineGraph(const map<int, set<int>> &lineGraph, int startLine, int lines) {
    queue<pair<int, int>> q;  // {linha atual, número de trocas}
    vector<bool> visited(lines + 1, false);

    // Iniciar BFS a partir da linha de partida
    q.push({startLine, 0});
    visited[startLine] = true;

    while (!q.empty()) {
        pair<int, int> front = q.front();
        int currentLine = front.first;
        int currentSwaps = front.second;
        q.pop();

        // Expandir para linhas adjacentes no grafo
        for (int neighborLine : lineGraph.at(currentLine)) {
            if (!visited[neighborLine]) {
                visited[neighborLine] = true;
                q.push({neighborLine, currentSwaps + 1});

                // Retorna o número de trocas de linha assim que todas as linhas foram visitadas
                if (all_of(visited.begin() + 1, visited.end(), [](bool v) { return v; })) {
                    return currentSwaps + 1;
                }
            }
        }
    }

    return -1;  // Não foi possível visitar todas as linhas
}

int main() {
    int n, m, n_lines;
    cin >> n >> m >> n_lines;

    map<int, set<int>> stationToLines;
    vector<vector<int>> connections[MAX_LINES];
    vector<vector<int>> stationGraph(n + 1); // Grafo das estações

    // Leitura das entradas
    for (int i = 0; i < m; ++i) {
        int u, v, line;
        cin >> u >> v >> line;
        stationToLines[u].insert(line);
        stationToLines[v].insert(line);
        connections[line].push_back({u, v});

        // Construir o grafo de estações
        stationGraph[u].push_back(v);
        stationGraph[v].push_back(u);
    }

    // Verificar conectividade entre estações
    if (!areStationsConnected(stationGraph, n)) {
        cout << -1 << endl; // Alguma estação não está conectada
        return 0;
    }

    // Remover linhas redundantes
    removeRedundantLines(connections, n_lines);

    // Verificar conexões
    int result = verify_connections(stationToLines, n_lines, n);

    if (result == -1) {
        cout << -1 << endl;
        return 0;
    } else if (result == 1) {
        cout << 0 << endl;
        return 0;
    }

    // Construir o grafo de linhas
    map<int, set<int>> lineGraph;
    buildLineGraph(stationToLines, n_lines, lineGraph);

    // Verificar conectividade do grafo de linhas
    vector<int> results;
    for (int i = 1; i <= n_lines; ++i) {
        if (lineGraph.find(i) != lineGraph.end()) {
            results.push_back(bfsLineGraph(lineGraph, i, n_lines));
        }
    }

    int final_result = *max_element(results.begin(), results.end());
    cout << final_result << endl;

    return 0;
}