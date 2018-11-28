#ifndef SCM_SOLUTIONS_HPP
#define SCM_SOLUTIONS_HPP

//#define MAX_SCM_CONST 524287
//#define MAX_SCM_CONST 1048575
//#define MAX_SCM_CONST 317361 //limitation in Hcub solutions file


typedef enum {ADD,MUL,LF2,LF3} graph_type_t;

/*
struct scm_solution_t
{
  int adders;
  graph_type_t gt;
  int op_a;
  int op_b;
};

extern scm_solution_t solutions[];
*/
extern int solutions_adders[];
extern graph_type_t solutions_gt[];
extern int solutions_op_a[];
extern int solutions_op_b[];
#endif // SCM_SOLUTIONS_HPP
