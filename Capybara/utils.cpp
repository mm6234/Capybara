#include "utils.h"


Dataset read_csv(const char* filename){
    float **X;
    float *y;
    int index = 0;
    int length = 0;
    int num_pred = 0; //number of predictors

    // Reading File to get the number of x and y data points, and mallocating space
    std::ifstream infile(filename);
    std::string line;
    while (std::getline(infile, line)){
        length++;
        if(length == 1){
            int i = 0;
            while(line[i] != '\0'){
                if(line[i] == ','){
                    num_pred++;
                }
                i++;
            }
        }
    }
    infile.close();

    X = (float **) malloc(sizeof(float*)*length);
    for(int i = 0; i < length; i++){
        X[i] = (float *) malloc(sizeof(float)*num_pred);
    }
    y = (float *) malloc(sizeof(float)*length);

    // Extract x and y
    char comma;
    std::ifstream samefile(filename);
    int curr_ind = 0;
    while(std::getline(samefile,line)){

        std::stringstream line_stream(line);
        int curr_pred = 0;
        float number;
        while (line_stream >> number)
        {
            if(curr_pred == num_pred){
                y[curr_ind] = number;
            }
            else{
                X[curr_ind][curr_pred] = number;
                curr_pred++;
            }

            if(line_stream.peek() == ','){
                line_stream.ignore();
            }

        }
        curr_ind++;
    }
    samefile.close();

    Dataset df = Dataset(X,y,length,num_pred);
    return df;
}

float mean(float *y, int length){
    float total = 0;
    for(int i = 0; i < length; i++){
        total = total + y[i];
    }
    return (total/length);
}

float sum_residual(Dataset data, float *y_pred, int current_predictor){
    float total = 0;
    float residual;
    for(int i = 0 ; i < data.length; i++){
        residual = (y_pred[i] - data.y[i]);
        total = total + residual*data.X[i][current_predictor];
    }
    return total;
}

float sum_of_square(float *y, int length){
    float total = 0;
    float residual;
    float y_mean = mean(y,length);
    for(int i = 0 ; i < length; i++){
        residual = (y[i] - y_mean);
        total = total + (residual*residual);
    }
    return total;
}

float residual_sum_of_square(float *y_pred, float *y_true, int length){
    float total = 0;
    float residual;
    for(int i = 0 ; i < length; i++){
        residual = (y_true[i] - y_pred[i]);
        total = total + (residual*residual);
    }
    return total;
}

int calculate_r2(float *y_pred, float *y_true, int length){
    float sum_squared_residual = residual_sum_of_square(y_pred,y_true,length);
    float sum_squared_total = sum_of_square(y_true,length);
    return (1 - ((sum_squared_residual/sum_squared_total)));
}

float mean_squared_error(float *y_pred, float *y_true, int length){
    return residual_sum_of_square(y_pred,y_true,length)/length;
}

float intercept_sum(float *y_pred, float *y_true, int length){
    float total = 0;
    float residual;
    for(int i = 0 ; i < length; i++){
        residual = (y_pred[i] - y_true[i]);
        total = total + residual;
    }
    return total;
}

float slope_sum(float *x, float *y_pred, float *y_true, int length){
    float total = 0;
    float residual;
    for(int i = 0 ; i < length; i++){
        residual = (y_pred[i] - y_true[i]);
        total = total + residual*x[i];
    }
    return total;
}
