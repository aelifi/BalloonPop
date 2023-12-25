#include <stdio.h>
#include "bpgame.h"


struct bpgame{
	int nrows;
	int ncols;
	char** currentBoard;
	int score;
	BPGame* pastBoard;
};

BPGame * bp_create(int nrows, int ncols){
	BPGame* head = malloc(sizeof(BPGame));
	head->currentBoard = malloc(sizeof(char*)*nrows);
	for (int i=0; i < nrows; i++){
		head->currentBoard[i] = malloc(sizeof(char)*ncols);
		for (int j=0; j < ncols; j++){
			int dice = rand() % 4;
			if (dice == 0){
				head->currentBoard[i][j] = Red;
			}
			if (dice == 1){
				head->currentBoard[i][j] = Blue;
			}
			if (dice == 2){
				head->currentBoard[i][j] = Green;
			}
			if (dice == 3){
				head->currentBoard[i][j] = Yellow;
			}
		}
	}
	
	head->pastBoard = NULL;

	head->nrows = nrows;
	head->ncols = ncols;
	head->score = 0;
	return head;
};

BPGame * bp_create_from_mtx(char mtx[MAX_ROWS][MAX_COLS], int nrows, int ncols){
	BPGame* head = malloc(sizeof(BPGame));
	head->currentBoard = malloc(sizeof(char*)*nrows);
	for (int i=0; i < nrows; i++){
		head->currentBoard[i] = malloc(sizeof(char)*ncols);
		for (int j=0; j < ncols; j++){
			head->currentBoard[i][j] = mtx[i][j];
		}
	}
	
	head->pastBoard = NULL;
	
	head->nrows = nrows;
	head->ncols = ncols;
	head->score = 0;
	return head;
};

void bp_destroy(BPGame * b){
	for (int i=0; i < b->nrows; i++){
		free(b->currentBoard[i]);
		
	}
	free(b->currentBoard);
	free(b);
};

void bp_display_STD(BPGame *b) {
    for(int i = 0; i < b->nrows; i++){
        for(int j = 0; j < b->ncols; j++) {
           printf("%c", b->currentBoard[i][j]);
        }
        printf("\n");
    }
}

void bp_display(BPGame * b){
	for(int i = 0; i < b->nrows; i++){
        for(int j = 0; j < b->ncols; j++) {
           printf("%c", b->currentBoard[i][j]);
        }
        printf("\n");
    }
}

int helper_pop(BPGame * b, int r, int c, char color){
	if (bp_get_balloon(b,r,c) == -1 || b->currentBoard[r][c] == None){
		return 0;
	}
	int score = 1;
	if (b->currentBoard[r][c] == color){
		b->currentBoard[r][c] = None;
		score += helper_pop(b,r-1,c,color);
		score += helper_pop(b,r,c-1,color);
		score += helper_pop(b,r,c+1,color);
		score += helper_pop(b,r+1,c,color);
	}else{
		score = 0;
	}
	return score;
}

int bp_pop(BPGame * b, int r, int c){
	if (bp_get_balloon(b,r,c) == -1 || b->currentBoard[r][c] == None){
		return 0;
	}
	
	BPGame* temp = b->pastBoard;
	b->pastBoard = bp_create(b->nrows,b->ncols);
	for (int i=0; i < b->nrows; i++){
		for (int j=0; j < b->ncols; j++){
			b->pastBoard->currentBoard[i][j] = b->currentBoard[i][j];
		}
	}
	b->pastBoard->score = b->score;
	b->pastBoard->pastBoard = temp;
	
	
	
	int score = 0;
	if (bp_get_balloon(b,r,c) == bp_get_balloon(b,r-1,c) ||
		bp_get_balloon(b,r,c) == bp_get_balloon(b,r,c-1) ||
		bp_get_balloon(b,r,c) == bp_get_balloon(b,r,c+1) ||
		bp_get_balloon(b,r,c) == bp_get_balloon(b,r+1,c) ){
		
		score += helper_pop(b, r, c, b->currentBoard[r][c]);
	}
	b->score += score * (score-1);
	return score;
};

int bp_is_compact(BPGame * b){
	for (int j=0; j < b->ncols; j++){//col major
		int shouldBeAir = 0;
		for (int i=0; i < b->nrows; i++){
			if (b->currentBoard[i][j] == None && !shouldBeAir){
				shouldBeAir = 1;
			}
			if (b->currentBoard[i][j] != None && shouldBeAir){
				return 0;
			}
		}
	} 
	return 1;
};

void bp_float_one_step(BPGame * b){
  for(int i = 1; i < b->nrows; i++){// 0 to 12
    for(int j = 0; j < b->ncols; j++){// 0 to 15
       if(b->currentBoard[i][j] != None && b->currentBoard[i-1][j] == None){
                    b->currentBoard[i-1][j] = b->currentBoard[i][j];
                    b->currentBoard[i][j] = None;
                }
    }
  }
}

int bp_score(BPGame * b){
	return b->score;
};

int bp_get_balloon(BPGame * b, int r, int c){
	if (r < 0 || r >= b->nrows || c < 0 || c >= b->ncols){
		return -1;
	}
	return b->currentBoard[r][c];
};
  
int bp_can_pop(BPGame * b){
	for(int i = 0; i < b->nrows; i++){
        for(int j = 0; j < b->ncols; j++) {
            if (b->currentBoard[i][j] != None){
				if ( (b->currentBoard[i][j] == b->currentBoard[i-1][j] && i-1 >= 0) ||
					 (b->currentBoard[i][j] == b->currentBoard[i][j-1] && j-1 >= 0) ||
					 (b->currentBoard[i][j] == b->currentBoard[i+1][j] && i+1 < b->nrows) ||
					 (b->currentBoard[i][j] == b->currentBoard[i][j+1] && j+1 < b->ncols) ){
					
					return 1;
                }
			}
        }
    }
	return 0;
};

int bp_undo(BPGame * b){
	if (b->pastBoard == NULL){
		return 0;
	}
	
	BPGame* temp = b->pastBoard->pastBoard;
	for(int i = 0; i < b->nrows; i++){
        for(int j = 0; j < b->ncols; j++) {
			printf("");
            b->currentBoard[i][j] = b->pastBoard->currentBoard[i][j];
        }
    }
	b->score = b->pastBoard->score;
	b->pastBoard = temp;
	
    return 1;
}
