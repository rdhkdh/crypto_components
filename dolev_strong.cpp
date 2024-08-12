#include<bits/stdc++.h>
using namespace std;

const int N = 12; //total no. of nodes
const int f = 3; //no. of faulty nodes
const int TOTAL_TIME = f + 1;
int GLOBAL_CLOCK = 0;
map<string, int> block_convinced; //count of nodes convinced of each block
vector<string> msgs_received[N]; //lists msgs received from each of the N nodes acting as senders

struct Message{
    string block;
    vector<int> signature_list;
};

struct Node{
    int id;
    bool honest;
    vector<Message*> incoming_msgs;
    vector<Message*> outgoing_msgs;
    set<string> convinced_blocks;
};

vector<Node*> nodes; //all nodes
vector<Node*> honest_nodes; //perceived honest nodes. Actual behaviour is represented by object attribute 'honest'.
set<int> dishonest_nodes; //indices of dishonest nodes

int generate_random_binary() // Create a random number generator engine
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 1); // Define distribution for 0 and 1

    // Generate random 0 or 1
    int randomNumber = distribution(gen);

    return randomNumber;
}

int generate_random_int(int min, int max) // Create a random number generator engine
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(min, max); // Define distribution for min and max

    // Generate random number between min and max
    int randomNumber = distribution(gen);

    return randomNumber;
}

// Validate block
bool validate_block(const string& block)
{
    // TODO: Implement block validation logic
    return true;
}

// Authenticates message sender
bool verify_signature(const Message* m, const Node* sender)
{
    // TODO: Implement signature verification logic
    // This function should verify if the message m has been signed by the sender
    // using the sender's public key
    return true;
}

// Check if the message is valid
bool is_valid(Message* m, Node* sender)
{
    // Verify sender's signature
    if (!verify_signature(m, sender)) return false;

    // Validate block
    if (!validate_block(m->block)) return false;

    return true;
}

// Send message to all other non-senders
void send_message(Node* sender, Message* msg, bool PKI)
{   
    if(sender->honest){
        for (Node* receiver : nodes) {
            if (receiver != sender) {
                Message* signed_msg = new Message(*msg);
                signed_msg->signature_list.push_back(sender->id);
                receiver->incoming_msgs.push_back(signed_msg);
            }
        }
    }
    else{
        for (Node* receiver : nodes) {
            if (receiver != sender) {
                Message* signed_msg = new Message();
                if (generate_random_binary() == 0) {
                    signed_msg->block = msg->block;
                    signed_msg->signature_list = msg->signature_list;
                }
                else {
                    signed_msg->block = "malicious message from " + to_string(sender->id);
                    if (!PKI) {
                        signed_msg->signature_list = msg->signature_list;
                    }
                }
                signed_msg->signature_list.push_back(sender->id);
                receiver->incoming_msgs.push_back(signed_msg);
            }
        }
    }
}

// Phase function for each time step
void phase(int t, int initial_sender_id, bool PKI)
{   
    // printf("Time step: %d\n", t);

    // Check if each node is convinced of exactly one value
    for (Node* node : nodes) {
        if(node->id == initial_sender_id)
            continue;
        
        // printf("Node %d received %lu messages\n", node->id, node->incoming_msgs.size());
        for (Message* msg : node->incoming_msgs) {
            // Check if the node is convinced of the block
            if (is_valid(msg, node) && msg->signature_list[0] == initial_sender_id && msg->signature_list.size() >= t) {
                if((node->convinced_blocks).find(msg->block) == node->convinced_blocks.end()) {
                    // printf("Node %d is convinced of block: %s\n", node->id, msg->block.c_str());
                    msg->signature_list.push_back(node->id);
                    node->convinced_blocks.insert(msg->block);
                    node->outgoing_msgs.push_back(msg);
                }
            }
            // printf("Node %d is about to send %lu messages\n", node->id, node->outgoing_msgs.size());
        }

    }

    // Update incoming messages
    for (Node* node : nodes) {
        node->incoming_msgs.clear();
    }

    // Send messages
    for (Node* node : nodes) {
        for (Message* msg : node->outgoing_msgs) {
            send_message(node, msg, PKI);
        }
        node->outgoing_msgs.clear();
    }
}

void dolev_strong(bool PKI)
{
    // Initialize nodes
    for (int i = 0; i < N; i++) {
        Node* temp = new Node();
        temp->id = i;
        temp->honest = (i >= f); // First f nodes are dishonest
        nodes.push_back(temp);
        if (temp->honest) {
            honest_nodes.push_back(temp);
        } else {
            dishonest_nodes.insert(i);
        }
    }

    // Send initial input from the designated node
    int initial_sender_id = generate_random_int(0, N - 1);
    if (nodes[initial_sender_id]->honest) {
        // Honest initial sender
        cout << "Honest initial sender: " << initial_sender_id << endl;
        for (Node* receiver : nodes) {
            if (receiver->id != initial_sender_id) {
                Message* msg = new Message();
                msg->block = "private input";
                vector<int> signature_list(1, initial_sender_id);
                msg->signature_list = signature_list;
                receiver->incoming_msgs.push_back(msg);
            }
        }
        nodes[initial_sender_id]->convinced_blocks.insert("private input");
    } else {
        // Dishonest initial sender
        cout << "Dishonest initial sender: " << initial_sender_id << endl;
        for (Node* receiver : nodes) {
            if (receiver->id != initial_sender_id) {
                Message* msg = new Message();
                if(generate_random_binary() == 0)
                    msg->block = "private input";
                else
                    msg->block = "malicious input from " + to_string(initial_sender_id);
                vector<int> signature_list(1, initial_sender_id);
                msg->signature_list = signature_list;
                receiver->incoming_msgs.push_back(msg);
            }
        }
    }

    GLOBAL_CLOCK = 1;
    // Run rounds
    while (GLOBAL_CLOCK <= TOTAL_TIME) {
        phase(GLOBAL_CLOCK, initial_sender_id, PKI);
        GLOBAL_CLOCK++;
    }

    // Insert the convinced blocks into the set of values
    for (Node* node : honest_nodes) {
        if (node->convinced_blocks.size() == 1) {
            printf("Node %d is convinced of block: %s\n", node->id, (*node->convinced_blocks.begin()).c_str());
            block_convinced[*node->convinced_blocks.begin()]++;
        }
        else if (node->convinced_blocks.size() > 1) {
            printf("Node %d is convinced of blocks: ", node->id);
            for (string block : node->convinced_blocks) {
                printf("%s, ", block.c_str());
                block_convinced[block]++;
            }
            printf("\n");
        }
        else {
            printf("Node %d is not convinced of any block\n", node->id);
        }
    }

    // Print consensus
    bool consensus_reached = true;
    for (auto it = block_convinced.begin(); it != block_convinced.end(); it++) {
        if (it->second < N - f) {
            consensus_reached = false;
            break;
        }
    }

    if (!consensus_reached) {
        printf("Consensus not reached\n");
    } else {
        if (block_convinced.size() == 1) {
            printf("Consensus reached on block: %s\n", (*block_convinced.begin()).first.c_str());
        } else if (block_convinced.size() > 1) {
            printf("Consensus reached on blocks: ");
            for (auto it = block_convinced.begin(); it != block_convinced.end(); it++) {
                printf("%s, ", it->first.c_str());
            }
            printf("\n");
        } else {
            printf("Consensus reached on no block\n");
        }
    }
}

int main()
{
    cout << "Do you want to use Public Key Infrastructure (PKI)? (y/n): ";
    char choice;
    cin >> choice;
    bool PKI = false;
    if (choice == 'y' || choice == 'Y') {
        PKI = true;
    }

    dolev_strong(PKI);

    // Delete allocated memory
    for (Node* node : nodes) {
        delete node;
    }
    return 0;
}
