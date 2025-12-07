// Food Delivery Simulator - Windows Native GUI
// Compile: g++ -o delivery.exe project.cpp -mwindows -lgdi32 -lcomdlg32

#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
const int INF = 999999;

// ============ DATA STRUCTURES (same as before) ============
template <typename T>
class Node {
public:
    T data;
    Node *next;
    Node(T val) : data(val), next(nullptr) {}
};

template <typename T>
class LinkedList {
private:
    Node<T> *head;
    int size;
public:
    LinkedList() : head(nullptr), size(0) {}
    void insert(T data) {
        Node<T> *newNode = new Node<T>(data);
        if (!head) head = newNode;
        else {
            Node<T> *temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
        size++;
    }
    Node<T> *getHead() { return head; }
    int getSize() { return size; }
    T *find(int id) {
        Node<T> *temp = head;
        while (temp) {
            if (temp->data.id == id) return &(temp->data);
            temp = temp->next;
        }
        return nullptr;
    }
    ~LinkedList() {
        Node<T> *temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }
};

template <typename T>
class Queue {
private:
    Node<T> *front, *rear;
    int size;
public:
    Queue() : front(nullptr), rear(nullptr), size(0) {}
    void enqueue(T data) {
        Node<T> *newNode = new Node<T>(data);
        if (!rear) front = rear = newNode;
        else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }
    T dequeue() {
        if (!front) throw runtime_error("Queue is empty");
        Node<T> *temp = front;
        T data = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        size--;
        return data;
    }
    bool isEmpty() { return front == nullptr; }
    int getSize() { return size; }
    ~Queue() { while (!isEmpty()) dequeue(); }
};

template <typename T>
class Stack {
private:
    Node<T> *top;
    int size;
public:
    Stack() : top(nullptr), size(0) {}
    void push(T data) {
        Node<T> *newNode = new Node<T>(data);
        newNode->next = top;
        top = newNode;
        size++;
    }
    T pop() {
        if (!top) throw runtime_error("Stack is empty");
        Node<T> *temp = top;
        T data = top->data;
        top = top->next;
        delete temp;
        size--;
        return data;
    }
    bool isEmpty() { return top == nullptr; }
    int getSize() { return size; }
    ~Stack() { while (!isEmpty()) pop(); }
};

template <typename T>
class PriorityQueue {
private:
    struct PQNode {
        T data;
        int priority;
        PQNode *next;
        PQNode(T d, int p) : data(d), priority(p), next(nullptr) {}
    };
    PQNode *head;
    int size;
public:
    PriorityQueue() : head(nullptr), size(0) {}
    void enqueue(T data, int priority) {
        PQNode *newNode = new PQNode(data, priority);
        if (!head || priority > head->priority) {
            newNode->next = head;
            head = newNode;
        } else {
            PQNode *temp = head;
            while (temp->next && temp->next->priority >= priority) temp = temp->next;
            newNode->next = temp->next;
            temp->next = newNode;
        }
        size++;
    }
    T dequeue() {
        if (!head) throw runtime_error("Priority Queue is empty");
        PQNode *temp = head;
        T data = head->data;
        head = head->next;
        delete temp;
        size--;
        return data;
    }
    bool isEmpty() { return head == nullptr; }
    int getSize() { return size; }
    ~PriorityQueue() { while (!isEmpty()) dequeue(); }
};

struct Restaurant {
    int id;
    string name;
    int nodeId;
    Restaurant() : id(0), name(""), nodeId(0) {}
    Restaurant(int i, string n, int nid) : id(i), name(n), nodeId(nid) {}
};

class BSTNode {
public:
    Restaurant data;
    BSTNode *left, *right;
    BSTNode(Restaurant r) : data(r), left(nullptr), right(nullptr) {}
};

class BST {
private:
    BSTNode *root;
    BSTNode *insertHelper(BSTNode *node, Restaurant r) {
        if (!node) return new BSTNode(r);
        if (r.name < node->data.name) node->left = insertHelper(node->left, r);
        else node->right = insertHelper(node->right, r);
        return node;
    }
    void destroyTree(BSTNode *node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
    void collectRestaurants(BSTNode *node, vector<Restaurant>& list) {
        if (node) {
            collectRestaurants(node->left, list);
            list.push_back(node->data);
            collectRestaurants(node->right, list);
        }
    }
public:
    BST() : root(nullptr) {}
    void insert(Restaurant r) { root = insertHelper(root, r); }
    vector<Restaurant> getAllRestaurants() {
        vector<Restaurant> list;
        collectRestaurants(root, list);
        return list;
    }
    ~BST() { destroyTree(root); }
};

struct Edge {
    int dest, weight;
    string streetName;
    Edge(int d, int w, string name = "") : dest(d), weight(w), streetName(name) {}
};

struct NodeInfo {
    int id;
    string name, type;
    NodeInfo() : id(0), name(""), type("") {}
    NodeInfo(int i, string n, string t) : id(i), name(n), type(t) {}
};

class Graph {
private:
    int vertices;
    LinkedList<Edge> *adjList;
    vector<NodeInfo> nodeInfo;
public:
    Graph(int v) : vertices(v) {
        adjList = new LinkedList<Edge>[v];
        nodeInfo.resize(v);
    }
    friend class FoodDeliverySimulator;
    
    void setNodeInfo(int id, string name, string type) {
        if (id >= 0 && id < vertices) nodeInfo[id] = NodeInfo(id, name, type);
    }
    string getNodeName(int id) {
        if (id >= 0 && id < vertices) return nodeInfo[id].name;
        return "Unknown";
    }
    string getNodeType(int id) {
        if (id >= 0 && id < vertices) return nodeInfo[id].type;
        return "unknown";
    }
    void addEdge(int src, int dest, int weight, string streetName = "") {
        adjList[src].insert(Edge(dest, weight, streetName));
        adjList[dest].insert(Edge(src, weight, streetName));
    }
    pair<vector<int>, vector<int>> dijkstraWithPath(int source, int destination) {
        vector<int> dist(vertices, INF);
        vector<int> parent(vertices, -1);
        vector<bool> visited(vertices, false);
        dist[source] = 0;
        
        for (int count = 0; count < vertices - 1; count++) {
            int u = -1, minDist = INF;
            for (int i = 0; i < vertices; i++) {
                if (!visited[i] && dist[i] < minDist) {
                    minDist = dist[i];
                    u = i;
                }
            }
            if (u == -1) break;
            visited[u] = true;
            
            Node<Edge> *temp = adjList[u].getHead();
            while (temp) {
                int v = temp->data.dest;
                int weight = temp->data.weight;
                if (!visited[v] && dist[u] != INF && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                }
                temp = temp->next;
            }
        }
        return make_pair(dist, parent);
    }
    vector<int> dijkstra(int source) {
        vector<int> dist(vertices, INF);
        vector<bool> visited(vertices, false);
        dist[source] = 0;
        
        for (int count = 0; count < vertices - 1; count++) {
            int u = -1, minDist = INF;
            for (int i = 0; i < vertices; i++) {
                if (!visited[i] && dist[i] < minDist) {
                    minDist = dist[i];
                    u = i;
                }
            }
            if (u == -1) break;
            visited[u] = true;
            
            Node<Edge> *temp = adjList[u].getHead();
            while (temp) {
                int v = temp->data.dest;
                int weight = temp->data.weight;
                if (!visited[v] && dist[u] != INF && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                }
                temp = temp->next;
            }
        }
        return dist;
    }
    LinkedList<Edge>* getAdjList() { return adjList; }
    int getVertices() { return vertices; }
    ~Graph() { delete[] adjList; }
};

struct User {
    int id;
    string name;
    int nodeId;
    User() : id(0), name(""), nodeId(0) {}
    User(int i, string n, int nid) : id(i), name(n), nodeId(nid) {}
};

struct Agent {
    int id;
    string name;
    int nodeId;
    bool available;
    Agent() : id(0), name(""), nodeId(0), available(true) {}
    Agent(int i, string n, int nid) : id(i), name(n), nodeId(nid), available(true) {}
};

struct Order {
    int orderId, userId, restaurantId, agentId, priority;
    string status;
    Order() : orderId(0), userId(0), restaurantId(0), agentId(-1), priority(0), status("Pending") {}
    Order(int oid, int uid, int rid, int p) : orderId(oid), userId(uid), restaurantId(rid), agentId(-1), priority(p), status("Pending") {}
};

struct Assignment {
    int orderId, agentId;
    Assignment() : orderId(0), agentId(0) {}
    Assignment(int oid, int aid) : orderId(oid), agentId(aid) {}
};

class FoodDeliverySimulator {
private:
    Graph *graph;
    LinkedList<User> users;
    LinkedList<Agent> agents;
    BST restaurants;
    LinkedList<Order> orderHistory;
    PriorityQueue<Order> urgentOrders;
    Queue<Order> backlogOrders;
    Stack<Assignment> assignmentHistory;
    int nextOrderId, nextUserId, numNodes;
    
public:
    FoodDeliverySimulator() : graph(nullptr), nextOrderId(1), nextUserId(1), numNodes(0) {
        loadGraph("nodes.csv", "edges.csv");
        loadRestaurants("restaurants.csv");
        clearUsersFile();
        loadAgents("agents.csv");
        loadOrderHistory("orders.csv");
    }
    ~FoodDeliverySimulator() {
        saveOrderHistory("orders.csv");
        if (graph) delete graph;
    }
    
    void loadGraph(string nodesFile, string edgesFile) {
        ifstream nodeIn(nodesFile);
        if (!nodeIn) return;
        string line;
        getline(nodeIn, line);
        numNodes = 0;
        vector<string> nodeNames, nodeTypes;
        
        while (getline(nodeIn, line)) {
            stringstream ss(line);
            string id, name, type;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, type, ',');
            nodeNames.push_back(name);
            nodeTypes.push_back(type);
            numNodes++;
        }
        nodeIn.close();
        
        graph = new Graph(numNodes);
        for (int i = 0; i < numNodes; i++) {
            graph->setNodeInfo(i, nodeNames[i], nodeTypes[i]);
        }
        
        ifstream edgeIn(edgesFile);
        if (!edgeIn) return;
        getline(edgeIn, line);
        
        while (getline(edgeIn, line)) {
            stringstream ss(line);
            string src, dest, weight, streetName;
            getline(ss, src, ',');
            getline(ss, dest, ',');
            getline(ss, weight, ',');
            getline(ss, streetName, ',');
            graph->addEdge(stoi(src), stoi(dest), stoi(weight), streetName);
        }
        edgeIn.close();
    }
    
    void loadRestaurants(string filename) {
        ifstream file(filename);
        if (!file) return;
        string line;
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, nodeId;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, nodeId, ',');
            restaurants.insert(Restaurant(stoi(id), name, stoi(nodeId)));
        }
        file.close();
    }
    
    void clearUsersFile() {
        ofstream file("users.csv", ios::trunc);
        if (file) {
            file << "id,name,nodeId\n";
            file.close();
        }
    }
    
    void loadAgents(string filename) {
        ifstream file(filename);
        if (!file) return;
        string line;
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, nodeId;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, nodeId, ',');
            agents.insert(Agent(stoi(id), name, stoi(nodeId)));
        }
        file.close();
    }
    
    void loadOrderHistory(string filename) {
        ifstream file(filename);
        if (!file) {
            // File doesn't exist, nextOrderId stays at 1
            return;
        }
        string line;
        getline(file, line);
        
        bool hasOrders = false;
        while (getline(file, line)) {
            if (line.empty()) continue;
            hasOrders = true;
            stringstream ss(line);
            string orderId, userId, restaurantId, agentId, priority, status;
            getline(ss, orderId, ',');
            getline(ss, userId, ',');
            getline(ss, restaurantId, ',');
            getline(ss, agentId, ',');
            getline(ss, priority, ',');
            getline(ss, status, ',');
            Order order;
            order.orderId = stoi(orderId);
            order.userId = stoi(userId);
            order.restaurantId = stoi(restaurantId);
            order.agentId = stoi(agentId);
            order.priority = stoi(priority);
            order.status = status;
            orderHistory.insert(order);
            if (order.orderId >= nextOrderId) nextOrderId = order.orderId + 1;
        }
        
        // If no orders in history, ensure nextOrderId starts at 1
        if (!hasOrders) {
            nextOrderId = 1;
        }
        file.close();
    }
    
    void saveOrderHistory(string filename) {
        ofstream file(filename);
        if (!file) return;
        file << "orderId,userId,restaurantId,agentId,priority,status" << endl;
        Node<Order> *temp = orderHistory.getHead();
        while (temp) {
            file << temp->data.orderId << "," << temp->data.userId << "," << temp->data.restaurantId << "," 
                 << temp->data.agentId << "," << temp->data.priority << "," << temp->data.status << endl;
            temp = temp->next;
        }
        file.close();
    }
    
    int addUser(string name, int nodeId) {
        int userId = nextUserId++;
        users.insert(User(userId, name, nodeId));
        ofstream file("users.csv", ios::app);
        if (file) {
            file << userId << "," << name << "," << nodeId << "\n";
            file.close();
        }
        return userId;
    }
    
    string createOrder(string userName, int nodeId, int restaurantId, int priority) {
        if (nodeId != 1 && nodeId != 3 && nodeId != 5 && nodeId != 7 && nodeId != 8) {
            return "Error: Invalid delivery point!";
        }
        if (restaurantId < 1 || restaurantId > 5) {
            return "Error: Invalid restaurant ID!";
        }
        int userId = addUser(userName, nodeId);
        Order order(nextOrderId++, userId, restaurantId, priority);
        if (priority >= 5) {
            urgentOrders.enqueue(order, priority);
            return "Urgent order #" + to_string(order.orderId) + " created successfully!";
        } else {
            backlogOrders.enqueue(order);
            return "Regular order #" + to_string(order.orderId) + " created successfully!";
        }
    }
    
    Agent *findNearestAgent(int restaurantNode) {
        if (!graph) return nullptr;
        vector<int> distances = graph->dijkstra(restaurantNode);
        Agent *nearestAgent = nullptr;
        int minDist = INF;
        Node<Agent> *temp = agents.getHead();
        while (temp) {
            if (temp->data.available && distances[temp->data.nodeId] < minDist) {
                minDist = distances[temp->data.nodeId];
                nearestAgent = &(temp->data);
            }
            temp = temp->next;
        }
        return nearestAgent;
    }
    
    struct DispatchResult {
        bool success;
        int orderId;
        string customerName, restaurantName, agentName;
        int distance, eta;
        vector<int> path;
    };
    
    vector<DispatchResult> dispatchOrders() {
        vector<DispatchResult> results;
        if (!graph) return results;
        
        // Process urgent orders first
        while (!urgentOrders.isEmpty()) {
            Order order = urgentOrders.dequeue();
            User *user = users.find(order.userId);
            if (!user) continue;
            
            int restaurantNode = order.restaurantId - 1;
            Agent *agent = findNearestAgent(restaurantNode);
            
            if (agent) {
                order.agentId = agent->id;
                order.status = "Assigned";
                agent->available = false;
                
                auto result = graph->dijkstraWithPath(restaurantNode, user->nodeId);
                vector<int> distances = result.first;
                vector<int> parent = result.second;
                int distance = distances[user->nodeId];
                int eta = (distance * 60) / 30;
                
                vector<int> path;
                int current = user->nodeId;
                while (current != -1) {
                    path.push_back(current);
                    current = parent[current];
                }
                reverse(path.begin(), path.end());
                
                DispatchResult dr;
                dr.success = true;
                dr.orderId = order.orderId;
                dr.customerName = user->name;
                dr.restaurantName = graph->getNodeName(restaurantNode);
                dr.agentName = agent->name;
                dr.distance = distance;
                dr.eta = eta;
                dr.path = path;
                results.push_back(dr);
                
                orderHistory.insert(order);
                assignmentHistory.push(Assignment(order.orderId, agent->id));
            } else {
                backlogOrders.enqueue(order);
            }
        }
        
        // Process backlog orders (ALL of them, not just 5)
        Queue<Order> tempQueue;
        
        while (!backlogOrders.isEmpty()) {
            Order order = backlogOrders.dequeue();
            User *user = users.find(order.userId);
            if (!user) {
                tempQueue.enqueue(order);
                continue;
            }
            
            int restaurantNode = order.restaurantId - 1;
            Agent *agent = findNearestAgent(restaurantNode);
            
            if (agent) {
                order.agentId = agent->id;
                order.status = "Assigned";
                agent->available = false;
                
                auto result = graph->dijkstraWithPath(restaurantNode, user->nodeId);
                vector<int> distances = result.first;
                vector<int> parent = result.second;
                int distance = distances[user->nodeId];
                int eta = (distance * 60) / 30;
                
                vector<int> path;
                int current = user->nodeId;
                while (current != -1) {
                    path.push_back(current);
                    current = parent[current];
                }
                reverse(path.begin(), path.end());
                
                DispatchResult dr;
                dr.success = true;
                dr.orderId = order.orderId;
                dr.customerName = user->name;
                dr.restaurantName = graph->getNodeName(restaurantNode);
                dr.agentName = agent->name;
                dr.distance = distance;
                dr.eta = eta;
                dr.path = path;
                results.push_back(dr);
                
                orderHistory.insert(order);
                assignmentHistory.push(Assignment(order.orderId, agent->id));
            } else {
                // No agent available, put back in queue
                tempQueue.enqueue(order);
            }
        }
        
        // Put unprocessed orders back
        while (!tempQueue.isEmpty()) {
            backlogOrders.enqueue(tempQueue.dequeue());
        }
        
        if (!results.empty()) {
            saveOrderHistory("orders.csv");
        }
        return results;
    }
    
    string undoLastAssignment() {
        if (assignmentHistory.isEmpty()) return "No assignments to undo.";
        Assignment lastAssignment = assignmentHistory.pop();
        Agent *agent = agents.find(lastAssignment.agentId);
        if (agent) {
            agent->available = true;
            return "Undo successful! Agent " + agent->name + " is now available.";
        }
        return "Agent not found.";
    }
    
    vector<string> getOrderHistory() {
        vector<string> history;
        
        // First show dispatched orders from history
        Node<Order> *temp = orderHistory.getHead();
        while (temp) {
            string line = "Order #" + to_string(temp->data.orderId) + " | User: " + to_string(temp->data.userId) +
                         " | Rest: " + to_string(temp->data.restaurantId) + " | Agent: " + to_string(temp->data.agentId) +
                         " | Priority: " + to_string(temp->data.priority) + " | " + temp->data.status;
            history.push_back(line);
            temp = temp->next;
        }
        
        return history;
    }
    
    vector<string> getAllOrders() {
        vector<string> allOrders;
        
        // Get dispatched orders
        Node<Order> *temp = orderHistory.getHead();
        while (temp) {
            string line = "[DISPATCHED] Order #" + to_string(temp->data.orderId) + 
                         " | User: " + to_string(temp->data.userId) +
                         " | Restaurant: " + to_string(temp->data.restaurantId) + 
                         " | Agent: " + to_string(temp->data.agentId) +
                         " | Priority: " + to_string(temp->data.priority) + 
                         " | Status: " + temp->data.status;
            allOrders.push_back(line);
            temp = temp->next;
        }
        
        return allOrders;
    }
    
    string getPendingOrdersInfo() {
        string info = "Urgent Orders: " + to_string(urgentOrders.getSize()) + "\n";
        info += "Backlog Orders: " + to_string(backlogOrders.getSize());
        return info;
    }
    
    vector<Restaurant> getAllRestaurants() { return restaurants.getAllRestaurants(); }
    
    vector<Agent> getAllAgents() {
        vector<Agent> agentList;
        Node<Agent> *temp = agents.getHead();
        while (temp) {
            agentList.push_back(temp->data);
            temp = temp->next;
        }
        return agentList;
    }
    
    vector<User> getAllUsers() {
        vector<User> userList;
        Node<User> *temp = users.getHead();
        while (temp) {
            userList.push_back(temp->data);
            temp = temp->next;
        }
        return userList;
    }
    
    string getNetworkMap() {
        if (!graph) return "Graph not loaded!";
        
        string map = "================================================\n";
        map += "    KARACHI DELIVERY NETWORK - GRAPH VIEW\n";
        map += "================================================\n\n";
        
        map += "LEGEND: [R] = Restaurant  [D] = Delivery Point\n\n";
        
        map += "--- ALL LOCATIONS ---\n";
        map += "RESTAURANTS:\n";
        map += "  [0] Burns Road Food Street\n";
        map += "  [2] Boat Basin Cafe\n";
        map += "  [4] Saddar Tikka House\n";
        map += "  [6] Tariq Road Broast\n";
        map += "  [9] Zamzama BBQ Tonight\n\n";
        
        map += "DELIVERY POINTS:\n";
        map += "  [1] Clifton Block 2\n";
        map += "  [3] Gulshan-e-Iqbal\n";
        map += "  [5] DHA Phase 5\n";
        map += "  [7] Bahria Town\n";
        map += "  [8] North Nazimabad\n\n";
        
        map += "================================================\n";
        map += "           NETWORK STRUCTURE (Tree View)\n";
        map += "================================================\n\n";
        
        map += "                [0] Burns Road [R]\n";
        map += "                    |\n";
        map += "        +-----------+-----------+\n";
        map += "        |           |           |\n";
        map += "     8km|        6km|       10km|\n";
        map += "  Shahrah    MA Jinnah   Clifton\n";
        map += "  Faisal       Road      Bridge\n";
        map += "        |           |           |\n";
        map += "        v           v           v\n";
        map += "    [1]Clifton  [4]Saddar  [2]Boat Basin\n";
        map += "       [D]         [R]         [R]\n";
        map += "        |           |           |\n";
        map += "     8km|        9km|        7km|\n";
        map += "  Korangi    Sharae    University\n";
        map += "    Road    Quaideen      Road\n";
        map += "        |           |           |\n";
        map += "        v           v           v\n";
        map += "    [5] DHA     [5] DHA    [3] Gulshan\n";
        map += "       [D]        [D]          [D]\n";
        map += "        |                       |\n";
        map += "     6km|                    4km|\n";
        map += "   Tariq                  Rashid\n";
        map += "    Road                  Minhas\n";
        map += "        |                       |\n";
        map += "        v                       v\n";
        map += "    [6] Tariq Road <----4km---[4] Saddar\n";
        map += "         [R]         (loop back)  [R]\n";
        map += "        |\n";
        map += "    11km|\n";
        map += "   Super\n";
        map += "  Highway\n";
        map += "        |\n";
        map += "        v\n";
        map += "    [7] Bahria Town [D]\n";
        map += "        |\n";
        map += "     5km|\n";
        map += "  Abul Hasan\n";
        map += "  Isphani Rd\n";
        map += "        |\n";
        map += "        v\n";
        map += "    [8] North Nazimabad [D]\n";
        map += "        |\n";
        map += "     7km|\n";
        map += "  Nishtar\n";
        map += "    Road\n";
        map += "        |\n";
        map += "        v\n";
        map += "    [9] Zamzama BBQ [R]\n\n";
        
        map += "================================================\n";
        map += "         ALL ROAD CONNECTIONS (Detailed)\n";
        map += "================================================\n\n";
        
        LinkedList<Edge>* adjList = graph->getAdjList();
        bool displayed[10][10] = {false};
        
        for (int i = 0; i < numNodes; i++) {
            Node<Edge> *temp = adjList[i].getHead();
            while (temp) {
                int j = temp->data.dest;
                if (!displayed[i][j] && !displayed[j][i]) {
                    string type1 = graph->getNodeType(i) == "restaurant" ? "[R]" : "[D]";
                    string type2 = graph->getNodeType(j) == "restaurant" ? "[R]" : "[D]";
                    
                    map += "[" + to_string(i) + "]" + type1 + " " + graph->getNodeName(i);
                    map += " <----> ";
                    map += "[" + to_string(j) + "]" + type2 + " " + graph->getNodeName(j) + "\n";
                    map += "    Street: " + temp->data.streetName + "\n";
                    map += "    Distance: " + to_string(temp->data.weight) + " km\n\n";
                    
                    displayed[i][j] = displayed[j][i] = true;
                }
                temp = temp->next;
            }
        }
        
        map += "================================================\n";
        map += "Total Nodes: " + to_string(numNodes) + "\n";
        map += "Restaurants: 5  |  Delivery Points: 5\n";
        map += "================================================\n";
        return map;
    }
    
    Graph* getGraph() { return graph; }
    int getNumNodes() { return numNodes; }
};

// Global simulator instance
FoodDeliverySimulator* sim = nullptr;
HFONT hTitleFont, hButtonFont, hTextFont;
HBRUSH hButtonBrush = NULL;

// Custom button colors
#define BTN_BG_COLOR RGB(45, 45, 65)
#define BTN_TEXT_COLOR RGB(255, 255, 255)
#define BTN_HOVER_COLOR RGB(60, 60, 85)

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Create fonts
            hTitleFont = CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                   OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                   DEFAULT_PITCH | FF_SWISS, "Arial");
            hButtonFont = CreateFont(18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                    DEFAULT_PITCH | FF_SWISS, "Arial");
            hTextFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                  DEFAULT_PITCH | FF_SWISS, "Arial");
            
            // Create title label
            HWND hTitle = CreateWindow("STATIC", "AUTOMATED FOOD DELIVERY SYSTEM",
                                      WS_VISIBLE | WS_CHILD | SS_CENTER,
                                      20, 15, 760, 35, hwnd, (HMENU)100, NULL, NULL);
            SendMessage(hTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
            
            // Create subtitle
            HWND hSubtitle = CreateWindow("STATIC", "Karachi Network - Dijkstra's Shortest Path Algorithm",
                                         WS_VISIBLE | WS_CHILD | SS_CENTER,
                                         20, 55, 760, 25, hwnd, (HMENU)101, NULL, NULL);
            SendMessage(hSubtitle, WM_SETFONT, (WPARAM)hTextFont, TRUE);
            
            // Create info section
            HWND hInfo = CreateWindow("STATIC", 
                "Delivery Points: Clifton, Gulshan, DHA, Bahria, N.Nazimabad\r\nRestaurants: Burns Road, Boat Basin, Saddar, Tariq Road, Zamzama",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                20, 90, 760, 50, hwnd, (HMENU)102, NULL, NULL);
            SendMessage(hInfo, WM_SETFONT, (WPARAM)hTextFont, TRUE);
            
            // Create buttons with custom style
            HWND btn0 = CreateWindow("BUTTON", "View Network Map", 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    150, 160, 500, 45, hwnd, (HMENU)7, NULL, NULL);
            
            HWND btn1 = CreateWindow("BUTTON", "Create New Order", 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    150, 215, 500, 45, hwnd, (HMENU)1, NULL, NULL);
            
            HWND btn2 = CreateWindow("BUTTON", "Dispatch Orders", 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    150, 270, 500, 45, hwnd, (HMENU)2, NULL, NULL);
            
            HWND btn3 = CreateWindow("BUTTON", "View Order History", 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    150, 325, 500, 45, hwnd, (HMENU)3, NULL, NULL);
            
            HWND btn4 = CreateWindow("BUTTON", "View All Agents", 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    150, 380, 500, 45, hwnd, (HMENU)4, NULL, NULL);
            
            HWND btn5 = CreateWindow("BUTTON", "View Pending Orders", 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    150, 435, 500, 45, hwnd, (HMENU)5, NULL, NULL);
            
            HWND btn6 = CreateWindow("BUTTON", "Exit Application", 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                                    150, 490, 500, 45, hwnd, (HMENU)6, NULL, NULL);
            
            hButtonBrush = CreateSolidBrush(BTN_BG_COLOR);
            
            // Status bar at bottom
            HWND hStatus = CreateWindow("STATIC", "Status: Ready | DSA Final Project",
                                       WS_VISIBLE | WS_CHILD | SS_CENTER,
                                       20, 555, 760, 20, hwnd, (HMENU)103, NULL, NULL);
            SendMessage(hStatus, WM_SETFONT, (WPARAM)hTextFont, TRUE);
            break;
        }
        
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
            if (dis->CtlType == ODT_BUTTON) {
                // Get button text
                char text[100];
                GetWindowText(dis->hwndItem, text, 100);
                
                // Fill button background
                FillRect(dis->hDC, &dis->rcItem, hButtonBrush);
                
                // Draw border
                HPEN hPen = CreatePen(PS_SOLID, 2, RGB(80, 120, 200));
                HPEN hOldPen = (HPEN)SelectObject(dis->hDC, hPen);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(dis->hDC, GetStockObject(NULL_BRUSH));
                Rectangle(dis->hDC, dis->rcItem.left, dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom);
                SelectObject(dis->hDC, hOldPen);
                SelectObject(dis->hDC, hOldBrush);
                DeleteObject(hPen);
                
                // Draw text
                SetBkMode(dis->hDC, TRANSPARENT);
                SetTextColor(dis->hDC, BTN_TEXT_COLOR);
                SelectObject(dis->hDC, hButtonFont);
                DrawText(dis->hDC, text, -1, &dis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                
                return TRUE;
            }
            break;
        }
        
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            HWND hwndStatic = (HWND)lParam;
            int id = GetDlgCtrlID(hwndStatic);
            
            if (id == 100) { // Title
                SetTextColor(hdcStatic, RGB(255, 165, 0)); // Bright Orange
                SetBkMode(hdcStatic, TRANSPARENT);
                return (LRESULT)GetStockObject(NULL_BRUSH);
            } else if (id == 101) { // Subtitle
                SetTextColor(hdcStatic, RGB(100, 200, 255)); // Light Blue
                SetBkMode(hdcStatic, TRANSPARENT);
                return (LRESULT)GetStockObject(NULL_BRUSH);
            } else if (id == 102) { // Info
                SetTextColor(hdcStatic, RGB(144, 238, 144)); // Light Green
                SetBkMode(hdcStatic, TRANSPARENT);
                return (LRESULT)GetStockObject(NULL_BRUSH);
            } else if (id == 103) { // Status
                SetTextColor(hdcStatic, RGB(169, 169, 169)); // Light Gray
                SetBkMode(hdcStatic, TRANSPARENT);
                return (LRESULT)GetStockObject(NULL_BRUSH);
            }
            break;
        }
        
        case WM_CTLCOLORBTN: {
            // Make buttons have dark background
            return (LRESULT)CreateSolidBrush(RGB(45, 45, 58));
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Draw dark background
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 40)); // Dark blue-gray
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            
            // Draw glowing line under header
            HPEN hPen = CreatePen(PS_SOLID, 3, RGB(100, 200, 255));
            SelectObject(hdc, hPen);
            MoveToEx(hdc, 20, 150, NULL);
            LineTo(hdc, 780, 150);
            DeleteObject(hPen);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 7: { // View Network Map
                    string map = sim->getNetworkMap();
                    MessageBox(hwnd, map.c_str(), "Karachi Network Map", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                case 1: { // Create Order
                    char name[100] = "", nodeStr[10] = "", restStr[10] = "", priorStr[10] = "";
                    
                    // Show dialog boxes for input
                    string locations = "Available Delivery Locations:\n\n"
                                      "1 - Clifton Block 2\n"
                                      "3 - Gulshan-e-Iqbal\n"
                                      "5 - DHA Phase 5\n"
                                      "7 - Bahria Town\n"
                                      "8 - North Nazimabad\n\n"
                                      "Please enter location number (1, 3, 5, 7, or 8):";
                    
                    string restaurants = "Available Restaurants:\n\n"
                                        "1 - Burns Road Food Street\n"
                                        "2 - Boat Basin Cafe\n"
                                        "3 - Saddar Tikka House\n"
                                        "4 - Tariq Road Broast\n"
                                        "5 - Zamzama BBQ Tonight\n\n"
                                        "Please enter restaurant ID (1-5):";
                    
                    // Input dialogs
                    if (MessageBox(hwnd, "Would you like to create a new order?", "Create Order", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        AllocConsole();
                        freopen("CONOUT$", "w", stdout);
                        freopen("CONIN$", "r", stdin);
                        
                        cout << "\n========================================\n";
                        cout << "        CREATE NEW ORDER\n";
                        cout << "========================================\n\n";
                        
                        cout << "Enter Customer Name: ";
                        cin.getline(name, 100);
                        
                        cout << "\nAvailable Delivery Points:\n";
                        cout << "  1 - Clifton Block 2\n";
                        cout << "  3 - Gulshan-e-Iqbal\n";
                        cout << "  5 - DHA Phase 5\n";
                        cout << "  7 - Bahria Town\n";
                        cout << "  8 - North Nazimabad\n";
                        cout << "\nSelect Delivery Point: ";
                        cin.getline(nodeStr, 10);
                        
                        cout << "\nAvailable Restaurants:\n";
                        cout << "  1 - Burns Road Food Street\n";
                        cout << "  2 - Boat Basin Cafe\n";
                        cout << "  3 - Saddar Tikka House\n";
                        cout << "  4 - Tariq Road Broast\n";
                        cout << "  5 - Zamzama BBQ Tonight\n";
                        cout << "\nSelect Restaurant ID: ";
                        cin.getline(restStr, 10);
                        
                        cout << "\nEnter Priority (1-10, >=5 is URGENT): ";
                        cin.getline(priorStr, 10);
                        
                        string result = sim->createOrder(name, atoi(nodeStr), atoi(restStr), atoi(priorStr));
                        
                        cout << "\n========================================\n";
                        cout << "RESULT: " << result << "\n";
                        cout << "========================================\n";
                        cout << "\nPress Enter to continue...";
                        cin.get();
                        
                        FreeConsole();
                        MessageBox(hwnd, result.c_str(), "Order Status", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                }
                case 2: { // Dispatch
                    // Check if there are pending orders first
                    string pendingInfo = sim->getPendingOrdersInfo();
                    
                    auto results = sim->dispatchOrders();
                    if (results.empty()) {
                        string msg = "No orders dispatched.\n\n";
                        msg += "Current Status:\n";
                        msg += pendingInfo + "\n\n";
                        
                        // Check agent availability
                        vector<Agent> agents = sim->getAllAgents();
                        int availableAgents = 0;
                        for (const auto& a : agents) {
                            if (a.available) availableAgents++;
                        }
                        msg += "Available Agents: " + to_string(availableAgents) + " / " + to_string(agents.size()) + "\n\n";
                        
                        if (availableAgents == 0) {
                            msg += "Issue: No agents available!\n";
                        } else {
                            msg += "Tip: Create orders first, then dispatch them.";
                        }
                        
                        MessageBox(hwnd, msg.c_str(), "Dispatch Status", MB_OK | MB_ICONWARNING);
                    } else {
                        string msg = "========== DISPATCH SUCCESSFUL ==========\n\n";
                        msg += "Total Orders Dispatched: " + to_string(results.size()) + "\n\n";
                        for (const auto& r : results) {
                            msg += "----------------------------------------\n";
                            msg += "Order #" + to_string(r.orderId) + "\n";
                            msg += "Customer: " + r.customerName + "\n";
                            msg += "Restaurant: " + r.restaurantName + "\n";
                            msg += "Agent: " + r.agentName + "\n";
                            msg += "Distance: " + to_string(r.distance) + " km\n";
                            msg += "ETA: " + to_string(r.eta) + " minutes\n";
                            msg += "Path: ";
                            for (size_t i = 0; i < r.path.size(); i++) {
                                msg += to_string(r.path[i]);
                                if (i < r.path.size() - 1) msg += " -> ";
                            }
                            msg += "\n\n";
                        }
                        msg += "=========================================";
                        MessageBox(hwnd, msg.c_str(), "Dispatch Results", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                }
                case 3: { // Order History
                    vector<string> history = sim->getAllOrders();
                    string msg = "========== DISPATCHED ORDER HISTORY ==========\n\n";
                    if (history.empty()) {
                        msg += "No dispatched orders yet.\n\n";
                        msg += "To see dispatched orders:\n";
                        msg += "1. Create orders using 'Create New Order'\n";
                        msg += "2. Dispatch them using 'Dispatch Orders'\n\n";
                        msg += "Check 'View Pending Orders' to see orders waiting for dispatch.";
                    } else {
                        msg += "Total Dispatched Orders: " + to_string(history.size()) + "\n\n";
                        for (size_t i = 0; i < history.size(); i++) {
                            msg += to_string(i + 1) + ". " + history[i] + "\n\n";
                        }
                    }
                    msg += "\n==========================================";
                    MessageBox(hwnd, msg.c_str(), "Order History", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                case 4: { // View Agents
                    vector<Agent> agents = sim->getAllAgents();
                    string msg = "========== DELIVERY AGENTS ==========\n\n";
                    for (const auto& agent : agents) {
                        msg += "----------------------------------------\n";
                        msg += "Agent #" + to_string(agent.id) + ": " + agent.name + "\n";
                        msg += "Location: Node " + to_string(agent.nodeId) + " (" + sim->getGraph()->getNodeName(agent.nodeId) + ")\n";
                        msg += "Status: " + string(agent.available ? "[AVAILABLE]" : "[BUSY]") + "\n\n";
                    }
                    msg += "=====================================";
                    MessageBox(hwnd, msg.c_str(), "Delivery Agents", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                case 5: { // Pending Orders
                    string info = "========== PENDING ORDERS ==========\n\n";
                    info += sim->getPendingOrdersInfo();
                    info += "\n\nNote: Urgent orders (Priority >= 5)\n";
                    info += "are dispatched first!\n";
                    info += "\n====================================";
                    MessageBox(hwnd, info.c_str(), "Pending Orders", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                case 6: // Exit
                    if (MessageBox(hwnd, "Are you sure you want to exit?", "Exit Application", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        PostQuitMessage(0);
                    }
                    break;
            }
            break;
            
        case WM_DESTROY:
            DeleteObject(hTitleFont);
            DeleteObject(hButtonFont);
            DeleteObject(hTextFont);
            if (hButtonBrush) DeleteObject(hButtonBrush);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize simulator
    sim = new FoodDeliverySimulator();
    
    // Register window class
    const char CLASS_NAME[] = "FoodDeliverySimulator";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);
    
    // Create window
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Food Delivery Simulator - Karachi Network",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 820, 660,
        NULL, NULL, hInstance, NULL
    );
    
    if (hwnd == NULL) return 0;
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    delete sim;
    return 0;
}