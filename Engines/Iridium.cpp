#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <climits>
#include <cmath>
#include <stdlib.h>
#include "Coin.hpp"
#include "Glyph.hpp"
#include "UTTT.hpp"

#define EXP_FACTOR 5

using namespace UTTT;

class TreeNode
{
public:
    int winScore = 0;
    int visits = 0;
    int playerNo;
    State board;
    TreeNode *parent = nullptr;
    std::vector<TreeNode *> children;

    TreeNode(State board)
    {
        this->board = board;
    }

    void set_player_no(int playerNo)
    {
        this->playerNo = playerNo;
    }

    auto get_player_no() -> int
    {
        return playerNo;
    }

    auto get_opponent() -> int
    {
        return -playerNo;
    }

    void set_parent(TreeNode *parent)
    {
        this->parent = parent;
    }

    void set_state(State board)
    {
        this->board = board;
    }

    void show()
    {
        std::cout << "My state is:\n";
        board.show();
        if (parent)
        {
            std::cout << "My parent's state is:\n";
            parent->show();
        }
        std::cout << "and I have " << children.size() << " children.\n";
    }

    auto get_children() -> std::vector<TreeNode *>
    {
        return children;
    }

    auto get_children_as_states() -> std::vector<State>
    {
        std::vector<State> childStates;
        for (auto &&move : board.legal_moves())
        {
            board.play(move);
            childStates.push_back(board);
            board.unplay();
        }
        return childStates;
    }

    auto get_win_score() -> int
    {
        return winScore;
    }

    auto get_visit_count() -> int
    {
        return visits;
    }

    auto get_parent_visits() -> int
    {
        return parent->get_visit_count();
    }

    void increment_visits()
    {
        visits++;
    }

    void add_score(int s)
    {
        winScore += s;
    }

    void set_win_score(int s)
    {
        winScore = s;
    }

    auto get_parent() -> TreeNode *
    {
        return parent;
    }

    auto get_state() -> State
    {
        return board;
    }

    auto random_child() -> TreeNode *
    {
        int noOfPossibleMoves = children.size();
        int selectRandom = rand() % noOfPossibleMoves;
        return children[selectRandom];
    }

    auto get_winrate() -> double
    {
        return (double)winScore / (double)visits;
    }

    auto best_child() -> TreeNode *
    {
        std::vector<TreeNode *>::iterator result;
        //show();
        result = std::max_element(
            children.begin(), children.end(),
            [](TreeNode *a, TreeNode *b) { return (a->get_visit_count() < b->get_visit_count()); });
        return children.at(std::distance(children.begin(), result));
    }

    void show_child_winrates()
    {
        for (auto &&child : children)
        {
            std::cout << child->get_win_score() << " ";
        }
        std::cout << "\n";
    }

    void show_child_visitrates()
    {
        for (auto &&child : children)
        {
            std::cout << child->get_visit_count() << " ";
        }
        std::cout << "\n";
    }
};

namespace UCT
{
    auto uct_value(
        int totalVisit, double nodeWinScore, int nodeVisit) -> double
    {
        if (nodeVisit == 0)
        {
            return INT_MAX;
        }
        return ((double)nodeWinScore / (double)nodeVisit) + 1.41 * sqrt(log(totalVisit) / (double)nodeVisit) * EXP_FACTOR;
    }

    auto uct_compare(TreeNode *a, TreeNode *b) -> bool
    {
        return (
            uct_value(
                a->get_parent_visits(),
                a->get_win_score(),
                a->get_visit_count()) <
            uct_value(
                b->get_parent_visits(),
                b->get_win_score(),
                b->get_visit_count()));
    }

    auto best_node_uct(TreeNode *node) -> TreeNode *
    {
        auto children = node->get_children();
        std::vector<TreeNode *>::iterator result;
        result = std::max_element(
            children.begin(), children.end(),
            uct_compare);
        return children[std::distance(children.begin(), result)];
    }
}; // namespace UCT

class MCTS
{
public:
    const int WIN_SCORE = 10;
    int timeLimit; // limiter on search time
    int opponent;  // the win score that the opponent wants
    int reward;    // the win score that the agent wants
    int nodes = 0;

    MCTS()
    {
        MCTS(1);
    }
    MCTS(int player)
    {
        MCTS(player, 3);
    }
    MCTS(int player, int strength)
    {
        srand(time(NULL));
        timeLimit = strength;
        opponent = -player;
        reward = player;
    }

    void deleteTree(TreeNode *root)
    {
        /* first delete the subtrees */
        for (TreeNode *child : root->children)
        {
            deleteTree(child);
        }
        /* then delete the node */
        delete root;
    }

    void set_opponent(int i)
    {
        opponent = i;
        reward = -i;
    }

    auto find_best_next_board(State board) -> State
    {
        nodes = 0;
        set_opponent(-board.turn);
        // define an end time which will act as a terminating condition
        int end = std::time(0) + timeLimit;
        TreeNode *rootNode = new TreeNode(board);
        rootNode->set_state(board);
        rootNode->set_player_no(opponent);
        
        while (std::time(0) < end)
        {
            TreeNode *promisingNode = select_promising_node(rootNode);

            if (!promisingNode->get_state().is_game_over())
            {
                expand_node(promisingNode);
            }
        
            TreeNode *nodeToExplore = promisingNode;
            if (promisingNode->get_children().size() > 0)
            {
                nodeToExplore = promisingNode->random_child();
            }
            int playoutResult = simulate_playout(nodeToExplore);
            backpropagate(nodeToExplore, playoutResult);
        }
        State out = rootNode->best_child()->get_state();
        std::cout << "ZERO:\n";
        std::cout << nodes << " nodes processed.\n";
        std::cout << "Zero win prediction: " << (int)(rootNode->best_child()->get_winrate() * (100 / WIN_SCORE)) << "%\n";
        deleteTree(rootNode);
        return out;
    }

    auto select_promising_node(TreeNode *rootNode) -> TreeNode *
    {
        TreeNode *node = rootNode;
        while (node->get_children().size() != 0)
        {
            node = UCT::best_node_uct(node);
        }
        return node;
    }

    void expand_node(TreeNode *node)
    {
        std::vector<State> possibleStates = node->get_children_as_states();
        for (State state : possibleStates)
        {
            TreeNode *newNode = new TreeNode(state);
            newNode->set_parent(node);
            newNode->set_player_no(node->get_opponent());
            node->children.push_back(newNode);
        }
    }

    void backpropagate(TreeNode *nodeToExplore, int winner)
    {
        TreeNode *tempNode = nodeToExplore;
        while (tempNode)
        {
            tempNode->increment_visits();
            if (tempNode->get_player_no() == winner)
            {
                tempNode->add_score(WIN_SCORE);
            }
            tempNode = tempNode->get_parent();
        }
    }

    auto simulate_playout(TreeNode *node) -> int
    {
        nodes++;
        TreeNode tempNode = TreeNode(*node);
        State tempState = tempNode.get_state();
        int boardStatus = tempState.evaluate();
        if (boardStatus == opponent)
        {
            node->get_parent()->set_win_score(INT_MIN);
            return boardStatus;
        }
        while (!tempState.is_game_over())
        {
            tempState.random_play();
        }
        boardStatus = tempState.evaluate();
        return boardStatus;
    }
};

class Zero
{
public:
    MCTS searchDriver = MCTS(1);
    State node = State();

    Zero()
    {
        Zero(3);
    }
    Zero(int strength)
    {
        searchDriver.timeLimit = strength;
    }

    void print(std::string input, std::string end = "\n")
    {
        std::cout << input << end;
    }

    auto get_player_move() -> int
    {
        int pos;
        std::cin >> pos;
        while (node.pos_filled(pos - 1))
        {
            print("invalid move.");
            node.show();
            std::cin >> pos;
        }
        return pos - 1;
    }

    void engine_move()
    {
        node = searchDriver.find_best_next_board(node);
    }

    void show_result()
    {
        int r;
        r = node.evaluate();
        if (r == 0)
            std::cout << "1/2-1/2" << '\n';
        else if (r == 1)
            std::cout << "1-0" << '\n';
        else
            std::cout << "0-1" << '\n';
    }
};

auto get_first_player() -> bool
{
    bool player;
    std::cout << "Is the human player going first? [1/0]"
              << "\n";
    std::cin >> player;
    return player;
}

class Istus
{
public:
    State node;
    int nodes;
    float timeLimit;
    short d;
    short bestcase;
    float end;
    float t;
    short score;

    Istus()
    {
        Istus(3);
    }
    Istus(float TL)
    {
        timeLimit = TL;
    }

    auto negamax(int depth = 10, int colour = 1, int a = -2, int b = 2) -> int //WORKING
    {
        if (depth < 1)
        {
            nodes++;
            return colour * node.evaluate();
        }

        if (node.is_game_over())
        {
            nodes++;
            return colour * node.evaluate() * depth;
        }
        int score;

        node.pass_turn();                             // MAKE A NULL MOVE
        score = -negamax(depth - 3, -colour, -b, -a); // PERFORM A LIMITED SEARCH
        node.pass_turn();                             // UNMAKE NULL MOVE
        if (score > a)
            a = score;
        if (a >= b)
            return a;

        for (auto &&move : node.legal_moves())
        {
            node.play(move);
            nodes += 1;
            score = -negamax(depth - 1, -colour, -b, -a);
            node.unplay();

            if (score >= b)
                return b;
            if (score > a)
                a = score;
        }

        return a;
    }

    void engine_move() //WORKING
    {
        nodes = 0;
        bestcase = -2;
        Move bestmove;
        d = 0;
        
        end = std::time(0) + timeLimit;
        while (std::time(0) < end && d < 22)
        {
            bestcase = -2;
            int loopstart = std::time(0);
            for (auto &&move : node.legal_moves())
            {
                node.play(move);
                score = -negamax(d, node.turn);
                node.unplay();
                if (bestcase < score)
                {
                    bestcase = score;
                    bestmove = move;
                }
            }
            t = (std::time(0) - loopstart);
            d += 1;
        }
        std::cout << "ISTUS:\n";
        std::cout << nodes << " nodes processed.\n";
        std::cout << "Istus win prediction: " << (int)((1 + bestcase) * (50)) << "%\n";
        node.play(bestmove);
    }

    void reset_nodes()
    {
        nodes = 0;
    }

    void show_result()
    {
        int r;
        r = node.evaluate();
        if (r == 0)
            std::cout << "1/2-1/2" << '\n';
        else if (r == 1)
            std::cout << "1-0" << '\n';
        else
            std::cout << "0-1" << '\n';
    }

    void print(std::string input, std::string end = "\n")
    {
        std::cout << input << end;
    }

    auto get_player_move() -> int
    {
        int pos;
        std::cin >> pos;
        while (node.pos_filled(pos - 1))
        {
            print("invalid move.");
            node.show();
            std::cin >> pos;
        }
        return pos - 1;
    }
};

inline void run_negamax_game(int TL)
{
    Istus glyph = Istus(TL);
    Move i;
    glyph.node.show();
    if (get_first_player())
    {
        i = glyph.get_player_move();
        glyph.node.play(i);
        glyph.node.show();
    }
    while (!glyph.node.is_game_over())
    {
        glyph.engine_move();
        glyph.reset_nodes();
        glyph.node.show();
        if (glyph.node.is_game_over())
            break;
        i = glyph.get_player_move();
        glyph.node.play(i);
        glyph.node.show();
    }
    glyph.show_result();
}

inline void run_mcts_game(int TL)
{
    Zero glyph = Zero(TL);
    Move i;
    glyph.node.show();
    if (get_first_player())
    {
        i = glyph.get_player_move();
        glyph.node.play(i);
        glyph.node.show();
    }
    while (!glyph.node.is_game_over())
    {
        glyph.engine_move();
        glyph.searchDriver.nodes = 0;
        glyph.node.show();
        if (glyph.node.is_game_over())
            break;
        i = glyph.get_player_move();
        glyph.node.play(i);
        glyph.node.show();
    }
    glyph.show_result();
}

inline void selfplay(int TL)
{
    Istus minimaxer = Istus(TL);
    Zero montecarlo = Zero(TL);
    int eturn;
    std::cout << "1 for Zero first, -1 for Istus first.\n--> ";
    std::cin >> eturn;
    while (!minimaxer.node.is_game_over() && !montecarlo.node.is_game_over())
    {
        minimaxer.node.show();
        if (eturn == -1)
        {
            minimaxer.engine_move();
            montecarlo.node = minimaxer.node;
        }
        else
        {
            montecarlo.engine_move();
            minimaxer.node = montecarlo.node;
        }
        eturn = -eturn;
    }
    minimaxer.node.show();
    minimaxer.show_result();
}

int main()
{
    std::cout << "Play against Zero [0] | Play against Istus [1] | Watch a self-play game [2]\n--> ";
    int ans;
    std::cin >> ans;
    std::cout << "seconds per move? ";
    int TL;
    std::cin >> TL;
    switch (ans)
    {
    case 0:
        run_mcts_game(TL);
        break;

    case 1:
        run_negamax_game(TL);
        break;

    case 2:
        selfplay(TL);
        break;

    default:
        break;
    }

    return 0;
}