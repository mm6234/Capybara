#include "utils.h"


Dataset::Dataset(){

}

Dataset::Dataset(float **X_train,float *y_train, int length_train, int size_pred_train){
    X = (float **) malloc(sizeof(float*)*length_train);
    for(int i = 0; i < length_train; i++){
        X[i] = (float *) malloc(sizeof(float)*size_pred_train);
        std::memcpy(X[i], X_train[i], sizeof(float)*size_pred_train);
    }

    y = (float *) malloc(sizeof(float)*length_train);
    std::memcpy(y, y_train, sizeof(float)*length_train);
    
    length = length_train;
    num_pred = size_pred_train;
}

void Dataset::copy(const Dataset &data){
    
    X = (float **) malloc(sizeof(float*)*data.length);
    for(int i = 0; i < data.length; i++){
        X[i] = (float *) malloc(sizeof(float)*data.num_pred);
        std::memcpy(X[i], data.X[i], sizeof(float)*data.num_pred);
    }

    y = (float *) malloc(sizeof(float)*data.length);
    std::memcpy(y, data.y, sizeof(float)*data.length);
    
    length = data.length;
    num_pred = data.num_pred;
}

Dataset::~Dataset(){
    
}

void Dataset::print_dataset(){
    
    for(int i = 0; i < length; i++){
        printf("row = %d: \n",i);
        for(int j = 0; j < num_pred; j++){
            printf("X%d = %f\n",j,X[i][j]);
        }
        printf("Y = %f\n",y[i]);
    }

    
}
