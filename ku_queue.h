#define INT_MIN 2147483647

struct Node {
    struct Node *link;
    int data;
};

struct Queue {
    struct Node *front;
    struct Node *rear;
    int size;
    int max;
    int swapable;
};

void ku_h_init_queue(struct Queue *q, int max) {
    q -> front = NULL;
    q -> rear = NULL;
    q -> size = 0;
    q -> max = max;
    q -> swapable = 1; // swap 가능 1, swap 불가능 0
}

int ku_h_enqueue(struct Queue *q, int data){
    if(q -> size == q -> max) return -1;

    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode -> data = data;

    if(q -> front == NULL)
        q -> front = newNode;
    else
        q -> rear -> link = newNode;

    q -> rear = newNode;
    q -> rear -> link = q -> front;
    return 0;
}

int ku_h_dequeue(struct Queue *q) {
    if(q -> front == NULL)
        return -1;
    
    int data;
    if(q -> front == q -> rear) { // 큐에 1개 들어있음
        data = q -> front -> data;
        free(q->front);
        q-> front = NULL;
        q-> rear = NULL;
    } else {
        struct Node *temp = q-> front;
        data = temp -> data;
        q-> front = q->front->link;
        q->rear->link = q->front;
        free(temp);
    }

    return data;
}

void ku_h_display_queue(struct Queue *q) 
{ 
    struct Node *temp = q->front;
    printf("\nElements in Circular Queue are: \n"); 
    while (temp->link != q->front) 
    { 
        printf("%d ", temp->data); 
        temp = temp->link; 
    } 
    printf("%d\n", temp->data); 
}

int ku_h_isfull(struct Queue *q) {
    return (q -> size) == (q -> max);
}

int ku_h_size(struct Queue *q) {
    return q -> size;
}
