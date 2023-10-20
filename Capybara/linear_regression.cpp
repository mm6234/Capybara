#include "utils.h"

class LinearRegressionModel{
    Dataset data;
    Weights weights;

    public:
        LinearRegressionModel(const Dataset &data_train){
            data.copy(data_train);
            weights.init(data.num_pred, 0);
        }

        void print_weights(){
            char result_str[1000];
            printf("Number weights = %d\n",weights.num_weights);
            strcpy(result_str, "y = ");
            for(int i = 0; i < weights.num_weights; i++){
                printf("Weights %d = %f\n",i,weights.values[i]);
                char weight[20];
                sprintf(weight,"%.2f * x%d",weights.values[i],i);
                strcat(result_str, weight);
                if(i == weights.num_weights-1){
                    strcat(result_str,"\n");
                }else{
                    strcat(result_str," + ");
                }
            }
            printf("%s\n",result_str);
        }

        void train(int max_iters, float learning_rate){
            float *y_pred = (float *) std::malloc(sizeof(float)*data.length);
            while(max_iters > 0){
                fit(y_pred);
                weights.update(data, y_pred, learning_rate);
                
                float mse = mean_squared_error(y_pred,data.y,data.length);

                if(max_iters % 100 == 0){
                    print_weights();
                    std::cout << "There are " << max_iters << "iterations left. \n";
                    std::cout << "MSE = " << mse << "\n";
                }
                max_iters--;
            }
            free(y_pred);
        }

        float predict(float *x){
            float prediction = 0;
                for(int i = 0; i < weights.num_weights; i++){
                    prediction = prediction + weights.values[i]*x[i];
                }
            return prediction;
        }

    private:
        void fit(float *y_pred){
            for(int i = 0; i < data.length; i++){
                y_pred[i] = predict(data.X[i]);
            }
        }
};


int main(){
    int length_train;
    const char* filename = "test.csv";
    std::cout << "Status message: Reading CSV \n";
    Dataset data = read_csv(filename); //need to replace with how API retrieves data from database
    float learning_rate = 0.1;
    int max_iters = 1000;

    std::cout << "Status message: Creating LinearRegressionModel \n";
    LinearRegressionModel lin_model = LinearRegressionModel(data);
    std::cout << "Status message: Training model \n";
    lin_model.train(max_iters, learning_rate);
    
    std::cout << "Status message: Testing \n";
    float X_test[2];
    X_test[0] = 1;
    X_test[1] = 36;
    float y_test = lin_model.predict(X_test);
    lin_model.print_weights();
    std::cout << "Testing for X0 = " << X_test[0] << ", X1 = " << X_test[1] << "\n";
    std::cout << "y = " << y_test << "\n"; 

}