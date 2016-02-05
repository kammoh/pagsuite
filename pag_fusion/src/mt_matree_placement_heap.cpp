#include "mt_matree_placement_heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
using namespace std;

mt_matree_placement_heap::mt_matree_placement_heap(vector<float> *matrix_row){
    this->matrix_row = matrix_row;
    data = new int[matrix_row->size() + 2];
    pointer = 0;
}

mt_matree_placement_heap::~mt_matree_placement_heap(){
    delete[] data;
}


void mt_matree_placement_heap::push(int item){
    pointer ++;
    data[pointer] = item;
    reheap_up(pointer);
}

void mt_matree_placement_heap::reheap_up(int start){
    int p =start;
    int new_p = p / 2;
    while (p > 1 && matrix_row->at(data[new_p]) > matrix_row->at(data[p])){
        int tmp = data[new_p];
        data[new_p] = data[p];
        data[p] = tmp;
        p = p / 2;
        new_p = new_p / 2;
    }
}

void mt_matree_placement_heap::reheap_all(){
    int p;
    for (p = 1; p <= pointer; p++){
        reheap_up(p);
    }
}


void mt_matree_placement_heap::reheap_down(int pointer){
    int new_p;
    if (2 * pointer > this->pointer){
        return;
    }
    if (2 * pointer + 1 <= this->pointer
            && matrix_row->at(data[2*pointer+1]) < matrix_row->at(data[2*pointer])){
        new_p = 2 * pointer + 1;
    } else {
        new_p = 2 * pointer;
    }
    if (matrix_row->at(data[pointer]) > matrix_row->at(data[new_p])){
        int tmp = data[pointer];
        data[pointer] = data[new_p];
        data[new_p] = tmp;
        reheap_down(new_p);
    }
}

int mt_matree_placement_heap::pop(void){
    int res = data[1];
    data[1] = data[pointer];
    pointer--;
    reheap_down(1);
    return res;
}

int mt_matree_placement_heap::pop_preview(void){
    return data[1];
}

void mt_matree_placement_heap::check_heap_condition(void){
    int i;
    for (i = 2; i <= pointer; i++){
        if (matrix_row->at(data[i/2]) > matrix_row->at(data[i])){
            printf("\nError at index %d, %d\n", i, i/2);
            exit(1);
        }

    }

}
