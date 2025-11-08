#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <sstream>
#include <list>
#include <algorithm>
#include <iterator>
#include <set>
#include <stack>
#include <cstring>
#include <map>


using namespace std;

vector<vector<int>> binary2;
vector<vector<int>> angles;
vector<vector<int>> edges;
vector<vector<int>> nms;
vector<vector<int>> binary3;
set<int> visited;

map<int, vector<vector<int>>> directions = {
        { -90, {{1, 0}, {-1, 0}} }, 
        {  90, {{1, 0}, {-1, 0}} }, 

        { 45,   {{1, 1}, {-1, -1}} }, 
        { -135, {{1, 1}, {-1, -1}} },

        { -45, {{-1, 1}, {1, -1}} }, 
        { 135, {{-1, 1}, {1, -1}} }, 

        {   0, {{0, -1}, {0, 1}} }, 
        { 180, {{0, -1}, {0, 1}} }
    };

string inputfile = "empire.ppm";
int low = 150;
int high = 200;

string grayfile = "empire_grayscale.ppm";
string hystheresis = "empire_hystheresis.ppm";
string nonmax = "empire_nonmax.ppm";
string binary = "empire_final.ppm";

vector<vector<int>> createPPM(string f) {
    ifstream infile(f);
    vector<string> info;
    string str;
    int height; int width;
    
    while (infile >> str) {
        info.push_back(str);
    }
    width = stoi(info[1]); height = stoi(info[2]);
    
    vector<vector<int>> imageppm(height, vector<int>(width * 3));
    int index = 4;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width * 3; ++j) {
            imageppm[i][j] = stoi(info[index]);
            index += 1;
        }
    }
    cout << info.size() << endl; 
    cout << imageppm.size() << " " << imageppm[0].size() << endl;
    return imageppm;
}

void render(string f, int height, int width, vector<vector<int>> matrix, int max) {
    ofstream myfile;
    myfile.open(f);
    myfile << "P3 " << width << " " << height << " " << max << endl;
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            myfile << matrix [i][j] << " "; 
            myfile << matrix [i][j] << " ";
            myfile << matrix [i][j] << " ";
        }
        myfile << " " << endl;
    }
    myfile.close();
}

vector<vector<int>> grayscale(vector<vector<int>>& matrix, int height, int width) {
    vector<vector<int>> gray(height, vector<int>(1));
    for (size_t i = 0; i < matrix.size(); i++) {
        gray[i].clear();
        for (size_t j = 0; j < matrix[i].size(); j +=3) {
            int avg = (matrix[i][j] + matrix[i][j+1] + matrix[i][j+2])/3;           
            gray[i].push_back(avg);
        }
    }
    return gray;
}

void changeWeak(size_t row, size_t col) {
    if (row < 0 || row >= binary2.size() || col < 0 || col >= binary2[0].size() || binary2[row][col] == 0 || visited.count(binary2[0].size()*row+col)) {
        return;
    }

    binary2[row][col] = 2;
    visited.insert(binary2[0].size()*row + col);

    changeWeak(row - 1, col); // Up
    changeWeak(row + 1, col); // Down
    changeWeak(row, col - 1); // Left
    changeWeak(row, col + 1); // Right

    changeWeak(row - 1, col - 1); // Up
    changeWeak(row - 1, col + 1); // Down
    changeWeak(row + 1, col - 1); 
    changeWeak(row + 1, col + 1); 
}

vector<vector<int>> edgeDetector2(vector<vector<int>>& matrix, int height, int width) {
    vector<vector<int>> Gx(height, vector<int>(width));
    vector<vector<int>> Gy(height, vector<int>(width));
    
    binary2.resize(height, vector<int>(width));
    angles.resize(height, vector<int>(width));
    edges.resize(height, vector<int>(width));
    nms.resize(height, vector<int>(width));
    binary3.resize(height, vector<int>(width));

    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            Gx[i][j] = 0;
            Gy[i][j] = 0;
            edges[i][j] = 0;
        }
    }

    for (size_t i = 0; i < matrix.size() - 2; i++) {
        for (size_t j = 0; j < matrix[i].size() - 2; j++) {
            int sumX = -1*matrix[i][j] + 0 + 1*matrix[i][j+2] +
                      -2*matrix[i+1][j] + 0 + 2*matrix[i+1][j+2] +
                      -1*matrix[i+2][j] + 0 + 1*matrix[i+2][j+2];
            Gx[i+1][j+1] = sumX;

            int sumY = -1*matrix[i][j] + -2*matrix[i][j+1] + -1*matrix[i][j+2] + 
                        0 + 0 + 0 +
                        1*matrix[i+2][j] + 2*matrix[i+2][j+1] + 1*matrix[i+2][j+2];
            Gy[i+1][j+1] = sumY;
        }
    }
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            edges[i][j] = int(sqrt((pow(Gx[i][j], 2) + pow(Gy[i][j], 2))));
            if (edges[i][j] >= high) {
                binary2[i][j] = 2;
            }
            else if (edges[i][j] >= low) {
                binary2[i][j] = 1;
            }
            else {
                binary2[i][j] = 0;
            }
        }
    }

    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = 0; j < matrix[i].size(); j++) {
            //angles[i][j] = int(atan2(Gy[i][j], Gx[i][j]));
            angles[i][j] = int(atan2(Gy[i][j], Gx[i][j]) * 57.2958);
            //cout << int(atan2(Gy[i][j], Gx[i][j])) << " " << angles[i][j] << endl;
        }
    }
    vector<int> checkangles = {0, 45, 90, 135, 180, -135, -90, -45};

    for (size_t i = 0; i < angles.size(); i++) {
        for (size_t j = 0; j < angles[i].size(); j++) {
            //vector<int> subtracted;
            int minimum = abs(angles[i][j] - 0);
            int minAngle;
            for (auto angle : checkangles) {
                if (angles[i][j] == angle) {
                    minAngle = angle;
                    break;
                }
                if ((abs(angles[i][j] - angle)) < minimum) {
                    minimum = abs(angles[i][j] - angle);
                    minAngle = angle;            
                }
            }
            angles[i][j] = minAngle;
        }
    }
    for (size_t i = 0; i < angles.size(); i++) {
        for (size_t j = 0; j < angles[i].size(); j++) {
            if (i == 0 || j == 0 || i == (height - 1) || j == (width - 1)) {
                continue;
            }
            vector<vector<int>> tocheck = directions[angles[i][j]];
            vector<int> v1 = tocheck[0]; vector<int> v2 = tocheck[1];
            int first = edges[i+v1[0]][j+v1[1]];
            int second = edges[i+v2[0]][j+v2[1]];
            if (edges[i][j] > first && edges[i][j] > second) {
                nms[i][j] = 1;
            }
            else {
                nms[i][j] = 0;
            }

        }
    }
    return binary2;
}

void render2(string f, int height, int width, vector<vector<int>> matrix, int max) {
    ofstream myfile;
    myfile.open(f);
    myfile << "P3 " << width << " " << height << " " << max << endl;
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            if (matrix[i][j] == 2) {
                myfile << 1 << " "; 
                myfile << 1 << " ";
                myfile << 1 << " ";
            }
            else {
                myfile << 0 << " "; 
                myfile << 0 << " ";
                myfile << 0 << " ";
            }
        }
        myfile << " " << endl;
    }
    myfile.close();
}
void render3(string f, int height, int width, vector<vector<int>> matrix, int max) {
    ofstream myfile;
    myfile.open(f);
    myfile << "P3 " << width << " " << height << " " << max << endl;
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            if (matrix[i][j] == 1) {
                myfile << 1 << " "; 
                myfile << 1 << " ";
                myfile << 1 << " ";
            }
            else {
                myfile << 0 << " "; 
                myfile << 0 << " ";
                myfile << 0 << " ";
            }
        }
        myfile << " " << endl;
    }
    myfile.close();
}

void beginPipeline() {
    vector<vector<int>> imageppm = createPPM(inputfile);
    cout << "created ppm" << endl;
    vector<vector<int>> gray = grayscale(imageppm, imageppm.size(), imageppm[0].size());
    render(grayfile, gray.size(), gray[0].size(), gray, 255);

    vector<vector<int>> edges = edgeDetector2(gray, gray.size(), gray[0].size());
    for (size_t i = 0; i < binary2.size(); i++) {
        for (size_t j = 0; j < binary2[0].size(); j++) {
            if (binary2[i][j] == 2) {
                changeWeak(i, j);
            }
        }
    }
    for (size_t i = 0; i < binary2.size(); i++) {
        for (size_t j = 0; j < binary2[0].size(); j++) {
            if (nms[i][j] == 1 && binary2[i][j] == 2) {
                binary3[i][j] = 1;
            }
            else {
                binary3[i][j] = 0;
            }
        }
    }
    
    render2(hystheresis, binary2.size(), binary2[0].size(), binary2, 1);
    render3(nonmax, nms.size(), nms[0].size(), nms, 1);
    render3(binary, binary3.size(), binary3[0].size(), binary3, 1);
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        for (int i=0; i<argc; i++) {
            if (strcmp("-f", argv[i]) == 0) {
                cout << "hello" << endl;
                inputfile = argv[i+1];
            }
            else if (strcmp("-lt", argv[i]) == 0) {
                low = stoi(argv[i+1]);
            }
            else if (strcmp("-ht", argv[i]) == 0) {
                high = stoi(argv[i+1]);
            }
            else if (strcmp("-fg", argv[i]) == 0) {
                grayfile = argv[i+1];
            }
            else if (strcmp("-of", argv[i]) == 0) {
                hystheresis = argv[i+1];
            }
            else if (strcmp("-f2", argv[i]) == 0) {
                nonmax = argv[i+1];
            }
            else if (strcmp("-ff", argv[i]) == 0) {
                binary = argv[i+1];
            }
        }
        
    }
    beginPipeline();
    return 0;
}