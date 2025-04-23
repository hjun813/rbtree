#include "rbtree.h"

#include <stdlib.h>

void rb_insert_fixup(rbtree *t, node_t *z);
void rb_erase_fixup(rbtree *t, node_t *x);
void rb_left_rotate(rbtree *t, node_t *z);
void rb_right_rotate(rbtree *t, node_t *z);
void delete_node(rbtree *t, node_t *node);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
node_t *rbtree_subtree_min(const rbtree *t, node_t *node);
int inorder_fill(const node_t *node, const rbtree *t, key_t *arr, size_t n, size_t *idx);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  p->nil = (node_t*)malloc(sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  p->nil->parent = p->nil;
  p->root = p->nil;
  return p;
}

void delete_node(rbtree *t, node_t *node){
  // 노드 전체 삭제
  if(node == t->nil){
    return;
  } 
  delete_node(t, node->left);
  delete_node(t, node->right);

  free(node);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  if(t == NULL){
    return ;
  }
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *insertNode = (node_t *)malloc(sizeof(node_t)); // 삽입할 노드 만들기
  insertNode->key = key; //key 설정
  
  node_t *cur = t->root; // 포인터로 insertNode 삽입할 위치 찾기
  node_t *curParent = t->nil;

  while (cur != t->nil) // 트리 끝까지 내려가기
  {
      curParent = cur;
      if(insertNode->key < cur->key){ // 왼쪽으로
          cur = cur->left;
      }
      else{ // 오른쪽으로
          cur = cur->right;
      }
  } // 트리 끝 도달 -> 삽입할 위치

  insertNode->parent = curParent;

  if(curParent == t->nil){ // 맨 처음 예외처리
      t->root = insertNode;
  }
  // 부모는 정해놨는데 아직 왼쪽에 달지 오른쪽에 달지 못정함
  else if(insertNode->key < curParent->key){ // 작으면 왼쪽에 달기
      curParent->left  = insertNode;
  }
  else{
      curParent->right = insertNode;  // 크거나 같으면 오른쪽에 달기
  }
  //insertNode 나머지 설정
  insertNode->left = t->nil;
  insertNode->right = t->nil;
  insertNode->color = RBTREE_RED;

  // 부모가 빨간색이면 문제되니까 
  rb_insert_fixup(t, insertNode);
  
  return insertNode; // 삽입한 자리 반환
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;

  while (cur != t->nil) {
    if (key == cur->key) {
      return cur;
    } else if (key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  return NULL; // 찾지 못한 경우
}


node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  // 왼쪽 노드가 없을 때 까지 내려가라
  node_t * cur = t->root;
  if(cur == t->nil){
      return NULL;
  }
  while (cur->left != t->nil)
  {
      cur = cur->left;
  }
  return cur;
}

node_t *rbtree_subtree_min(const rbtree *t, node_t *node) {
  // TODO: implement find
  // 후임자 찾는 함수
  // 왼쪽 노드가 없을 때 까지 내려가라
  node_t * cur = node;
  if(cur == t->nil){
      return cur;
  }
  while (cur->left != t->nil)
  {
      cur = cur->left;
  }
  return cur;
}


node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  // 오른쪽 노드가 없을 때 까지 내려가라
  node_t * cur = t->root;
  if(cur == t->nil){
      return NULL;
  }
  while (cur->right != t->nil)
  {
      cur = cur->right;
  }
  return cur;
}

int rbtree_erase(rbtree *t, node_t *p) { // BST 삭제와 같음
  // TODO: implement erase
  node_t *y = p; // y는 실제로 삭제되거나 후임자
  node_t *x; // y자리를 대체할 노드 (fix up 대상)
  int y_original_color = y->color; 
  
  if(p->left == t->nil){ // 자식 1명(왼쪽이 없는 경우), 0명도 여기서 걸림
    x = p->right;
    rbtree_transplant(t, p, p->right); // 오른쪽 이어주기
  }
  else if(p->right == t->nil){ // 자식 1명(오른쪽이 없는 경우)
    x = p->left;
    rbtree_transplant(t, p, p->left); // 왼쪽 이어주기
  }
  else{ // 자식 2명
    y = rbtree_subtree_min(t, p->right); // 후임자 찾기 y는 이제 후임자
    y_original_color = y->color; // 후임자 색 저장
    x = y->right; 

    if(y != p->right){ // 후임자가 바로 오른쪽 자식이 아니라면 
      rbtree_transplant(t, y, y->right); // 후임자 대신 오른쪽 이어주기
      y->right = p->right;
      y->right->parent = y;
    }
    else{
      x->parent = y; // ?  여기서 부터 밑에 다시 봐서 이해하기 <<
    }
    rbtree_transplant(t, p, y); //y 가 p 대체
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  // 삭제 노드의 색이 black 일때 문제 
  if(y_original_color == 1){
    rb_erase_fixup(t, x);
  }

  return 0;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){ // u를 v로 대체
  if(u->parent == t->nil){ // u가 루트일때
    t->root = v;
  }
  else if(u == u->parent->left){ // u가 부모의 왼쪽 자식일때 
    u->parent->left = v;
  }
  else{ // u가 부모의 오른쪽 자식일때 
    u->parent->right = v;
  }
  v->parent = u->parent;
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////???
int inorder_fill(const node_t *node, const rbtree *t, key_t *arr, size_t n, size_t *idx) {
  // 중위 순회
  if (node == t->nil || *idx >= n) return 0;

  inorder_fill(node->left, t, arr, n, idx);

  if (*idx < n) {
    arr[*idx] = node->key;
    (*idx)++;
  }
  
  inorder_fill(node->right, t, arr, n, idx);

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  size_t idx = 0;
  inorder_fill(t->root, t, arr, n, &idx);
  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void rb_insert_fixup(rbtree *t, node_t *z){ // rbtree에서 z노드를 검사해라?

  while (z->parent->color == RBTREE_RED) //z 부모가 빨간색일때만 문제가 됨
  {
      if(z->parent == z->parent->parent->left){ // 부모가 왼쪽 자식이라면 (1. 반대 경우 조건)
          node_t* uncle = z->parent->parent->right; // 삼촌
          if(uncle->color == RBTREE_RED){ // 삼촌이 RED (부모 R, 삼촌 R) (2. 삼촌 색깔 조건)
              z->parent->color = RBTREE_BLACK;
              uncle->color = RBTREE_BLACK;
              z->parent->parent->color = RBTREE_RED;
              z = z->parent->parent;
          } else{ // 삼촌이 BLACK
              if(z == z->parent->right){ // 꺽여있는 상황이라면
                  z = z->parent;
                  rb_left_rotate(t, z); // 펴기 (회전인데 red,red니까 색 변환 생략)
              }
              z->parent->color = RBTREE_BLACK; // 부모와 조부모 색 바꾸고 회전
              z->parent->parent->color = RBTREE_RED;
              rb_right_rotate(t, z->parent->parent);
          }
      } else{ // 부모가 오른쪽 자식
          node_t* uncle = z->parent->parent->left; // 삼촌은 왼쪽
          if(uncle->color == RBTREE_RED){ // 부모 R 삼촌 R
              z->parent->color = RBTREE_BLACK;
              uncle->color = RBTREE_BLACK;
              z->parent->parent->color = RBTREE_RED;
              z = z->parent->parent; // 루트가 되어서 RED일수 있으니까
          } else { //부모 R 삼촌 B
              if(z == z->parent->left){ //꺽여있음
                  z = z->parent;
                  rb_right_rotate(t, z);
              }
              z->parent->color = RBTREE_BLACK;
              z->parent->parent->color = RBTREE_RED;
              rb_left_rotate(t, z->parent->parent);
          }
          
      }
  }
  t->root->color = RBTREE_BLACK;  // 루트 검은색
}

void rb_erase_fixup(rbtree *t, node_t *x){ // x노드가 tree에서 규칙을 지키는가?
  // 삭제된 색이 black일때만 호출됨
  // 지금 x 가 doubly-black인건가?
  while ((x != t->root) && (x->color == RBTREE_BLACK))
  {
    if(x == x->parent->left){ // 왼쪽
      node_t *w = x->parent->right; // x의 형제
      if(w->color == RBTREE_RED){ // 형제가 빨간색이면
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED; // 회전하기 전에 색 바꾸기
        rb_left_rotate(t, x->parent); // 회전
        w = x->parent->right;
      }

      if((w->left->color == RBTREE_BLACK) && (w->right->color == RBTREE_BLACK)){
        // 형제의 자식이 모두 BLACK
        w->color = RBTREE_RED; // 형제 빨간색
        x = x->parent;
      } else {
        if(w->right->color == RBTREE_BLACK){ //왼쪽자식:red, 오른쪽자식:BLACK
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rb_right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        rb_left_rotate(t, x->parent);    
        x = t->root;
      }
    }
    else{ // 오른쪽
      node_t *w = x->parent->left; // x의 형제
      if(w->color == RBTREE_RED){ // 형제가 빨간색이면
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED; // 회전하기 전에 색 바꾸기
        rb_right_rotate(t, x->parent); // 회전
        w = x->parent->left;
      }

      if((w->right->color == RBTREE_BLACK) && (w->left->color == RBTREE_BLACK)){
        // 형제의 자식이 모두 BLACK
        w->color = RBTREE_RED; // 형제 빨간색
        x = x->parent;
      } else {
        if(w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          rb_left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        rb_right_rotate(t, x->parent);    
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

void rb_left_rotate(rbtree *t, node_t *z){ // z가 부모 노드여야 하고  

  if( t == NULL || z == NULL || z->right == t->nil){
    return;
  }
  node_t* temp = z->right;
  z->right = temp->left; // z의 오른쪽에 temp 왼쪽 달기

  if(temp->left != t->nil){ 
    temp->left->parent = z; // temp 왼쪽 부모 정해주고
  }
  temp->parent = z->parent; // temp 부모 정해주고
  if(z->parent == t->nil){
    t->root = temp;
  }
  else if(z == z->parent->left){
    z->parent->left = temp; // z가 왼쪽 자식이었다면 temp가 새로운 왼쪽 자식
  }
  else{
    z->parent->right = temp; // z가 오른쪽 자식이었다면 temp가 새로운 오른쪽 자식
  }
  temp->left = z; // temp 왼쪽에다가 z 연결
  z->parent = temp; // z 부모 설정 

}

void rb_right_rotate(rbtree *t, node_t *z){

  if(t==NULL || z == NULL || z->left == t->nil){
    return;
  }
  node_t* temp = z->left;
  z->left = temp->right;

  if(temp->right != t->nil){
    temp->right->parent = z;
  }
  temp->parent = z->parent;
  if(z->parent == t->nil){
    t->root = temp;
  }
  else if(z == z->parent->right){
    z->parent->right = temp;
  }
  else{
    z->parent->left = temp;
  }
  temp->right = z;
  z->parent = temp;

}