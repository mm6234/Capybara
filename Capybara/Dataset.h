class Dataset{
    public:
        float **X;
        float *y;
        int num_pred;
        int length;

        Dataset();
        Dataset(float **X_train,float *y_train, int length_train, int size_pred_train);
        void copy(const Dataset &data);
        ~Dataset();

        void print_dataset();
};