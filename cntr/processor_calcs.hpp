/***********

Copyright 2021 Agriculex Inc.

This file is part of the Agriculex ESC-3 program.

The Agriculex ESC-3 program is free software: you can redistribute it 
and/or modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation, GPL-3.0-or-later.

The Agriculex ESC-3 program is distributed in the hope that it will be 
useful, but, except as noted below, WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Agriculex ESC-3 program.  If not, see 
<https://www.gnu.org/licenses/>.

Agriculex Inc. warrants that executable files of the ESC-3 program 
compiled and installed by Agriculex Inc. will function as advertized in 
ESC-3 machines produced by Agriculex Inc.

**********/

//functions required by processor

const float yfactor = 2.5;//multiply y values by this when calculating angle or distance to compensate for aspect ratio of image

float anglexy(float x, float y)
{
  if(fabs(x)<.5 && fabs(y)<.5) return(-1000);//angle invalid
  y *= yfactor;
  float angle;
  if(x==0)
  {
    if(y>=0)
    {
      angle=(M_PI/2.0);
    }
    else
    {
      angle=(3.0*M_PI/2.0);
    }
  }
  else
  {
    float atanyx = atan(float(y)/float(x));
    if(x>0) angle = atanyx;
    else angle = M_PI + atanyx;
  }
  if(angle<0) angle += 2*M_PI;
  return angle;
}

float distance(int x1, int y1, int x2, int y2)
{
  float dx = x1-x2;
  float dy = float(y1-y2) * yfactor;
  return pow((dx*dx+dy*dy), 0.5);
}

float angle_sum(float a1, float a2)
{
  float sum = a1 + a2;
  while(sum>=2*M_PI) sum -= (2*M_PI); 
  while(sum<0) sum += (2*M_PI); 
  return sum;
}

float angle_diff(float a1, float a2)
{
  float diff = a1 - a2;
  while(diff>=2*M_PI) diff -= (2*M_PI); 
  while(diff<0) diff += (2*M_PI); 
  return diff;
}

float angle_abs_diff(float a1, float a2)
{
  float diff1 = angle_diff(a1, a2);
  float diff2 = angle_diff(a2, a1);
  if(diff1 < diff2) return diff1;
  else return diff2;
}

float angle_normalize(float a)//returns an equivalent angle less than 2*PI, greater than or equal to 0
{
  while(a>=2*M_PI) a -= (2*M_PI); 
  while(a<0) a += (2*M_PI); 
  return a;
}
  

int link_score(int x0, int y0, float direction_before1, float direction_before2, float direction_after1, float direction_after2, int x_target, int y_target)
//returns a rating for a potential link from point at (x0,y0)
//directions of curve are given in order along the curve, 2 before (x0,y0), and 2 after it
//(x_target,y_target) is the link location being scored
{
  float score;
  float angle_target = anglexy(x_target-x0, y_target-y0);
  float distance_target = distance(x0, y0, x_target, y_target);
  if (distance_target == 0) return 0;
  
  //reverse direction of two directions after target
  direction_after1 += M_PI;
  direction_after2 += M_PI;  
  
  float angle_change_before = angle_abs_diff(direction_before1, direction_before2);
  float angle_change_after = angle_abs_diff(direction_after1, direction_after2);
  float score_before = 10000.0 / (angle_change_before+1) / pow(distance_target,1);//score if target is on projection of direction_before2 line
  float score_after = 10000.0 / (angle_change_after+1) / pow(distance_target,1);//score if target is on projection of direction_after1 line
  
  //rotate direction_before2, direction_after1, and angle_target lines so that direction_before2 becomes 0.
  direction_after1 = angle_diff(direction_after1, direction_before2);
  angle_target = angle_diff(angle_target, direction_before2);
  float angle_boundary = direction_after1/2.0 + M_PI;//if angle_target is greater than this, it is closer to direction_before2 line (which is now on 0)
  //If angle_target is less than this, it is closer to direction_after1 line.
  
  if(angle_target < direction_after1)//it is between the projections of the two parts of the outline
  {
    if(direction_after1 == 0)//not expected
    {
      return 0;
    }
    float m = (score_after-score_before)/direction_after1;
    score = m*angle_target + score_before;
  }
  else if(angle_target < angle_boundary)//it is outside the outline projections, closer to direction_after1
  {
    
    float a = angle_target - direction_after1;
    score = score_after - a*score_after/(M_PI/10);
  }
  else//it is closer to direction_before2 (at 0)
  {
    float a = 2*M_PI - angle_target;
    score = score_before - a*score_before/(M_PI/10);
  }
  
  float min_score = 500.0/(distance_target+1);
  
  if(score<min_score) score = min_score;
  return score;
}
    
  
  

