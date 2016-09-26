#ifndef MYHEAP_H
#define MYHEAP_H

#include <vector>
using namespace std;

class mt_matree_placement_heap {
    public:
        mt_matree_placement_heap(vector<float> *matrix_row);

        ~mt_matree_placement_heap();

        void push(int item);

        int pop(void);
        int pop_preview(void);
        int pointer;
    protected:
        vector<float> *matrix_row;
        int *data;


        void reheap_up(int start);
        void reheap_down(int pointer);
        void reheap_all();

        void check_heap_condition(void);
};

#endif // MYHEAP_H
