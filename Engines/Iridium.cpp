#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <climits>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <cassert>
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
    short playerNo;
    State board;
    TreeNode *parent = nullptr;
    std::vector<TreeNode *> children;

    TreeNode(State board)
    {
        this->board = board;
    }

    void set_player_no(short playerNo)
    {
        this->playerNo = playerNo;
    }

    auto get_player_no() -> short
    {
        return playerNo;
    }

    auto get_opponent() -> short
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
        return children[rand() % children.size()];
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

    auto best_child_as_move() -> Move
    {
        std::vector<TreeNode *>::iterator result;
        //show();
        result = std::max_element(
            children.begin(), children.end(),
            [](TreeNode *a, TreeNode *b) { return (a->get_visit_count() < b->get_visit_count()); });
        return board.legal_moves().at(std::distance(children.begin(), result));
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
    auto uct_value(int totalVisit, double nodeWinScore, int nodeVisit) -> double
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
    const short WIN_SCORE = 10;
    short timeLimit;     // limiter on search time
    short opponent;      // the win score that the opponent wants
    short reward;        // the win score that the agent wants
    bool memsafe = true; // dictates whether we preserve a part of the tree across moves
    int nodes = 0;
    TreeNode *preservedNode = nullptr;

    MCTS()
    {
        MCTS(1);
    }
    MCTS(short player)
    {
        MCTS(player, 3);
    }
    MCTS(short player, short strength)
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

    void set_opponent(short i)
    {
        opponent = i;
        reward = -i;
    }

    auto prune(TreeNode *parent, State target) -> TreeNode *
    {
        TreeNode *out = nullptr;
        bool found = false;
        for (TreeNode *child : parent->get_children())
        {
            if (!found && child->get_state() == target)
            {
                out = child;
                found = true;
            }
            else
                deleteTree(child);
        }
        if (out)
            out->set_parent(nullptr);
        delete parent;
        return out;
    }

    auto find_best_next_board(State board) -> State
    {
        nodes = 0;
        set_opponent(-board.turn);
        // define an end time which will act as a terminating condition
        auto end = std::chrono::steady_clock::now();
        end += std::chrono::milliseconds(timeLimit);
        TreeNode *rootNode;
        if (preservedNode)
        {
            rootNode = prune(preservedNode, board);
            if (!rootNode)
            {
                rootNode = new TreeNode(board);
                rootNode->set_state(board);
                rootNode->set_player_no(opponent);
            }
        }
        else
        {
            rootNode = new TreeNode(board);
            rootNode->set_state(board);
            rootNode->set_player_no(opponent);
        }

        while (std::chrono::steady_clock::now() < end)
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
            short playoutResult = simulate_playout(nodeToExplore);
            backpropagate(nodeToExplore, playoutResult);
        }
        State out = rootNode->best_child()->get_state();
        std::cerr << "ZERO:\n";
        std::cerr << nodes << " nodes processed.\n";
        std::cerr << "Zero win prediction: " << (int)(rootNode->best_child()->get_winrate() * (100 / WIN_SCORE)) << "%\n";
        short action, sboard, square, row, col;
        action = rootNode->best_child_as_move();
        square = action % 9;
        sboard = action / 9;
        col = (sboard % 3) * 3 + square % 3;
        row = (sboard / 3) * 3 + square / 3;
        std::cout << row << " " << col << std::endl;
        if (!memsafe)
        {
            deleteTree(rootNode);
        }
        else
        {
            preservedNode = prune(rootNode, out);
        }
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

    void backpropagate(TreeNode *nodeToExplore, short winner)
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

    auto simulate_playout(TreeNode *node) -> short
    {
        nodes++;
        TreeNode tempNode = TreeNode(*node);
        State tempState = tempNode.get_state();
        short boardStatus = tempState.evaluate();
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
    MCTS searchDriver = MCTS();
    State node = State();

    Zero()
    {
        Zero(99);
    }
    Zero(short strength)
    {
        searchDriver.timeLimit = strength;
    }

    void print(std::string input, std::string end = "\n")
    {
        std::cout << input << end;
    }

    auto get_player_move() -> int
    {
        std::vector<Move> legals = node.legal_moves();
        int pos;
        std::cin >> pos;
        while (std::none_of(legals.begin(), legals.end(), [pos](Move m) { return m == (pos - 1); }))
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
        switch (node.evaluate())
        {
        case 0:
            std::cout << "1/2-1/2" << '\n';
            break;
        case 1:
            std::cout << "1-0" << '\n';
            break;
        case -1:
            std::cout << "0-1" << '\n';
            break;
        default:
            std::cerr << "evaluate returned non-zero";
            break;
        }
    }
};

// Possible heuristic improvement: use a long search to generate MCTS values for each starting square, use them as a heuristic starter.
// The RAVE approach makes this heuristic value = some sort of aggregate score of the move on parent nodes.
// UCT becomes (simulation value / rollouts) + (heuristic value / rollouts) + (exploration factor)

class Istus
{
public:
    State node;
    int nodes;
    int timeLimit;
    short d;
    short bestcase;
    short score;

    Istus()
    {
        Istus(99);
    }
    Istus(int TL)
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

        auto end = std::chrono::steady_clock::now();
        end += std::chrono::milliseconds(timeLimit);
        while (std::chrono::steady_clock::now() < end && d < 22)
        {
            bestcase = -2;
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
        std::vector<Move> legals = node.legal_moves();
        int pos;
        std::cin >> pos;
        while (std::none_of(legals.begin(), legals.end(), [pos](Move m) { return m == (pos - 1); }))
        {
            print("invalid move.");
            node.show();
            std::cin >> pos;
        }
        return pos - 1;
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
        if (i == -1)
        {
            glyph.node.unplay();
            glyph.node.unplay();
            i = glyph.get_player_move();
            glyph.node.play(i);
            glyph.node.show();
        }
        else
        {
            glyph.node.play(i);
            glyph.node.show();
        }
    }
    glyph.show_result();
}

inline void selfplay(int TL)
{
    Zero engine1 = Zero(TL);
    Zero engine2 = Zero(TL);
    int eturn;
    std::cout << "1 for Zero first, -1 for Istus first.\n--> ";
    std::cin >> eturn;
    while (!engine1.node.is_game_over() && !engine2.node.is_game_over())
    {
        engine1.node.show();
        if (eturn == -1)
        {
            engine1.engine_move();
            engine2.node = engine1.node;
        }
        else
        {
            engine2.engine_move();
            engine1.node = engine2.node;
        }
        eturn = -eturn;
    }
    engine1.node.show();
    engine1.show_result();
}

int main()
{
    // Zero engine4 = Zero(60000);
    // engine4.node.show();
    // std::cout << "Search starting!\n";
    // engine4.engine_move();

    std::cout << "Play against Zero [0] | Play against Istus [1] | Watch a self-play game [2]\n--> ";
    int ans;
    std::cin >> ans;
    std::cout << "milliseconds per move? ";
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