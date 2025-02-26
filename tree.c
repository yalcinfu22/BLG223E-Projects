#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

TreeNode* init_node(GameState* gs) // ALLOCATES A NEW TREE NODE WITH THE GIVEN GAMESTATE 
{
    TreeNode* new_tree_node = (TreeNode*)malloc(sizeof(TreeNode));
    if (new_tree_node == NULL) {
        fprintf(stderr, "Memory allocation failed for TreeNode\n");
        return NULL;
    }
    new_tree_node->game_state = gs;
    new_tree_node->num_children = -1;
    new_tree_node->children = NULL;
    return new_tree_node;
}

TreeNode* init_tree(GameState *gs, int depth) // IT WILL RETURN THE ROOT
{
    if (depth <= 0) return NULL;
    if(!gs) return NULL;

    TreeNode* parent = init_node(gs);

    // THIS WILL HANDLE NO CHILDREN (DRAW) AND WIN CASES THEY ARE THE END STATES SO NO NEED TO DO THE REST
    // Also if the it is the deepest part of the tree it will set it's children to NULL and returns them immediately
    if(get_game_status(gs) != IN_PROGRESS || depth == 1)
    {
        parent->children = NULL;
        return parent;
    }

    bool moves[parent->game_state->width];
    memset(moves, 0, sizeof(bool) * parent->game_state->width);
    parent->num_children = available_moves(gs, moves);

    if (parent->num_children > 0) parent->children = (TreeNode**)malloc(sizeof(TreeNode*) * parent->num_children);
    else 
    { 
        printf("num_children %d is invalid.\n", parent->num_children);
        return NULL;
    }

    for(int i = 0; i < parent->num_children; i++)
    {
        for(int j = 0; j < parent->game_state->width; j++)
        {
            if(moves[j])
            {
                moves[j] = false;
                parent->children[i] = init_tree(make_move(gs, j), depth-1);
                break;
            }
        }
    }
    return parent;
}

int node_count(TreeNode *root) // COUNTS ALL STATES
{
    if(!root->children) return 1;
    int count = 1;

    for(int i = 0; i < root->num_children; i++)
    {
        count += node_count(root->children[i]);
    }

    return count;
}

void free_tree(TreeNode* root)
{
    if (!root) return;

    if (!root->children) // LEFT TO RIGHT SEGMENTATIOPN FAULT ESCAPED
    {
        free_game_state(root->game_state);
        if (root->children) 
        {
            free(root->children); // FREES IF ALLOCATED
            root->children = NULL;
        }
        free(root);
        return;
    }

    for (int i = 0; i < root->num_children; i++)
        free_tree(root->children[i]);

    free(root->children);
    root->children = NULL;
    free_game_state(root->game_state);
    free(root);
    return;
}

void expand_tree(TreeNode *root)
{
    if(!root) return;

    if(get_game_status(root->game_state) != IN_PROGRESS) return;

    if(!root->children)
    {
        bool moves[root->game_state->width];
        memset(moves, 0, sizeof(bool) * root->game_state->width);
        root->num_children = available_moves(root->game_state, moves);

        if (root->num_children > 0) root->children = (TreeNode**)malloc(sizeof(TreeNode*) * root->num_children);
        else 
        { 
            printf("num_children %d is invalid.\n", root->num_children);
            return;
        }

        for(int i = 0; i < root->num_children; i++)
        {
            for(int j = 0; j < root->game_state->width; j++)
            {
                if(moves[j])
                {
                    moves[j] = false;
                    root->children[i] = init_node(make_move(root->game_state, j));
                    break;
                }
            }
        }

        return;
    }

    for(int i = 0; i < root->num_children; i++)
    expand_tree(root->children[i]);

}

void print_tree(TreeNode *root, int level)
{
    if(!root || level < 0) return; 
    // THE CHILDREN ARRAY MAY EXIST (!NULL) BUT THE CHILDRENS ARE MAY NOT YET CREATED SO THEY ARE NULL

    if(level == 0) 
    {  
        printf("Tree at that level\n");
        print_game_state(root->game_state); 
        return;
    }
    
    for (int i = 0; i < root->num_children; i++)
    {
        print_tree(root->children[i], level-1);
    }
}    