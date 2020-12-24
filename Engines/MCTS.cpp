#include <iostream>
#include <vector>



class State 
{
public:
    Board board;
    int playerNo;
    int visitCount;
    double winScore;

    // copy constructor, getters, and setters

    std::vector<State> getAllPossibleStates() {
        // constructs a list of all possible states from current state
    }

    void randomPlay() {
        /* get a list of all possible positions on the board and 
           play a random move */
    }
};

class Node
{
    State state;
    Node parent;
    std::vector<Node> childArray;
    // setters and getters
};

class Tree
{
    Node root;
};

class MonteCarloTreeSearch 
{
public:
    const int WIN_SCORE = 10;
    int level;
    int opponent;

    Board findNextMove(Board board, int playerNo) {
        // define an end time which will act as a terminating condition

        opponent = 3 - playerNo;
        Tree *tree = new Tree();
        Node *rootNode = tree->getRoot();
        rootNode->getState().setBoard(board);
        rootNode->getState().setPlayerNo(opponent);

        while (System.currentTimeMillis() < end) {
            Node promisingNode = selectPromisingNode(rootNode);
            if (promisingNode.getState().getBoard().checkStatus() 
              == Board.IN_PROGRESS) {
                expandNode(promisingNode);
            }
            Node nodeToExplore = promisingNode;
            if (promisingNode.getChildArray().size() > 0) {
                nodeToExplore = promisingNode.getRandomChildNode();
            }
            int playoutResult = simulateRandomPlayout(nodeToExplore);
            backPropogation(nodeToExplore, playoutResult);
        }

        Node winnerNode = rootNode.getChildWithMaxScore();
        tree.setRoot(winnerNode);
        return winnerNode.getState().getBoard();
    }
};