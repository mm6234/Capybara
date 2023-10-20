class Weights{
    private:
        int TOTAL_WEIGHTS;

    public:
        float* values;
        int num_weights;

        Weights();
        void init(int num_pred, int random_init);
        ~Weights();
        void update(Dataset data, float *y_pred, float learning_rate);
};