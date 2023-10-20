#include "utils.h"

Weights::Weights(){};

void Weights::init(int num_pred, int random_init){
    TOTAL_WEIGHTS = 100;
    srand(time(0));

    num_weights = num_pred ;
    values = (float *) std::malloc(sizeof(float)*num_weights);
    for(int i=0; i<num_weights; i++){
        if(random_init == 1){
            values[i] = (rand() % TOTAL_WEIGHTS);
        }else{
            values[i] = 0;
        }
    }
}

Weights::~Weights(){
}

void Weights::update(Dataset data, float *y_pred, float learning_rate){
    float scale = learning_rate/data.length;
    for(int i = 0; i < num_weights; i++){
        float sum_res = (sum_residual(data,y_pred,i));
        printf("Sum_residual = %f\n",sum_res);
        values[i] = values[i] - scale*sum_res;
    }
}
