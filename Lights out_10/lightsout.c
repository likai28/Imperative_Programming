//This file is used to implement the lightsout solver
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "lib/contracts.h"
#include "lib/boardutil.h"
#include "lib/bitvector.h"
#include "lib/hdict.h"
#include "lib/queue.h"
#include "board-ht.h"
#include "lib/heap.h"
#include "lib/xalloc.h"

bitvector press_button(bitvector B, int row, int col, uint8_t height, uint8_t width);

void get_row_col(uint8_t i, int* row, int* col,uint8_t height,
                uint8_t width)
{
    REQUIRES( row != NULL && row != NULL);
    *row = i/width;
    *col = i%width;
    (void)height;
    return;
}

int main(int argc, char** argv)
{
    if(argc!=2)
    {
        fprintf(stderr,"usage: lightsout <board name\n");
        return 1;
    }
    char *board_filename = argv[1];
    bitvector* p = malloc(sizeof(bitvector));
    uint8_t width = 0;
    uint8_t height = 0;
    if(file_read( board_filename,p, &width, &height))
    {
        hdict_t H = ht_new(100);
        queue_t Q = queue_new();
        board_data* B = malloc(sizeof(board_data));
        B->board = *p;
        B->test = 0;
        B->prev = NULL;

        enq(Q,(void*)B);
        ht_insert(H,(void*)B);
        //build a all off lights board
        bitvector all_lightsout = bitvector_new();
        while( !queue_empty(Q))
        {
            B = deq(Q);
            for(int row = 0; row < height; row++)
            {
                for(int col =0; col<width;col++)
                {
                    uint8_t i = get_index(row,col,width,height);
                    bitvector board_updated = press_button(B->board,row,col,
                    height,width);
                    //if found the lightsout board
                    if(bitvector_equal(board_updated,all_lightsout))
                    {
                                                                                                        int size = 0;
                        board_data* BD =B;
                        while(BD->prev!=NULL)
                        {
                            size++;
                            BD = BD->prev;
                        }
                                                                                                        BD = B;
                        uint8_t board_res[size+1];
                        for (int j = 0; j< size;j++)
                        {
                            board_res[size-j] = BD->test;
                            BD = BD->prev;
                        }
                        board_res[0]=i;
                        for(int j =0;j<size+1;j++)
                        {
                            int row2;
                            int col2;
                            get_row_col(board_res[j],&row2,&col2,height,width);
                            printf("%d:%d\n",row2,col2);
                        }
                        queue_free(Q,NULL);
                        hdict_free(H);
                        free(p);
                        return 0;
                    }
                    //check whether the board is in the H
                    board_data* po = ht_lookup(H,board_updated);
                    if(po == NULL)
                    {
                        board_data* B_new = malloc(sizeof(board_data));
                        B_new->board = board_updated;
                        B_new->test =i;
                        B_new->prev = B;
                        ht_insert(H,(void*)B_new);
                        enq(Q,(void*)B_new);
                    }
                }
            }
        }
        fprintf(stderr,"No solution\n");
        hdict_free(H);
        queue_free(Q,NULL);
        free(p);
        return 1;
    }
    else
    {
        fprintf(stderr,"The file is not reading successfully\n");
        free(p);
        return 1;
    }
}

bitvector press_button(bitvector B, int row, int col, uint8_t height, uint8_t   width)
{
    bitvector B2 = B;
    uint8_t i =0;
    uint8_t i_right =0;
    uint8_t i_left =0;
    uint8_t i_up =0;
    uint8_t i_down =0;

    if(row==0&&col==0)
    {
        i = get_index(row, col, width, height);
        i_right = get_index(row,col+1,width,height);
        i_down = get_index(row+1, col,width, height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_right);
        B2 = bitvector_flip(B2,i_down);
    }

    else if(row==height-1&&col==0)
    {
        i = get_index(row, col, width, height);
        i_right = get_index(row,col+1,width,height);
        i_up = get_index(row-1, col,width, height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_right);
        B2 = bitvector_flip(B2,i_up);
    }

    else if(row>0&&row<height-1&&col==0)
    {
        i = get_index(row, col, width, height);
        i_right = get_index(row,col+1,width,height);
        i_down = get_index(row+1, col,width, height);
        i_up = get_index(row-1,col,width,height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_right);
        B2 = bitvector_flip(B2,i_down);
        B2 = bitvector_flip(B2,i_up);
    }
    else if(row==0&&col>0&&col<width-1)
    {
        i = get_index(row, col, width, height);
        i_right = get_index(row,col+1,width,height);
        i_left = get_index(row,col-1,width,height);
        i_down = get_index(row+1, col,width, height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_right);
        B2 = bitvector_flip(B2,i_left);
        B2 = bitvector_flip(B2,i_down);
    }
    else if(row==height-1&&col>0&&col<width-1)
    {
        i = get_index(row, col, width, height);
        i_right = get_index(row,col+1,width,height);
        i_left = get_index(row,col-1,width,height);
        i_up = get_index(row-1, col,width, height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_right);
        B2 = bitvector_flip(B2,i_left);
        B2 = bitvector_flip(B2,i_up);
    }
    else if(row==0&&col==width-1)
    {
        i = get_index(row, col, width, height);
        i_left = get_index(row,col-1,width,height);
        i_down = get_index(row+1, col,width, height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_left);
        B2 = bitvector_flip(B2,i_down);
    }
    else if(row==height-1&&col==width-1)
    {
        i = get_index(row, col, width, height);
        i_left = get_index(row,col-1,width,height);
        i_up = get_index(row-1, col,width, height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_left);
        B2 = bitvector_flip(B2,i_up);

    }
    else if(row>0&&row<height-1&&col==width-1)
    {
        i = get_index(row, col, width, height);
        i_left = get_index(row,col-1,width,height);
        i_down = get_index(row+1, col,width, height);
        i_up = get_index(row-1,col,width,height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_left);
        B2 = bitvector_flip(B2,i_down);
        B2 = bitvector_flip(B2,i_up);
    }
    else
    {
        i = get_index(row, col, width, height);
        i_right = get_index(row,col+1,width,height);
        i_left = get_index(row,col-1,width,height);
        i_down = get_index(row+1, col,width, height);
        i_up = get_index(row-1, col, width,height);
        B2 = bitvector_flip(B2,i);
        B2 = bitvector_flip(B2,i_right);
        B2 = bitvector_flip(B2,i_left);
        B2 = bitvector_flip(B2,i_down);
        B2 = bitvector_flip(B2,i_up);
    }
    return B2;
}
