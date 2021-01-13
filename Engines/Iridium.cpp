#pragma GCC optimize("Ofast", "unroll-loops", "omit-frame-pointer", "inline")
#pragma GCC target("avx")

#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "Coin.hpp"
#include "Glyph.hpp"
#include "UTTT.hpp"

using namespace UTTT;

constexpr auto EXP_FACTOR = gameexpfactor;

class TreeNode {
   public:
    int winScore = 0;
    int visits = 0;
    short playerNo;
    State board;
    TreeNode *parent = nullptr;
    std::vector<TreeNode *> children;

    TreeNode(const State &board) {
        this->board = board;
    }

    void set_player_no(const short playerNo) {
        this->playerNo = playerNo;
    }

    auto get_player_no() -> short {
        return playerNo;
    }

    auto get_opponent() -> short {
        return -playerNo;
    }

    void set_parent(TreeNode *parent) {
        this->parent = parent;
    }

    void set_state(const State &board) {
        this->board = board;
    }

    void show() {
        std::cout << "My state is:\n";
        board.show();
        if (parent) {
            std::cout << "My parent's state is:\n";
            parent->show();
        }
        std::cout << "and I have " << children.size() << " children.\n";
    }

    auto get_children() -> std::vector<TreeNode *> {
        return children;
    }

    void expand() {
        children.reserve(board.num_legal_moves());  // do this in the constructor, but only if you figure out how to make arrays work
        for (const auto &move : board.legal_moves()) {
            board.play(move);
            children.push_back(new TreeNode(board));
            children.back()->set_parent(this);
            children.back()->set_player_no(get_opponent());
            board.unplay();
        }
    }

    auto get_win_score() -> int {
        return winScore;
    }

    auto get_visit_count() -> int {
        return visits;
    }

    auto get_parent_visits() -> int {
        return parent->get_visit_count();
    }

    void increment_visits() {
        visits++;
    }

    void add_score(const int s) {
        winScore += s;
    }

    void set_win_score(const int s) {
        winScore = s;
    }

    auto get_parent() -> TreeNode * {
        return parent;
    }

    auto get_state() -> State {
        return board;
    }

    auto random_child() -> TreeNode * {
        return children[rand() % children.size()];
    }

    auto get_winrate() -> double {
        return (double)winScore / (double)visits;
    }

    auto best_child() -> TreeNode * {
        std::vector<TreeNode *>::iterator result;
        result = std::max_element(
            children.begin(), children.end(),
            [](TreeNode *a, TreeNode *b) { return (a->get_visit_count() < b->get_visit_count()); });
        return children[std::distance(children.begin(), result)];
    }

    auto best_child_as_move() -> Move {
        std::vector<TreeNode *>::iterator result;
        result = std::max_element(
            children.begin(), children.end(),
            [](TreeNode *a, TreeNode *b) { return (a->get_visit_count() < b->get_visit_count()); });
        // if (board.legal_moves().size() != children.size()) {
        //     std::cout << board.legal_moves().size() << " " << children.size() << '\n';
        //     board.show_debug();
        //     std::terminate();
        // }
        return board.legal_moves()[std::distance(children.begin(), result)];
    }

    void show_child_winrates() {
        for (const auto &child : children) {
            std::cout << child->get_win_score() << " ";
        }
        std::cout << "\n";
    }

    void show_child_visitrates() {
        for (const auto &child : children) {
            std::cout << child->get_visit_count() << " ";
        }
        std::cout << "\n";
    }
};

namespace UCT {
auto uct_value(const int totalVisit, const double nodeWinScore, const int nodeVisit) -> double {
    if (nodeVisit == 0) {
        return INT_MAX;
    }
    return ((double)nodeWinScore / (double)nodeVisit) + 1.41 * sqrt(log(totalVisit) / (double)nodeVisit) * EXP_FACTOR;
}

auto uct_compare(TreeNode *const a, TreeNode *const b) -> bool {
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

auto best_node_uct(TreeNode *const node) -> TreeNode * {
    auto children = node->get_children();
    std::vector<TreeNode *>::iterator result;
    result = std::max_element(
        children.begin(), children.end(),
        uct_compare);
    return children[std::distance(children.begin(), result)];
}
};  // namespace UCT

class MCTS {
   public:
    const short WIN_SCORE = 10;
    long long timeLimit;        // limiter on search time
    const bool memsafe = true;  // dictates whether we preserve a part of the tree across moves
    short opponent;             // the win score that the opponent wants
    short reward;               // the win score that the agent wants
    int nodes = 0;
    TreeNode *preservedNode = nullptr;

    MCTS() {
        MCTS(1);
    }
    MCTS(const short player) {
        MCTS(player, 3);
    }
    MCTS(const short player, const long long strength) {
        srand(time(NULL));
        timeLimit = strength;
        opponent = -player;
        reward = player;
    }

    void deleteTree(TreeNode *root) {
        /* first delete the subtrees */
        for (TreeNode *child : root->children) {
            deleteTree(child);
        }
        /* then delete the node */
        delete root;
    }

    void set_opponent(const short i) {
        opponent = i;
        reward = -i;
    }

    auto prune(TreeNode *parent, const State &target) -> TreeNode * {
        TreeNode *out = nullptr;
        bool found = false;
        for (TreeNode *child : parent->get_children()) {
            if (!found && child->get_state() == target) {
                out = child;
                found = true;
            } else
                deleteTree(child);
        }
        if (out)
            out->set_parent(nullptr);
        delete parent;
        return out;
    }

    auto find_best_next_board(const State board) -> State {
        nodes = 0;
        set_opponent(-board.turn);
        // an end time which will act as a terminating condition
        auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeLimit);
        TreeNode *rootNode = nullptr;
        if (preservedNode)
            rootNode = prune(preservedNode, board);
        if (!rootNode) {
            rootNode = new TreeNode(board);
            rootNode->set_state(board);
            rootNode->set_player_no(opponent);
        }
        // if you start getting weird out_of_range() errors at low TC then expand the root node here
        // auto breakunit = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() < end) {
            TreeNode *promisingNode = select_promising_node(rootNode);

            if (!promisingNode->get_state().is_game_over())
                expand_node(promisingNode);

            TreeNode *nodeToExplore = promisingNode;

            if (promisingNode->get_children().size() != 0)
                nodeToExplore = promisingNode->random_child();

            short playoutResult = simulate_playout(nodeToExplore);
            backpropagate(nodeToExplore, playoutResult);

            /*if (std::chrono::steady_clock::now() > breakunit){
            //     std::cout << "The best move so far is: " << rootNode->best_child_as_move() + 1 << '\n';
                breakunit += std::chrono::milliseconds(500);
                rootNode->show_child_visitrates();
            }*/
        }
        State out = rootNode->best_child()->get_state();
        std::cerr << "ZERO:\n";
        std::cerr << nodes << " nodes processed.\n";
        std::cerr << "Zero win prediction: " << (int)(rootNode->best_child()->get_winrate() * (100 / WIN_SCORE)) << "%\n";
        short action, sboard, square, row, col;
        action = rootNode->best_child_as_move();
        // assert(action >= 0 && action <= 80);
        std::cout << action << '\n';
        square = action % 9;
        sboard = action / 9;
        col = (sboard % 3) * 3 + square % 3;
        row = (sboard / 3) * 3 + square / 3;
        std::cout << row << " " << col << std::endl;
        // rootNode->show_child_visitrates();
        if (!memsafe) {
            deleteTree(rootNode);
        } else {
            preservedNode = prune(rootNode, out);
        }
        return out;
    }

    inline auto select_promising_node(TreeNode *const rootNode) -> TreeNode * {
        TreeNode *node = rootNode;
        while (node->get_children().size() != 0)
            node = UCT::best_node_uct(node);
        return node;
    }

    inline void expand_node(TreeNode *node) {
        node->expand();
    }

    inline void backpropagate(TreeNode *nodeToExplore, const short winner) {
        TreeNode *tempNode = nodeToExplore;
        while (tempNode) {
            tempNode->increment_visits();
            if (tempNode->get_player_no() == winner) {
                tempNode->add_score(WIN_SCORE);
            }
            tempNode = tempNode->get_parent();
        }
    }

    inline auto simulate_playout(TreeNode *node) -> short {
        nodes++;
        State tempState = node->get_state();
        tempState.mem_setup();
        short boardStatus = tempState.evaluate();
        if (boardStatus == opponent) {
            node->get_parent()->set_win_score(INT_MIN);
            return boardStatus;
        }
        while (!tempState.is_game_over()) {
            tempState.random_play();
        }
        boardStatus = tempState.evaluate();
        return boardStatus;
    }
};

class Zero {
   public:
    MCTS searchDriver = MCTS();
    State node = State();

    Zero() {
        Zero(99);
    }
    Zero(const long long strength) {
        searchDriver.timeLimit = strength;
    }

    inline void print(const std::string input, const std::string end = "\n") {
        std::cout << input << end;
    }

    auto get_player_move() -> int {
        const std::vector<Move> legals = node.legal_moves();
        std::vector<Move> shiftedLegals;
        std::transform(legals.begin(), legals.end(), std::back_inserter(shiftedLegals), [](Move n) { return n + 1; });
        std::cout << "Your legal moves are: " << string(shiftedLegals) << "\n--> ";
        int pos;
        std::cin >> pos;
        while (std::none_of(legals.begin(), legals.end(), [pos](Move m) { return m == (pos - 1); })) {
            print("invalid move.");
            node.show();
            std::cin >> pos;
        }
        return pos - 1;
    }

    void engine_move() {
        node = searchDriver.find_best_next_board(node);
    }

    void show_result() const {
        switch (node.evaluate()) {
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

class Istus {
   public:
    State node;
    int nodes;
    int timeLimit;
    short d;
    short bestcase;
    short score;

    Istus() {
        Istus(99);
    }
    Istus(const int TL) {
        timeLimit = TL;
    }

    auto negamax(int depth = 10, int colour = 1, int a = -2, int b = 2) -> int  //WORKING
    {
        if (depth < 1) {
            nodes++;
            return colour * node.evaluate();
        }

        if (node.is_game_over()) {
            nodes++;
            return colour * node.evaluate() * depth;
        }
        int score;

        node.pass_turn();                              // MAKE A NULL MOVE
        score = -negamax(depth - 3, -colour, -b, -a);  // PERFORM A LIMITED SEARCH
        node.pass_turn();                              // UNMAKE NULL MOVE
        if (score > a)
            a = score;
        if (a >= b)
            return a;

        for (const auto &move : node.legal_moves()) {
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

    void engine_move()  //WORKING
    {
        nodes = 0;
        bestcase = -2;
        Move bestmove;
        d = 0;

        auto end = std::chrono::steady_clock::now();
        end += std::chrono::milliseconds(timeLimit);
        while (std::chrono::steady_clock::now() < end && d < 22) {
            bestcase = -2;
            for (const auto &move : node.legal_moves()) {
                node.play(move);
                score = -negamax(d, node.turn);
                node.unplay();
                if (bestcase < score) {
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

    void reset_nodes() {
        nodes = 0;
    }

    void show_result() {
        int r;
        r = node.evaluate();
        if (r == 0)
            std::cout << "1/2-1/2" << '\n';
        else if (r == 1)
            std::cout << "1-0" << '\n';
        else
            std::cout << "0-1" << '\n';
    }

    void print(const std::string input, const std::string end = "\n") {
        std::cout << input << end;
    }

    auto get_player_move() -> int {
        std::vector<Move> legals = node.legal_moves();
        int pos;
        std::cin >> pos;
        while (std::none_of(
            legals.begin(),
            legals.end(),
            [pos](Move m) { return m == (pos - 1); })) {
            print("invalid move.");
            node.show();
            std::cin >> pos;
        }
        return pos - 1;
    }
};

auto get_first_player() -> bool {
    bool player;
    std::cout << "Is the human player going first? [1/0]"
              << "\n";
    std::cin >> player;
    return player;
}

inline void run_negamax_game(const long long TL) {
    Istus glyph = Istus(TL);
    Move i;
    glyph.node.show();
    if (get_first_player()) {
        i = glyph.get_player_move();
        glyph.node.play(i);
        glyph.node.show();
    }
    while (!glyph.node.is_game_over()) {
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

inline void run_mcts_game(const long long TL) {
    Zero glyph = Zero(TL);
    Move i;
    glyph.node.show();
    if (get_first_player()) {
        i = glyph.get_player_move();
        glyph.node.play(i);
        glyph.node.show();
    }
    while (!glyph.node.is_game_over()) {
        glyph.engine_move();
        glyph.searchDriver.nodes = 0;
        glyph.node.show();
        if (glyph.node.is_game_over())
            break;
        i = glyph.get_player_move();
        if (i == -1) {
            glyph.node.unplay();
            glyph.node.unplay();
            i = glyph.get_player_move();
            glyph.node.play(i);
            glyph.node.show();
        } else {
            glyph.node.play(i);
            glyph.node.show();
        }
    }
    glyph.show_result();
}

inline void selfplay(const long long TL) {
    Zero engine1 = Zero(TL);
    Zero engine2 = Zero(TL);
    int eturn = 1;
    // std::cout << "1 for Zero first, -1 for Istus first.\n--> ";
    // std::cin >> eturn;
    while (!engine1.node.is_game_over() && !engine2.node.is_game_over()) {
        engine1.node.show();
        std::cout << string(engine1.node.legal_moves()) << '\n';
        std::cout << string(engine2.node.legal_moves()) << '\n';
        std::cout << "efficient counter: "
                  << engine1.node.num_legal_moves()
                  << " actual counter: "
                  << engine1.node.legal_moves().size()
                  << '\n';
        // assert(engine1.node.num_legal_moves() == engine1.node.legal_moves().size());
        if (eturn == -1) {
            engine1.engine_move();
            engine2.node = engine1.node;
        } else {
            engine2.engine_move();
            engine1.node = engine2.node;
        }
        eturn = -eturn;
    }
    engine1.node.show();
    engine1.show_result();
}

inline void userplay() {
    Zero game = Zero();
    game.node.show();
    while (!game.node.is_game_over() && !game.node.is_game_over()) {
        int i;
        i = game.get_player_move();
        game.node.play(i);
        game.node.show();
    }
    game.node.show();
    game.show_result();
}

inline void testsuite() {
    Zero game = Zero();
    while (!game.node.is_game_over()){
        std::cout << "\nposition legal moves: " 
                  << game.node.legal_moves().size()
                  << "\nfast move counter: "
                  << game.node.num_legal_moves()
                  << "\nactual list of moves: "
                  << string(game.node.legal_moves())
                  << "\nstate of play (is game over?): "
                  << game.node.is_game_over()
                  << '\n';
        assert(game.node.legal_moves().size() == game.node.num_legal_moves());
        game.node.random_play();
    }
}

int main() {
    std::cout << "Play against Zero [0] | Play against Istus [1] | Watch a self-play game [2] | Play with a friend [3] | Run tests [4]\n--> ";
    int ans;
    std::cin >> ans;
    std::cout << "milliseconds per move? ";
    long long TL;
    std::cin >> TL;
    switch (ans) {
        case 0:
            run_mcts_game(TL);
            break;

        case 1:
            run_negamax_game(TL);
            break;

        case 2:
            selfplay(TL);
            break;

        case 3:
            userplay();
            break;

        case 4:
            testsuite();
            break;

        default:
            break;
    }

    return 0;
}