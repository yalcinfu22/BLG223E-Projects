// ONLY MODIFY THE "apply_move_to_tree" FUNCTION

#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

//FREE THE UNNECESSARY PART, EXPAND ONCE, AND DECIDE FOR EXPANSION ACCORDING TO THE NODE COUNT
void apply_move_to_tree(TreeNode **root, int move, int initial_tree_depth)
{
    TreeNode* old_root = *root;
    TreeNode* new_root = old_root->children[move];

    old_root->children[move] = NULL; // NEW ROOT IS DISCONNECTED FROM THE OLD ROOT
    free_tree(old_root); 

    expand_tree(new_root); // ***PROBLEM HERE***

    if((int)pow(new_root->game_state->width, initial_tree_depth - 2) > node_count(new_root)) 
        expand_tree(new_root);
    
    *root = new_root;
}


void print_leaf(TreeNode* root) // PRINTS THE LEAF SCORES
{
    if (!root) return;

    for(int i = 0; i < root->num_children; i++)
    {
       if (root->children[i]) print_leaf(root->children[i]);
       else 
       {
        printf("Evalulation score is %d\n", root->game_state->evaluation);
        print_game_state(root->game_state); 
       }
    }

    if(!root->children) // TO EVALUATE ENDED GAMES WITH NO CHILDREN
    {
        printf("Evalulation score is %d\n", root->game_state->evaluation);
        print_game_state(root->game_state);         
    }

    return;
}

int get_random_move(GameState *gs) // GIVES A VALID MOVE INDEX PROVIDED THAT THERE IS A VALID MOVE
{
    // This function assumes that there is at least one available move
    bool moves[gs->width];
    memset(moves, false, gs->width * sizeof(bool));
    int move_count = available_moves(gs, moves);

    return rand() % move_count;
}

int get_human_move(GameState *gs)  // GIVES THE CORRECT INDEX AMONG THE CHILDREN (NEXT STATES OF THE TREE)
{
    bool moves[gs->width];
    memset(moves, 0, gs->width * sizeof(bool));
    available_moves(gs, moves);
    
    printf("Available moves: ");
    for (int i = 0; i < gs->width; i++)
    {
        if (moves[i])
            printf("%d ", i);
    }

    char c;
    printf("\nEnter your move: ");
    c = getchar();
    getchar(); // dump newline character
    int human_move = c - '0';

    // get the index of the move among the children, not the all moves
    int move_among_children = 0;

    for (int i = 0; i <= human_move; i++)
    {
        if (moves[i])
        {
            move_among_children++;
        }
    }
    
    return move_among_children - 1;
}

void play_game(int game_width, int game_height, int tree_depth, bool play_against_human)
{
    // Init game state
    GameState *state = init_game_state(game_width, game_height);

    // Init tree
    TreeNode *root = init_tree(state, tree_depth);
    int move;
    
    // While game is not over
    while (get_game_status(root->game_state) == IN_PROGRESS)
    {
        // Get the move
        if (root->game_state->next_turn) // Player 2 MOVES IF NEXT_TURN IS TRUE
        {
            if (play_against_human)
                move = get_human_move(root->game_state);
            else
                move = get_random_move(root->game_state);
        }
        else // Player 1
        {
            move = best_move(root);
        }
        // Apply the move
        apply_move_to_tree(&root, move, tree_depth); // MY JOB

        // Print the board
        printf("Node Count: %d\n", node_count(root));
        print_game_state(root->game_state);

    }

    // Declare the winner
    if (get_game_status(root->game_state) == DRAW)
        printf("It is a draw\n");
    else
        printf("Player %d won!\n", get_game_status(root->game_state) + 1);

    free_tree(root); // MY JOB
}
