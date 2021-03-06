#include <iostream>
#include <fstream>
#include <cassert>
#include <iomanip>
#include <cmath>
#include <stack>
#include <queue>
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))
#define epsilon (1.0e-6)

class homogeneous_point
{
public:
    double x, y, z, w;

    // set the three coordinates, set w to 1
    homogeneous_point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }

    /*
    default constructor. does nothing. allows declarations like below:
        matrix m;
    therefore, usage is dangerous
    */
    homogeneous_point() {
    }

    // constructs a homogeneous point with given coordinates. forces w to be 1.0
    // if w is zero, raises error
    homogeneous_point(double x, double y, double z, double w)
    {
        assert (w != 0);
        this->x = x/w;
        this->y = y/w;
        this->z = z/w;
        this->w = 1;
    }

    // adds two points. returns a point forcing w to be 1.0
    homogeneous_point operator+ (const homogeneous_point& point)
    {
        double x = this->x + point.x;
        double y = this->y + point.y;
        double z = this->z + point.z;
        double w = this->w + point.w;
        homogeneous_point p(x, y, z, w);
        return p;
    }

    // subtracts one point from another. returns a point forcing w to be 1.0
    homogeneous_point operator- (const homogeneous_point& point)
    {
        double x = this->x - point.x;
        double y = this->y - point.y;
        double z = this->z - point.z;
        double w = this->w - point.w;
        homogeneous_point p(x, y, z, w);
    }

    // Print the coordinates of a point. exists for testing purpose.
    void print()
    {
        cout << "Point: " << endl;
        cout << x << " " << y << " " << z << " " << w << endl;
    }

};


class Vector
{
public:
    double x, y, z;

    // constructs a vector with given components
    Vector(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // keeps the direction same. recalculates the vector to be unit.
    void normalize()
    {
        double r = sqrt(x*x + y*y + z*z);
        x = x / r;
        y = y / r;
        z = z / r;
    }

    // add two vectors
    Vector operator+(const Vector& v)
    {
        Vector v1(x+v.x, y+v.y, z+v.z);
        return v1;
    }

    // subtract one vector from another
    Vector operator-(const Vector& v)
    {
        Vector v1(x-v.x, y-v.y, z-v.z);
        return v1;
    }

    // scale a vector with a given coefficient
    Vector operator* (double m)
    {
        Vector v(x*m, y*m, z*m);
        return v;
    }

    // get the dot product of two vectors
    static double dot(Vector a, Vector b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    // get the cross product of two vectors
    static Vector cross(Vector a, Vector b)
    {
        Vector v(a.y*b.z - a.z*b.y, b.x*a.z - b.z*a.x, a.x*b.y - a.y*b.x);
        return v;
    }

    // print a vector. only for testing purposes.
    void print ()
    {
        cout << "Vector" << endl;
        cout << x << " " << y << " " << z << endl;
    }
};


/*
The matrices are forced to be 4x4. This is because in this assignment, we will deal with points in triangles.
Maximum # of points that we will deal with at once is 3. And all the standard matrices are 4x4 (i.e. scale, translation, rotation etc.)
*/
class matrix
{
public:
    double values[4][4];
    int num_rows, num_cols;

    // only set the number of rows and cols
    matrix(int rows, int cols)
    {
        assert (rows <= 4 && cols <= 4);
        num_rows = rows;
        num_cols = cols;
    }

    // prepare an nxn square matrix
    matrix(int n)
    {
        assert (n <= 4);
        num_rows = num_cols = n;
    }

    // prepare and return an identity matrix of size nxn
    static matrix make_identity(int n)
    {
        assert (n <= 4);
        matrix m(n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (i == j)
                    m.values[i][j] = 1;
                else
                    m.values[i][j] = 0;
            }
        }
        return m;
    }

    // print the matrix. exists for testing purposes
    void print()
    {
        cout << "Matrix:" << endl;
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                cout << values[i][j] << "\t";
            }
            cout << endl;
        }
    }

    // add the two matrices. Raise error if dimension mismatches
    matrix operator+ (const matrix& m)
    {
        assert (this->num_rows == m.num_rows);
        assert (this->num_cols == m.num_cols);

        matrix m1(num_rows, num_cols);
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                m1.values[i][j] = values[i][j] + m.values[i][j];
            }
        }
        return m1;
    }

    // subtract a matrix from another. raise error if dimension mismatches
    matrix operator- (const matrix& m)
    {
        assert (this->num_rows == m.num_rows);
        assert (this->num_cols == m.num_cols);

        matrix m1(num_rows, num_cols);
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                m1.values[i][j] = values[i][j] - m.values[i][j];
            }
        }
        return m1;
    }

    // multiply two matrices. allows statements like m1 = m2 * m3; raises error is dimension mismatches
    matrix operator* (const matrix& m)
    {
        assert (this->num_cols == m.num_rows);
        matrix m1(this->num_rows, m.num_cols);

        for (int i = 0; i < m1.num_rows; i++) {
            for (int j = 0; j < m1.num_cols; j++) {
                double val = 0;
                for (int k = 0; k < this->num_cols; k++) {
                    val += this->values[i][k] * m.values[k][j];
                }
                m1.values[i][j] = val;
            }
        }
        return m1;
    }

    // multiply a matrix with a constant
    matrix operator* (double m)
    {
        matrix m1(this->num_rows, this->num_cols);
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                m1.values[i][j] = m * this->values[i][j];
            }
        }
        return m1;
    }

    // multiply a 4x4 matrix with a homogeneous point and return the resulting point.
    // usage: homogeneous_point p = m * p1;
    // here, m is a 4x4 matrix, intended to be the transformation matrix
    // p1 is the point on which the transformation is being made
    // p is the resulting homogeneous point
    homogeneous_point operator* (const homogeneous_point& p)
    {
        assert (this->num_rows == this->num_cols && this->num_rows == 4);

        matrix m(4, 1);
        m.values[0][0] = p.x;
        m.values[1][0] = p.y;
        m.values[2][0] = p.z;
        m.values[3][0] = p.w;

        matrix m1 = (*this)*m;
        homogeneous_point p1(m1.values[0][0], m1.values[1][0], m1.values[2][0], m1.values[3][0]);
        return p1;
    }

    // return the transpose of a matrix
    matrix transpose()
    {
        matrix m(num_cols, num_rows);
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                m.values[j][i] = values[i][j];
            }
        }
        return m;
    }

};

/*
A simple class to hold the color components, r, g, b of a certain shade.
*/
class color {
public:
    double r, g, b;
    color(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    color() {
    }

    void print(){
        cout<<"Color:"<<endl;
        cout<<r<<" "<<g<<" "<<b<<endl;
    }
};


double eye_x, eye_y, eye_z;
double look_x, look_y, look_z;
double up_x, up_y, up_z;
double fov_x, fov_y, aspectRatio, near, far;
color backgroud;
int screen_x, screen_y;

vector<color> clrs;
vector<color> finalclrs;

bool comp(const homogeneous_point& lhs, const homogeneous_point& rhs)
{
  return lhs.y > rhs.y;
}

homogeneous_point mapped(homogeneous_point p){
    double maxY=p.y;
    maxY=(maxY+1)*(screen_y/2);
    p.y=maxY;

    double maxX=p.x;
    maxX=(maxX+1)*(screen_x/2);

    p.x=maxX;

    return p;
}

homogeneous_point pointKnownY(homogeneous_point p3,homogeneous_point p2, double y){
    homogeneous_point ans;
    ans.w=1;
    ans.y=y;
    ans.x=p2.x + ((p3.x-p2.x)/(p3.y-p2.y))*(y-p2.y);
    ans.z=p2.z + ((p3.z-p2.z)/(p3.y-p2.y))*(y-p2.y);
    //ans.print();
    return ans;
}


int toint(double awm){
    int x=int(awm);
    double prec=awm-x;
    if(prec >0.5){
        return x+1;
    }
    else return x;

}

void scan_convert() {
    ifstream stage3;
    stage3.open("stage3.txt");
    cout<<clrs.size()<<endl;
    cout<<finalclrs.size()<<endl;
    color** pixels = new color*[screen_x];
    double** zs = new double*[screen_x];
    for (int i = 0; i < screen_x; i++) {
        pixels[i] = new color [screen_y];
        for (int j = 0; j < screen_y; j++) {
            pixels[i][j] = backgroud;
        }
        zs[i] = new double [screen_y];
        for (int j = 0; j < screen_y; j++) {
            zs[i][j] = +20; // a very large value intended as +INFINITY
        }
    }

    // perform scan conversion, populate the 2D array pixels
    // the array zs is the z-buffer.

    homogeneous_point s3p;

    int cnt=1;
    homogeneous_point Points[3];
    int ic=0;
    while(stage3 >> s3p.x >> s3p.y >> s3p.z){
        s3p.w=1;
        Points[cnt-1]=s3p;

        if(cnt%3==0){

            int x,y;
            std::sort(std::begin(Points), std::end(Points), comp);
            for(int i=0;i<3;i++){

                    Points[i]=mapped(Points[i]);
                    //Points[i].print();
                    x=toint(Points[i].x);
                    y=screen_y-(toint(Points[i].y));
                    if(zs[x][y] >Points[i].z){
                        zs[x][y]=Points[i].z;
                        pixels[x][y]=finalclrs[ic];

                    }


            }


            int maxY=toint(Points[0].y);
            int minY=toint(Points[2].y);

            for(int ys=maxY-1; ys>minY; ys--){
                int xs,xe;
                homogeneous_point xa= pointKnownY(Points[0],Points[2],ys);
                xs=toint(xa.x);
                y=screen_y-(toint(xa.y));

                if(zs[xs][y] >xa.z){
                    zs[xs][y]=xa.z;
                    pixels[xs][y]=finalclrs[ic];
                }

                homogeneous_point xb;
                if(ys<toint(Points[1].y)){
                    xb= pointKnownY(Points[1],Points[2],ys);
                }
                else{
                    xb= pointKnownY(Points[0],Points[1],ys);
                    //cout<<"ok"<<endl;
                }

                xe=toint(xb.x);
                y=screen_y-(toint(xb.y));

                if(zs[xe][y] >xb.z){
                    zs[xe][y]=xb.z;
                    pixels[xe][y]=finalclrs[ic];
                }
                //cout<<xs+1<<" xe="<<xe<<endl;
                int xpp,xee;
                if(xs<xe) {
                        xpp=xs+1;
                        xee=xe;
                }
                else{
                  xpp=xe+1;
                  xee=xs;
                }
                for(int xp=xpp; xp<xee; xp++){

                    double zp=xa.z+ ((xb.z-xa.z)/(xb.x-xa.x))*(xp-xa.x);
                    if(zs[xp][y]>zp){
                           // cout<<"okay"<<endl;
                    zs[xp][y]=zp;
                    pixels[xp][y]=finalclrs[ic];
                    }
                }

            }


            cnt=0;
            ic++;
        }

        cnt++;
    }



    // the following code generates a bmp image. do not change this.
    bitmap_image image(screen_x, screen_y);
    for (int x = 0; x < screen_x; x++) {
        for (int y = 0; y < screen_y; y++) {
            image.set_pixel(x, y, pixels[x][y].r, pixels[x][y].g, pixels[x][y].b);
        }
    }
    image.save_image("out.bmp");

    // free the dynamically allocated memory
    delete[] pixels;
    delete[] zs;

}




homogeneous_point intersect(homogeneous_point p3,homogeneous_point p2, double z){
    homogeneous_point ans;
    ans.w=1;
    ans.z=z;
    ans.x=p2.x + ((p3.x-p2.x)/(p3.z-p2.z))*(z-p2.z);
    ans.y=p2.y + ((p3.y-p2.y)/(p3.z-p2.z))*(z-p2.z);
    //ans.print();
    return ans;
}

void stage3()
{
    //cout<<"ok"<<endl;
    if (near == far) return;
    ifstream stage2;
    ofstream stage3;
    stage2.open ("stage2.txt");
    stage3.open ("stage3.txt");
    stage3 << std::fixed;
    stage3 << std::setprecision(7);

    fov_y=fov_y*(pi/180);
    fov_x=fov_y * aspectRatio;
    double t= near * tan(fov_y/2);
    double r=near * tan(fov_x/2);



    matrix projectMat(4);

    projectMat.values[0][0]=near/r;
    projectMat.values[0][1]=0;
    projectMat.values[0][2]=0;
    projectMat.values[0][3]=0;

    projectMat.values[1][0]=0;
    projectMat.values[1][1]=near/t;
    projectMat.values[1][2]=0;
    projectMat.values[1][3]=0;

    projectMat.values[2][0]=0;
    projectMat.values[2][1]=0;
    projectMat.values[2][2]=-((far + near)/(far - near));
    projectMat.values[2][3]=-((2 * far * near)/(far - near));

    projectMat.values[3][0]=0;
    projectMat.values[3][1]=0;
    projectMat.values[3][2]=-1;
    projectMat.values[3][3]=0;


    //projectMat.print();
    //cout<<clrs.size()<<endl;
    //cout<<finalclrs.size()<<endl;
    double NEAR=-near;
    double FAR=-far;
    homogeneous_point s2p;
    int cnt=1;
    homogeneous_point Points[3];
    int ic=0;
    while(stage2>> s2p.x >> s2p.y >> s2p.z){
        s2p.w=1;
        //s2p.print();
        Points[cnt-1]=s2p;
        //homogeneous_point projP=projectMat*s2p;
        //stage3 << projP.x <<" "<<projP.y <<" "<<projP.z<<endl;
        if((cnt%3)==0){



            //case 1
            if((Points[0].z>=FAR && Points[0].z<= NEAR) && (Points[1].z>=FAR && Points[1].z<= NEAR)
               && (Points[2].z>=FAR && Points[2].z<= NEAR)){
                //cout<<"T"<<endl;
                for(int i=0;i<3;i++){
                    homogeneous_point projP=projectMat*Points[i];
                    stage3 << projP.x <<" "<<projP.y <<" "<<projP.z<<endl;
                }
                stage3<<endl;
                cnt=0;

                //clrs[ic].print();
                finalclrs.push_back(clrs[ic]);
                //finalclrs[ic].print();



                ic++;
            }

            //case 2
            else if((Points[0].z>NEAR || Points[0].z<FAR) && (Points[1].z>=FAR && Points[1].z<=NEAR)
               && (Points[2].z>=FAR && Points[2].z<=NEAR)){
                //cout<< "nice"<<endl;
                double z;
                if( (Points[0].z>NEAR) && (Points[1].z==NEAR ) &&(Points[2].z==NEAR)){
                  cnt=0;
                  continue;
                }
                else if((Points[0].z>NEAR) && (Points[1].z<NEAR ) &&(Points[2].z<NEAR)) {
                    z=NEAR;
                }
                else if((Points[0].z<FAR) && (Points[1].z==FAR ) &&(Points[2].z==FAR)){
                  //z=NEAR;
                  cnt=0;
                  continue;
                }
                else if((Points[0].z<FAR) && (Points[1].z>FAR ) &&(Points[2].z>FAR)) {
                    z=FAR;
                }


                homogeneous_point p=intersect(Points[0],Points[1],z);
                p=projectMat*p;
                //p.print();


                homogeneous_point q=intersect(Points[0],Points[2],z);
                q=projectMat*q;
                //q.print();
                Points[1]=projectMat*Points[1];
                Points[2]=projectMat*Points[2];

                stage3 << Points[1].x<<" "<<Points[1].y<<" "<<Points[1].z<<endl;
                stage3 << Points[2].x<<" "<<Points[2].y<<" "<<Points[2].z<<endl;
                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;

                stage3 <<endl;

                finalclrs.push_back(clrs[ic]);

                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                stage3 << Points[2].x<<" "<<Points[2].y<<" "<<Points[2].z<<endl;

                stage3 << endl;

                finalclrs.push_back(clrs[ic]);
                cnt=0;



                ic++;

            }

            else if((Points[1].z>NEAR || Points[1].z<FAR) && (Points[0].z>=FAR && Points[0].z<=NEAR)
               && (Points[2].z>=FAR && Points[2].z<=NEAR)){
                //cout<< "nice"<<endl;
                double z;
                if( (Points[1].z>NEAR) && (Points[0].z==NEAR ) &&(Points[2].z==NEAR)){
                  cnt=0;
                  continue;
                }
                else if((Points[1].z>NEAR) && (Points[0].z<NEAR ) &&(Points[2].z<NEAR)) {
                    z=NEAR;
                }
                else if((Points[1].z<FAR) && (Points[0].z==FAR ) &&(Points[2].z==FAR)){
                  //z=NEAR;
                  cnt=0;
                  continue;
                }
                else if((Points[1].z<FAR) && (Points[0].z>FAR ) &&(Points[2].z>FAR)) {
                    z=FAR;
                }

                homogeneous_point p=intersect(Points[1],Points[0],z);
                p=projectMat*p;
                //p.print();


                homogeneous_point q=intersect(Points[1],Points[2],z);
                q=projectMat*q;
                //q.print();
                Points[0]=projectMat*Points[0];
                Points[2]=projectMat*Points[2];

                stage3 << Points[0].x<<" "<<Points[0].y<<" "<<Points[0].z<<endl;
                stage3 << Points[2].x<<" "<<Points[2].y<<" "<<Points[2].z<<endl;
                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;

                stage3 <<endl;

                finalclrs.push_back(clrs[ic]);

                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                stage3 << Points[2].x<<" "<<Points[2].y<<" "<<Points[2].z<<endl;

                stage3 << endl;

                finalclrs.push_back(clrs[ic]);
                cnt=0;
                ic++;

            }
            else if((Points[2].z>NEAR || Points[2].z<FAR) && (Points[0].z>=FAR && Points[0].z<=NEAR)
               && (Points[1].z>=FAR && Points[1].z<=NEAR)){
                //cout<< "nice"<<endl;
                double z;
                if( (Points[2].z>NEAR) && (Points[0].z==NEAR ) &&(Points[1].z==NEAR)){
                  cnt=0;
                  continue;
                }
                else if((Points[2].z>NEAR) && (Points[0].z<NEAR ) &&(Points[1].z<NEAR)) {
                    z=NEAR;
                }
                else if((Points[2].z<FAR) && (Points[0].z==FAR ) &&(Points[1].z==FAR)){
                  //z=NEAR;
                  cnt=0;
                  continue;
                }
                else if((Points[2].z<FAR) && (Points[0].z>FAR ) &&(Points[1].z>FAR)) {
                    z=FAR;
                }


                homogeneous_point p=intersect(Points[2],Points[0],z);
                p=projectMat*p;
                //p.print();


                homogeneous_point q=intersect(Points[2],Points[1],z);
                q=projectMat*q;
                //q.print();
                Points[0]=projectMat*Points[0];
                Points[1]=projectMat*Points[1];

                stage3 << Points[0].x<<" "<<Points[0].y<<" "<<Points[0].z<<endl;
                stage3 << Points[1].x<<" "<<Points[1].y<<" "<<Points[1].z<<endl;
                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;

                stage3 <<endl;

                finalclrs.push_back(clrs[ic]);

                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                stage3 << Points[1].x<<" "<<Points[1].y<<" "<<Points[1].z<<endl;

                stage3 << endl;

                finalclrs.push_back(clrs[ic]);
                cnt=0;

                ic++;
            }


            //case 3
            else if((Points[0].z<=FAR && Points[1].z<=FAR && Points[2].z<=FAR)
                    ||(Points[0].z >=NEAR && Points[1].z>=NEAR && Points[2].z>=NEAR)){
                    cnt=0;
                    continue;
            }

            //case 4
            else if((Points[0].z>FAR && Points[0].z<NEAR)&&((Points[1].z>NEAR && Points[2].z>NEAR)
                    || (Points[1].z<FAR && Points[2].z<FAR) )){
                double z;
                if(Points[1].z>NEAR && Points[2].z>NEAR){
                    z=NEAR;
                }
                else if(Points[1].z<FAR && Points[2].z<FAR){
                  z=FAR;
                }

                homogeneous_point p=intersect(Points[1],Points[0],z);
                homogeneous_point q=intersect(Points[2],Points[0],z);

                p=projectMat*p;
                q=projectMat*q;
                Points[0]=projectMat*Points[0];

                stage3 << Points[0].x<<" "<<Points[0].y<<" "<<Points[0].z<<endl;
                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;


                stage3 << endl;

                finalclrs.push_back(clrs[ic]);
                cnt=0;
                ic++;
            }

            else if((Points[1].z>FAR && Points[1].z<NEAR)&&((Points[0].z>NEAR && Points[2].z>NEAR)
                    || (Points[0].z<FAR && Points[2].z<FAR) )){
                double z;
                if(Points[0].z>NEAR && Points[2].z>NEAR){
                    z=NEAR;
                }
                else if(Points[0].z<FAR && Points[2].z<FAR){
                  z=FAR;
                }

                homogeneous_point p=intersect(Points[0],Points[1],z);
                homogeneous_point q=intersect(Points[2],Points[1],z);

                p=projectMat*p;
                q=projectMat*q;
                Points[1]=projectMat*Points[1];

                stage3 << Points[1].x<<" "<<Points[1].y<<" "<<Points[1].z<<endl;
                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;


                stage3 << endl;
                finalclrs.push_back(clrs[ic]);
                cnt=0;
                ic++;
            }

            else if((Points[2].z>FAR && Points[2].z<NEAR)&&((Points[0].z>NEAR && Points[1].z>NEAR)
                    || (Points[0].z<FAR && Points[1].z<FAR) )){
                double z;
                if(Points[0].z>NEAR && Points[1].z>NEAR){
                    z=NEAR;
                }
                else if(Points[0].z<FAR && Points[1].z<FAR){
                  z=FAR;
                }

                homogeneous_point p=intersect(Points[0],Points[2],z);
                homogeneous_point q=intersect(Points[1],Points[2],z);

                p=projectMat*p;
                q=projectMat*q;
                Points[2]=projectMat*Points[2];

                stage3 << Points[2].x<<" "<<Points[2].y<<" "<<Points[2].z<<endl;
                stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;


                stage3 << endl;
                finalclrs.push_back(clrs[ic]);
                cnt=0;
                ic++;
            }

            //case 5 due

            else if((Points[0].z>FAR && Points[0].z<NEAR) && ((Points[1].z<FAR && Points[2].z>NEAR)
                || (Points[1].z>NEAR && Points[2].z <FAR) )){
                 //cout<< "Find"<<endl;

                 double z2,z3;
                 homogeneous_point x1=Points[0];
                 x1.w=1;
                 homogeneous_point x2,x3;
                 x2.w=1;
                 x3.w=1;
                 z2=FAR;
                 z3=NEAR;
                 if(Points[1].z<FAR){
                    x2=Points[1];
                    x3=Points[2];
                   // cout<<"1 is in FAR"<<endl;
                 }
                 else{
                    x2=Points[2];
                    x3=Points[1];
                    //cout<<"2 is in FAR"<<endl;

                 }

                 homogeneous_point p=intersect(x2,x1,z2);
                 homogeneous_point q=intersect(x2,x3,z2);

                 homogeneous_point r=intersect(x3,x1,z3);
                 homogeneous_point s=intersect(x2,x3,z3);

                 x1=projectMat*x1;
                 p=projectMat*p;
                 q=projectMat*q;
                 r=projectMat*r;
                 s=projectMat*s;

                 stage3 << x1.x <<" "<<x1.y <<" "<<x1.z<<endl;
                 stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;

                 stage3<<endl;
                 finalclrs.push_back(clrs[ic]);

                 stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                 stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;

                 stage3 <<endl;
                 finalclrs.push_back(clrs[ic]);

                 stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;
                 stage3 << s.x <<" "<<s.y <<" "<<s.z<<endl;

                 stage3 <<endl;
                 finalclrs.push_back(clrs[ic]);
                 cnt=0;
                 ic++;

            }

            else if((Points[1].z>FAR && Points[1].z<NEAR) && ((Points[0].z<FAR && Points[2].z>NEAR)
                || (Points[0].z>NEAR && Points[2].z <FAR) )){
                 //cout<< "Find"<<endl;

                 double z2,z3;
                 homogeneous_point x1=Points[1];
                 x1.w=1;
                 homogeneous_point x2,x3;
                 x2.w=1;
                 x3.w=1;
                 z2=FAR;
                 z3=NEAR;
                 if(Points[0].z<FAR){
                    x2=Points[0];
                    x3=Points[2];
                   // cout<<"0 is in FAR"<<endl;
                 }
                 else{
                    x2=Points[2];
                    x3=Points[0];
                   // cout<<"2bc is in FAR"<<endl;

                 }

                 homogeneous_point p=intersect(x2,x1,z2);
                 homogeneous_point q=intersect(x2,x3,z2);

                 homogeneous_point r=intersect(x3,x1,z3);
                 homogeneous_point s=intersect(x2,x3,z3);

                 x1=projectMat*x1;
                 p=projectMat*p;
                 q=projectMat*q;
                 r=projectMat*r;
                 s=projectMat*s;

                 stage3 << x1.x <<" "<<x1.y <<" "<<x1.z<<endl;
                 stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;

                 stage3<<endl;
                 finalclrs.push_back(clrs[ic]);

                 stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                 stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;

                 stage3 <<endl;
                 finalclrs.push_back(clrs[ic]);

                 stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;
                 stage3 << s.x <<" "<<s.y <<" "<<s.z<<endl;

                 stage3 <<endl;
                 finalclrs.push_back(clrs[ic]);
                 cnt=0;
                 ic++;

            }
            else if((Points[2].z>FAR && Points[2].z<NEAR) && ((Points[1].z<FAR && Points[0].z>NEAR)
                || (Points[1].z>NEAR && Points[0].z <FAR) )){
                 //cout<< "Find"<<endl;

                 double z2,z3;
                 homogeneous_point x1=Points[2];
                 x1.w=1;
                 homogeneous_point x2,x3;
                 x2.w=1;
                 x3.w=1;
                 z2=FAR;
                 z3=NEAR;
                 if(Points[0].z<FAR){
                    x2=Points[0];
                    x3=Points[1];
                   // cout<<"1 is in FAR"<<endl;
                 }
                 else{
                    x2=Points[1];
                    x3=Points[0];
                   // cout<<"2 is in FAR"<<endl;

                 }

                 homogeneous_point p=intersect(x2,x1,z2);
                 homogeneous_point q=intersect(x2,x3,z2);

                 homogeneous_point r=intersect(x3,x1,z3);
                 homogeneous_point s=intersect(x2,x3,z3);

                 x1=projectMat*x1;
                 p=projectMat*p;
                 q=projectMat*q;
                 r=projectMat*r;
                 s=projectMat*s;

                 stage3 << x1.x <<" "<<x1.y <<" "<<x1.z<<endl;
                 stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;

                 stage3<<endl;
                 finalclrs.push_back(clrs[ic]);

                 stage3 << p.x <<" "<<p.y <<" "<<p.z<<endl;
                 stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;

                 stage3 <<endl;
                 finalclrs.push_back(clrs[ic]);

                 stage3 << q.x <<" "<<q.y <<" "<<q.z<<endl;
                 stage3 << r.x <<" "<<r.y <<" "<<r.z<<endl;
                 stage3 << s.x <<" "<<s.y <<" "<<s.z<<endl;

                 stage3 <<endl;
                 finalclrs.push_back(clrs[ic]);
                 cnt=0;
                 ic++;

            }

        }


        cnt++;


    }



    // process input from stage2 and write to stage3

    stage3.close();
    stage2.close();

}

void stage2()
{
    ifstream stage1;
    ofstream stage2;
    stage1.open ("stage1.txt");
    stage2.open ("stage2.txt");
    stage2 << std::fixed;
    stage2 << std::setprecision(7);

    Vector l(look_x-eye_x,look_y-eye_y,look_z-eye_z);
    l.normalize();
    Vector Up(up_x,up_y,up_z);
    Vector r=Vector::cross(l,Up);
    r.normalize();
    Vector u=Vector::cross(r,l);


    matrix T(4);
    T=T.make_identity(4);
    T.values[0][3]=-eye_x;
    T.values[1][3]=-eye_y;
    T.values[2][3]=-eye_z;

    matrix viewR(4);
    viewR=viewR.make_identity(4);

    viewR.values[0][0]=r.x;
    viewR.values[0][1]=r.y;
    viewR.values[0][2]=r.z;

    viewR.values[1][0]=u.x;
    viewR.values[1][1]=u.y;
    viewR.values[1][2]=u.z;

    viewR.values[2][0]=-l.x;
    viewR.values[2][1]=-l.y;
    viewR.values[2][2]=-l.z;

    matrix V=viewR*T;
    //V.print();
    homogeneous_point s1p;
    int cnt=1;
    while(stage1>> s1p.x >> s1p.y >> s1p.z){
        s1p.w=1;
        homogeneous_point viewP=V*s1p;
        stage2 << viewP.x <<" "<<viewP.y <<" "<<viewP.z<<endl;
        if((cnt%3)==0) stage2<<endl;
        cnt++;
    }


    // collect input from stage1 and process, write output to stage2

    stage1.close();
    stage2.close();

}

Vector R(Vector x, Vector a, double theta){
    Vector res= x*(cos(theta)) + a*(Vector::dot(a,x)*(1-cos(theta))) + Vector::cross(a,x)*(sin(theta));
    return res;

}

void stage1()
{
    ifstream scene;
    ofstream stage1;
    scene.open ("scene.txt");
    stage1.open ("stage1.txt");
    stage1 << std::fixed;
    stage1 << std::setprecision(7);

    string command;

    scene >> eye_x >> eye_y >> eye_z;
    scene >> look_x >> look_y >> look_z;
    scene >> up_x >> up_y >> up_z;
    scene >> fov_y >> aspectRatio >> near >> far;
    scene >> screen_x >> screen_y;
    scene >> backgroud.r >> backgroud.g >> backgroud.b;
    cout<<screen_x<<endl;
    // take other commands as input from scene in a loop
    // process accordingly
    // write to stage1
    matrix Mi(4);
    stack <matrix> s;
    s.push(Mi.make_identity(4));
    while(true){
        scene >> command;
        //cout << command<<endl;
        if(command == "triangle"){
            for(int i=0;i<3;i++){
            homogeneous_point pp;
            scene >> pp.x >>pp.y >>pp.z;
            pp.w=1.0;
            //pp.print();
            //s.top().print();
            pp=s.top()*pp;
            //pp.print();
            stage1 << pp.x <<" "<<pp.y <<" "<<pp.z<<endl;
        }
            color trclr;
            scene >> trclr.r >> trclr.g >> trclr.b;
            //stage1 << trclr.r<<" " << trclr.g<< " " << trclr.b<<endl;
            clrs.push_back(trclr);

            stage1 <<endl;
        }
        else if(command == "push"){
            s.push(s.top());

        }
        else if(command == "pop"){
            s.pop();
        }
        else if(command == "rotate"){
            double angle;
            Vector rotateAxis(0,0,0);
            scene >> angle >> rotateAxis.x >> rotateAxis.y >> rotateAxis.z;
            angle= angle*(pi/180);
           //cout<<angle<<endl;
            rotateAxis.normalize();
            //rotateAxis.print();
            Vector i(1,0,0), j(0,1,0), k(0,0,1);
            Vector c1= R(i, rotateAxis,angle);
            Vector c2= R(j, rotateAxis, angle);
            Vector c3= R(k, rotateAxis, angle);

            matrix rotationMat(4);

            rotationMat.values[0][0]=c1.x;
            rotationMat.values[0][1]=c2.x;
            rotationMat.values[0][2]=c3.x;
            rotationMat.values[0][3]=0;

            rotationMat.values[1][0]=c1.y;
            rotationMat.values[1][1]=c2.y;
            rotationMat.values[1][2]=c3.y;
            rotationMat.values[1][3]=0;

            rotationMat.values[2][0]=c1.z;
            rotationMat.values[2][1]=c2.z;
            rotationMat.values[2][2]=c3.z;
            rotationMat.values[2][3]=0;

            rotationMat.values[3][0]=0;
            rotationMat.values[3][1]=0;
            rotationMat.values[3][2]=0;
            rotationMat.values[3][3]=1;

            rotationMat= rotationMat * s.top();
            s.pop();
            s.push(rotationMat);
        }
        else if(command == "translate"){
            matrix transMat(4);
            transMat=transMat.make_identity(4);
            scene >> transMat.values[0][3] >> transMat.values[1][3] >>transMat.values[2][3];
            //transMat.print();
            transMat= transMat * s.top();
            s.pop();
            s.push(transMat);
        }
        else if(command == "scale"){
            matrix scaleMat(4);
            scaleMat=scaleMat.make_identity(4);
            scene >> scaleMat.values[0][0] >> scaleMat.values[1][1] >>scaleMat.values[2][2];
            //scaleMat.print();
            scaleMat= scaleMat * s.top();
            s.pop();
            s.push(scaleMat);

        }
        else if(command == "end"){
            break;
        }

    }
    scene.close();
    stage1.close();

}

int main()
{
    cout << std::fixed;
    cout << std::setprecision(4);

    stage1();
    stage2();
    stage3();
    scan_convert();

    return 0;
}
