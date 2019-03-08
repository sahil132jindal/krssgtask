#include "stdio.h"
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include<fstream>
using namespace cv;
using namespace std;

#define neigh 15
#define maxit 1000
typedef struct
{
  int x;
  int y;
}coordi;

struct pix {
    vector<pix *> children;
    pix *parent;
    coordi position;
};

pix start_node;
pix end_node;
Mat img = imread("rrt.png", CV_LOAD_IMAGE_COLOR);

coordi arr[35];
int xx=0;
pix* nodes[5000];
int totnodes = 0;
int reached = 0;
int step_size =7;
int iter = 0;
int pathnodes= 0;

void init()
{
    start_node.position.x = 50;
    start_node.position.y = 50;
    start_node.parent = NULL;
    for(int i=start_node.position.x - 5; i < start_node.position.x + 5; i++)
    {
      for(int j=start_node.position.y - 5; j < start_node.position.y + 5; j++)
      {
        img.at<Vec3b>(i, j)[0] = 255;
        img.at<Vec3b>(i, j)[1] = 0;
        img.at<Vec3b>(i, j)[2] = 0;
      }
    }
    nodes[totnodes++] = &start_node;
    end_node.position.x = 350;
    end_node.position.y = 350;
    for(int i=end_node.position.x - 5; i < end_node.position.x + 5; i++)
    {
      for(int j=end_node.position.y - 5; j < end_node.position.y + 5; j++)
      {
        img.at<Vec3b>(i, j)[0] = 0;
        img.at<Vec3b>(i, j)[1] = 0;
        img.at<Vec3b>(i, j)[2] = 255;
      }
    }
    srand(time(NULL));
}
float node_dist(coordi p, coordi q)
{
  coordi v;
  v.x = p.x - q.x;
  v.y = p.y - q.y;
  return sqrt(pow(v.x, 2) + pow(v.y, 2));
}
int near_node(pix rnode)
{
  float min_dist = 999.0, dist= node_dist(start_node.position, rnode.position);
  int lnode = 0, i = 0;
  for(i=0; i<totnodes; i++)
  {
    dist = node_dist(nodes[i]->position, rnode.position);
    if(dist<min_dist)
    {
      min_dist = dist;
      lnode = i;
    }
  }
  return lnode;
}
coordi stepping(coordi nnode, coordi rnode)
{
  coordi interm, step;
  float magn = 0.0, x = 0.0, y = 0.0;
  interm.x = rnode.x - nnode.x;
  interm.y = rnode.y - nnode.y;
  magn = sqrt((interm.x)*(interm.x) + (interm.y)*(interm.y));
  x = (float)(interm.x / magn);
  y = (float)(interm.y / magn);
  step.x = (int)(nnode.x + step_size*x);
  step.y = (int)(nnode.y + step_size*y);
  return step;
}
int check_validity_1(coordi p, coordi q)
{
  coordi large, small;
  int i = 0, j1 = 0, j2 = 0;
  double slope;
  if(q.x<p.x)
  {
    small = q;
    large = p;
  }
  else
  {
    small = p;
    large = q;
  }
  if(large.x == small.x)
    return 0;
  slope = ((double)large.y - small.y)/((double)large.x - small.x);
  for(i=small.x+1; i<large.x; i++)
  {
    j1 = (int)((i*slope) - (small.x)*(slope) + small.y);
    j2 = j1 + 1;
    if((i<0) || (i>400) || (j1<0) || (j1>400) || (j2<0) || (j2>400))
      continue;
    if(((int)img.at<Vec3b>(i, j1)[0] <250) && ((int)img.at<Vec3b>(i, j1)[1] < 250) && ((int)img.at<Vec3b>(i, j1)[2] < 250))
     return 0;
     if(((int)img.at<Vec3b>(i, j2)[0] <250) && ((int)img.at<Vec3b>(i, j2)[1] < 250) && ((int)img.at<Vec3b>(i, j2)[2] < 250))
      return 0;
  }
  return 1;
}
int check_validity_2(coordi p, coordi q)
{
  coordi large, small;
  int i = 0, j1 = 0, j2 = 0;
  double slope;
  if(q.y<p.y)
  {
    small = q;
    large = p;
  }
  else
  {
    small = p;
    large = q;
  }
  if(large.x == small.x)
    return 0;
  slope = ((double)large.y - small.y)/((double)large.x - small.x);
  for(i=small.y+1; i<large.y; i++)
  {
    j1 = (int)(((i - small.y)/slope) + small.x);
    j2 = j1 + 1;
    if((i<0) || (i>400) || (j1<0) || (j1>400) || (j2<0) || (j2>400))
      continue;
    if(((int)img.at<Vec3b>(j1, i)[0] <250) && ((int)img.at<Vec3b>(j1, i)[1] < 250) && ((int)img.at<Vec3b>(j1, i)[2] < 250))
     return 0;
     if(((int)img.at<Vec3b>(j2, i)[0] <250) && ((int)img.at<Vec3b>(j2, i)[1] < 250) && ((int)img.at<Vec3b>(j2, i)[2] < 250))
      return 0;
  }
  return 1;
}
void draw_path()
{
  pix up, down;
  int breaking = 0;
  down = end_node;
  up = *(end_node.parent);
  while(1)
  {
    if(pathnodes%3==0&&xx!=35)
    {
    arr[xx].x=down.position.y;
    arr[xx].y=400-down.position.x;
    ++xx; 
    }
    line(img, Point(up.position.y, up.position.x), Point(down.position.y, down.position.x), Scalar(0, 255, 0), 2, 8);
    pathnodes++;
    if(up.parent == NULL)
      break;
    up = *(up.parent);
    down = *(down.parent);
  }
}

void rrt()
{

  int flag1 = 0, index = 0, flag2 = 0,f1=0,f2=0;
  float cost=0;
    pix* rnode = new pix;
    pix* stepnode = new pix;
    pix* neighbour = new pix;
    (rnode->position).x = rand() % 400 + 1;
    (rnode->position).y = rand() % 400 + 1;
    index = near_node(*rnode);
    if((node_dist(rnode->position, nodes[index]->position)) < step_size)
      return;
    else
      stepnode->position = stepping(nodes[index]->position, rnode->position);
    flag1 = check_validity_1(nodes[index]->position, stepnode->position);
    flag2 = check_validity_2(nodes[index]->position, stepnode->position);
    if((flag1 == 1) && (flag2 == 1))
    {
      stepnode->parent = nodes[index];

      neighbour= stepnode;
       while(neighbour->parent!= NULL)
      { 
        
        cost+=node_dist(neighbour->position,neighbour->parent->position);
        neighbour=neighbour->parent;
      } 
  for(int i=0; i<totnodes; i++)
  {
    float dist = node_dist(nodes[i]->position, stepnode->position);
    f1 = check_validity_1(nodes[i]->position, stepnode->position);
    f2 = check_validity_2(nodes[i]->position, stepnode->position);
    if(f1==0||f2==0||i==index)
      continue;
    if(dist<neigh)
    {
      neighbour= nodes[i];
      float cc=dist;
      while(neighbour->parent!= NULL)
      { 
        cc+=node_dist(neighbour->position,neighbour->parent->position);        
        neighbour=neighbour->parent; 
      } 
      if(cc<cost)
      {
        stepnode->parent=nodes[i];
        cost=cc;
      }
    }
  }
    for(int i=0; i<totnodes; i++)
  {
    float dist = node_dist(nodes[i]->position, stepnode->position);
    f1 = check_validity_1(nodes[i]->position, stepnode->position);
    f2 = check_validity_2(nodes[i]->position, stepnode->position);
    if(f1==0||f2==0||i==index)
      continue;
    if(dist<neigh)
    {
      neighbour= nodes[i];
      float cc=dist;
      while(neighbour->parent!= NULL)
      { 
        cc+=node_dist(neighbour->position,neighbour->parent->position);        
        neighbour=neighbour->parent; 
      } 
      if(cc>cost)
      {
        nodes[i]->parent=stepnode;
      }
    }
  }
        nodes[totnodes++] = stepnode;

      for(int i=stepnode->position.x - 2; i < stepnode->position.x + 2; i++)
      {
        for(int j=stepnode->position.y - 2; j < stepnode->position.y + 2; j++)
        {
          if((i<0) || (i>400) || (j<0) || (j>400))
            continue;

          img.at<Vec3b>(i, j)[0] = 0;
          img.at<Vec3b>(i, j)[1] = 255;
          img.at<Vec3b>(i, j)[2] = 0;
        }
      }

      if((check_validity_1(stepnode->position, end_node.position)) && (check_validity_2(stepnode->position, end_node.position)) && (node_dist(stepnode->position,end_node.position) < step_size)&&!reached)
      {
        reached = 1;
        nodes[totnodes++] = &end_node;
        end_node.parent = stepnode;
        (nodes[totnodes-1]->children).push_back(&end_node);

      }
    }
    iter++;
}

int main()
{
      ofstream fout; 
      fout.open("nodes.txt");
      namedWindow( "window", WINDOW_NORMAL);

  init();
  while((reached == 0)||iter<maxit)
    rrt();
  for(int i=1;i<totnodes;++i)
                {line(img, Point((nodes[i]->position).y, (nodes[i]->position).x), Point(nodes[i]->parent->position.y, nodes[i]->parent->position.x), Scalar(0, 255, 255), 2, 8);

                        imshow("window", img);
                        waitKey(1);
                }
    draw_path(); 
    cout<<"Tot nodes:"<<totnodes<<"\t Iterations: "<<iter<<"\t Nodes in path: "<<pathnodes;
    cout<<endl;

    arr[xx].x= 50;
    arr[xx].y= 350; 

    for(int i=xx;i>=0;--i)
    {
      fout<<arr[i].x<<"\n"<<arr[i].y<<"\n";
    }
    fout.close();

      namedWindow("final",WINDOW_NORMAL);
  imshow("final", img);
  waitKey();
  return 0;
}