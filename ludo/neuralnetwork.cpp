#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <iostream>
#include <fstream>

using namespace std;



struct Matrix
{
    vector<vector<double>> mat;
    int rows;
    int cols;
    
    Matrix()
    {
        rows = 0;
        cols = 0;
    }
    
    Matrix(int n, int m) //init to zero
    {
        mat.resize(n);
        for (int i = 0; i < mat.size(); i++)
            mat[i] = vector<double>(m, 0.0);
        
        rows = n;
        cols = m;
    }
    
    Matrix(int n, int m, double val) //init to specific value
    {
        mat.resize(n);
        for (int i = 0; i < mat.size(); i++)
            mat[i] = vector<double>(m, val);
        
        rows = n;
        cols = m;
    }
    
    Matrix operator*(Matrix b)
    {
        if (cols == b.rows)
        {
            Matrix res(rows, b.cols);
            for (int i = 0; i < res.rows; i++)
                for (int j = 0; j < res.cols; j++)
                    for (int k = 0; k < cols; k++)
                        res.mat[i][j] += mat[i][k] * b.mat[k][j];
            return res;
        }
        else
        {
            cout << "[Matrix::operator*()]: Matrix dimension don't match!" << endl;
            return Matrix();
        }
    }
    
    Matrix operator*(double c)
    {
        Matrix res(rows, cols);
        for (int i = 0; i < res.rows; i++)
            for (int j = 0; j < res.cols; j++)
                res.mat[i][j] = mat[i][j] * c;
        return res;
    }
    
    Matrix operator+(Matrix b)
    {
        if (rows == b.rows && cols == b.cols)
        {
            Matrix res(rows, cols);
            for (int i = 0; i < res.rows; i++)
                for (int j = 0; j < res.cols; j++)
                    res.mat[i][j] = mat[i][j] + b.mat[i][j];
            return res;
        }
        else
        {
            cout << "[Matrix::operator+()]: Matrix dimension don't match!" << endl;
            return Matrix();
        }
    }
    
    Matrix operator-(Matrix b)
    {
        if (rows == b.rows && cols == b.cols)
        {
            Matrix res(rows, cols);
            for (int i = 0; i < res.rows; i++)
                for (int j = 0; j < res.cols; j++)
                    res.mat[i][j] = mat[i][j] - b.mat[i][j];
            return res;
        }
        else
        {
            cout << "[Matrix::operator-()]: Matrix dimension don't match!" << endl;
            return Matrix();
        }
    }
    
    Matrix T()
    {
        Matrix res(cols, rows);
        for (int i = 0; i < res.rows; i++)
            for (int j = 0; j < res.cols; j++)
                res.mat[i][j] = mat[j][i];
        return res;
    }
    
    void Print(int fieldsize)
    {
        if (fieldsize > 0)
        {
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    cout << setw(fieldsize) << mat[i][j];
                }
                cout << endl;
            }
        }
        else
        {
            cout << "[Matrix::Print()]: Illegal field size!" << endl;
        }
    }
};

struct ANN
{
    vector<Matrix> W; //weights
    vector<Matrix> b; //biases
    
    vector<Matrix> n; //net inputs
    vector<Matrix> a; //outputs
    vector<Matrix> s; //sensitivities
    
    Matrix p; //input
    
    //-------------------
    // Transfer function
    //-------------------
    Matrix Sigmoid(Matrix anN) //sigmoidal transfer function
    {
        Matrix res(anN.rows, 1);
        for (int i = 0; i < res.rows; i++)
        {
            //hyperbolic tangent sigmoid (tansig)
            if (anN.mat[i][0] < -5.0)
            {
                res.mat[i][0] = -1.0;
            }
            else if (anN.mat[i][0] > 5.0)
            {
                res.mat[i][0] = 1.0;
            }
            else //only call exp() in the safe range, otherwise it may blow up and return nan!
            {
                double tmp1 = exp(anN.mat[i][0]);
                double tmp2 = exp(-anN.mat[i][0]);
                res.mat[i][0] = (tmp1 - tmp2) / (tmp1 + tmp2);
            }
            
            //sigmoid (logsig)
//            if (anN.mat[i][0] <= -6.0)
//            {
//                res.mat[i][0] = 0.0;
//            }
//            else if (anN.mat[i][0] >= 6.0)
//            {
//                res.mat[i][0] = 1.0;
//            }
//            else //only call exp() in the safe range, otherwise it may blow up and return nan!
//            {
//                res.mat[i][0] = 1.0 / (1.0 + exp(-anN.mat[i][0]));
//            }
        }
        return res;
    }
    
    Matrix SigmoidDot(Matrix anN) //derivative of the sigmoidal transfer function
    {
        Matrix tmp = Sigmoid(anN);
        Matrix res(tmp.rows, 1);
        for (int i = 0; i < tmp.rows; i++)
        {
            //1 - f^2 (tansig)
            res.mat[i][0] = 1.0 - (tmp.mat[i][0] * tmp.mat[i][0]);
            
            //f (1 - f) (logsig)
            //res.mat[i][0] = tmp.mat[i][0] * (1.0 - tmp.mat[i][0]);
        }
        return res;
    }
    
    Matrix SigmoidDotDiag(Matrix anN) //return diagonal matrix with the elements of TFDot()
    {
        Matrix tmp = SigmoidDot(anN);
        Matrix res(tmp.rows, tmp.rows);
        for (int i = 0; i < tmp.rows; i++)
            res.mat[i][i] = tmp.mat[i][0];
        return res;
    }
    
    //--------------
    // Core network
    //--------------
    ANN()
    {
    }
    
    ANN(int inputs, vector<int> layerSizes)
    {
        if (layerSizes.size() > 0)
        {
            W.resize(layerSizes.size());
            for (int i = 0; i < W.size(); i++)
            {
                if (i == 0)
                    W[i] = Matrix(layerSizes[i], inputs);
                else
                    W[i] = Matrix(layerSizes[i], layerSizes[i - 1]);
            }
            
            b.resize(layerSizes.size());
            for (int i = 0; i < b.size(); i++)
                b[i] = Matrix(layerSizes[i], 1);
            
            n.resize(layerSizes.size());
            a.resize(layerSizes.size());
            s.resize(layerSizes.size());
        }
        else
        {
            cout << "[ANN()]: Too few layers!" << endl;
        }
    }
    
    void FeedForward()
    {
        if (p.rows == W[0].cols && p.cols == 1)
        {
            for (int i = 0; i < a.size(); i++)
            {
                if (i == 0) //first
                {
                    n[i] = W[i]*p + b[i];
                    a[i] = Sigmoid(n[i]);
                }
                else if (i != a.size() - 1) //middle
                {
                    n[i] = W[i]*a[i - 1] + b[i];
                    a[i] = Sigmoid(n[i]);
                }
                else //last
                {
                    n[i] = W[i]*a[i - 1] + b[i];
                    a[i] = n[i]; //linear
                }
            }
        }
        else
        {
            cout << "[ANN::FeedForward()]: Illegal input dimensions!" << endl;
        }
    }
    
    Matrix CalcOutput(Matrix input)
    {
        if (input.rows == W[0].cols && input.cols == 1)
        {
            p = input;
            FeedForward();
            return a[a.size() - 1];
        }
        else
        {
            cout << "[ANN::CalcOutput()]: Illegal input dimensions!" << endl;
            return Matrix();
        }
    }
    
    void BackProp() //propagate sensitivities backwards
    {
        //s[s.size() - 1] = SigmoidDot(n[n.size() - 1]); //problem specific implementation! (using sigmoidal transfer function)
        
        Matrix tmp(n[n.size() - 1].rows, 1, 1.0);
        s[s.size() - 1] = tmp; //(using linear transfer function)
        
        for (int i = s.size() - 2; i >= 0; i--) //this is general backpropagation
            s[i] = SigmoidDotDiag(n[i]) * (W[i + 1].T() * s[i + 1]);
    }
    
    //----------
    // Learning
    //----------
    //single point
    void Update(double delta, double alpha) //problem specific implementation!
    {
        for (int i = 0; i < W.size(); i++)
        {
            if (i == 0)
            {
                W[i] = W[i] + ((s[i] * p.T()) * delta * alpha);
                b[i] = b[i] + (s[i] * delta * alpha);
            }
            else
            {
                W[i] = W[i] + ((s[i] * a[i - 1].T()) * delta * alpha);
                b[i] = b[i] + (s[i] * delta * alpha);
            }
        }
    }
    
    void Train(Matrix input, Matrix inputPrime, double reward, double gamma, double alpha) //problem specific implementation!
    {
        if (input.rows == W[0].cols && input.cols == 1)
        {
            double Q_statePrime = CalcOutput(inputPrime).mat[0][0];
            double Q_state = CalcOutput(input).mat[0][0]; //calculate Q_state after Q_statePrime, so that n and a from the call to FeedForward() are correct for BackProp() and Update()!
            BackProp();
            
            double delta = reward + (gamma * Q_statePrime) - Q_state;
            Update(delta, alpha);
        }
        else
        {
            cout << "[ANN::Train()]: Illegal input dimensions!" << endl;
        }
    }
    
    //batch
    void Gradient(vector<Matrix> &dW, vector<Matrix> &db, Matrix input, double Q_statePrime, double reward, double gamma, double alpha) //calculate the networks gradient for a single point
    {
        if (input.rows == W[0].cols && input.cols == 1)
        {
            double Q_state = CalcOutput(input).mat[0][0]; //calculate Q_state after Q_statePrime, so that n and a from the call to FeedForward() are correct for BackProp()!
            BackProp();
            
            double delta = reward + (gamma * Q_statePrime) - Q_state;
            
            for (int i = 0; i < W.size(); i++)
            {
                if (i == 0)
                {
                    dW[i] = dW[i] + ((s[i] * p.T()) * delta * alpha);
                    db[i] = db[i] + (s[i] * delta * alpha);
                }
                else
                {
                    dW[i] = dW[i] + ((s[i] * a[i - 1].T()) * delta * alpha);
                    db[i] = db[i] + (s[i] * delta * alpha);
                }
            }
        }
        else
        {
            cout << "[ANN::Gradient()]: Illegal input dimensions!" << endl;
        }
    }
    
    void Update(vector<Matrix> dW, vector<Matrix> db) //update the network according to a given gradient
    {
        if (dW.size() == W.size() && db.size() == b.size())
        {
            for (int i = 0; i < W.size(); i++)
            {
                if (i == 0)
                {
                    W[i] = W[i] + dW[i];
                    b[i] = b[i] + db[i];
                }
                else
                {
                    W[i] = W[i] + dW[i];
                    b[i] = b[i] + db[i];
                }
            }
        }
        else
        {
            cout << "[ANN::Update()]: Illegal input dimensions!" << endl;
        }
    }
    
    //---------------
    // Miscellaneous
    //---------------
    void InitToRand(double min, double max)
    {
        random_device gen;
        uniform_real_distribution<double> dist(min, max);
        
        for (int i = 0; i < W.size(); i++)
            for (int j = 0; j < W[i].rows; j++)
                for (int k = 0; k < W[i].cols; k++)
                    W[i].mat[j][k] = dist(gen);
        
        for (int i = 0; i < b.size(); i++)
            for (int j = 0; j < b[i].rows; j++)
                for (int k = 0; k < b[i].cols; k++)
                    b[i].mat[j][k] = dist(gen);
    }
    
    void Save(string path)
    {
        std::ofstream outputFile;
        
        outputFile.open(path + "header.txt", ios::out);
        if (outputFile.is_open())
        {
            outputFile << W[0].cols << "\n"; //input size
            for (int i = 0; i < W.size(); i++)
                outputFile << W[i].rows << "\n"; //layer size
            
            outputFile.close();
        }
        else
        {
            cout << "[ANN::Save()]: Couldn't open file!" << endl;
        }
        
        outputFile.open(path + "W.txt", ios::out);
        if (outputFile.is_open())
        {
            for (int i = 0; i < W.size(); i++)
                for (int j = 0; j < W[i].rows; j++)
                    for (int k = 0; k < W[i].cols; k++)
                        outputFile << W[i].mat[j][k] << "\n";
            
            outputFile.close();
        }
        else
        {
            cout << "[ANN::Save()]: Couldn't open file!" << endl;
        }
        
        outputFile.open(path + "b.txt", ios::out);
        if (outputFile.is_open())
        {
            for (int i = 0; i < b.size(); i++)
                for (int j = 0; j < b[i].rows; j++)
                    outputFile << b[i].mat[j][0] << "\n";
            
            outputFile.close();
        }
        else
        {
            cout << "[ANN::Save()]: Couldn't open file!" << endl;
        }
    }
    
    void Load(string path)
    {
        vector<string> tmp;
        string line;
        ifstream inputFile;
        
        ANN res;
        
        inputFile.open(path + "header.txt", ios::in);
        if (inputFile.is_open())
        {
            while (getline(inputFile, line))
                tmp.push_back(line);
            inputFile.close();
            
            int inputs = stoi(tmp[0]);
            vector<int> layerSizes;
            for (int i = 1; i < tmp.size(); i++)
                layerSizes.push_back(stoi(tmp[i]));
            
            res = ANN(inputs, layerSizes);
        }
        else
        {
            cout << "[ANN::Load()]: Couldn't open file!" << endl;
        }
        
        if (res.W.size() > 0)
        {
            tmp.clear();
            inputFile.open(path + "W.txt", ios::in);
            if (inputFile.is_open())
            {
                while (getline(inputFile, line))
                    tmp.push_back(line);
                inputFile.close();
                
                vector<double> weights;
                for (int i = 0; i < tmp.size(); i++)
                    weights.push_back(stod(tmp[i]));
                
                vector<int> offsets;
                offsets.push_back(0);
                for (int i = 1; i < res.W.size(); i++)
                    offsets.push_back((res.W[i - 1].rows * res.W[i - 1].cols) + offsets[i - 1]);
                
                for (int i = 0; i < res.W.size(); i++)
                    for (int j = 0; j < res.W[i].rows; j++)
                        for (int k = 0; k < res.W[i].cols; k++)
                            res.W[i].mat[j][k] = weights[offsets[i] + (j * res.W[i].cols) + k];
            }
            else
            {
                cout << "[ANN::Load()]: Couldn't open file!" << endl;
            }
        }
        
        if (res.b.size() > 0)
        {
            tmp.clear();
            inputFile.open(path + "b.txt", ios::in);
            if (inputFile.is_open())
            {
                while (getline(inputFile, line))
                    tmp.push_back(line);
                inputFile.close();
                
                vector<double> biases;
                for (int i = 0; i < tmp.size(); i++)
                    biases.push_back(stod(tmp[i]));
                
                vector<int> offsets;
                offsets.push_back(0);
                for (int i = 1; i < res.b.size(); i++)
                    offsets.push_back(res.b[i - 1].rows + offsets[i - 1]);
                
                for (int i = 0; i < res.b.size(); i++)
                    for (int j = 0; j < res.b[i].rows; j++)
                        res.b[i].mat[j][0] = biases[offsets[i] + j];
            }
            else
            {
                cout << "[ANN::Load()]: Couldn't open file!" << endl;
            }
        }
        
        W = res.W;
        b = res.b;
        
        n = res.n;
        a = res.a;
        s = res.s;
    }
};
