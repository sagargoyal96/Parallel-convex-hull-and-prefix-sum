#include <utility>
using namespace std;
#define iPair pair<int, int>

typedef struct region
{
	iPair pt1;
	iPair pt2;
	vector<iPair> pts;
}reg;


// point konsi side pe hai bata dega

int findSide(iPair p1, iPair p2, iPair p)
{
    int val = (p.second - p1.second) * (p2.first - p1.first) -
              (p2.second - p1.second) * (p.first - p1.first);
 
    if (val > 0)
        return 1;
    if (val < 0)
        return -1;
    return 0;
}

// line se distance bata dega

int lineDist(iPair p1, iPair p2, iPair p)
{
    return abs ((p.second - p1.second) * (p2.first - p1.first) -
               (p2.second - p1.second) * (p.first - p1.first));
}


vector<pair<int , int>> calcConvexHull ( vector<vector<int>> image, int num_threads)
{
	vector<iPair> Result;

	vector<pair<int , int> > points;
    for(int i=0;i<image.size();i++)
    {
        for(int j=0;j<image[i].size();j++)
        {
            // printf("%d ", image[i][j]);
            if(image[i][j]==1)
            {
            	pair<int, int> pt;
            	pt.first=i;
            	pt.second=j;
            	points.push_back(pt);
            }
        }
    }

    // points has all the points in the databse

    // start the algo

    pair<int , int> maxi=points.at(0);
    pair<int , int> mini=points.at(0);

    vector<reg> regions;

    reg top,bottom;

    // finding max and min point
    for(int i=0;i<points.size();i++)
    {
        if(points.at(i).first <mini.first)
        	mini=points.at(i);
        if(points.at(i).first>maxi.first)
        	maxi=points.at(i);
        
    }
    top.pt1=mini;
    top.pt2=maxi;
    bottom.pt1=mini;
    bottom.pt2=maxi;

    Result.push_back(mini);
    Result.push_back(maxi);

    // take care when points lie on the line joining straight line conditions

    // make regions




    for(int i=0;i<points.size();i++)
    {
        if(findSide(mini, maxi,points.at(i))==-1)
        	top.pts.push_back(points.at(i));
        else if(findSide(mini, maxi,points.at(i))==1)
        	bottom.pts.push_back(points.at(i));
        
    }

    // equality condition

    // if(top.pts.size()==0 ||bottom.pts.size()==0)
    // {
    // 	for(int i=0;i<points.size();i++)
    // 	{
	   //      if(findSide(mini, maxi,points.at(i))==0 && points.at(i)!=mini && points.at(i)!=maxi)
	   //      	Result.push_back(points.at(i));
        
    // 	}
    // }

    regions.push_back(top);
    regions.push_back(bottom);

    // solve regions

    int threads=num_threads;
	omp_set_num_threads(threads);


    while(!regions.empty())
    {
    	#pragma omp parallel
    	{
    		reg one;
    		int flag=0;
    		#pragma omp critical
    		{
    			if(!regions.empty())
    			{
    				one=regions.back();
	    			regions.pop_back();
    			}
    			else flag=1;
    			
    		}

    		// cout<<omp_get_thread_num()<<" "<<endl;
    		
	    	// reg pop
	    	if(one.pts.size()!=0 && flag==0)
	    	{
	    		iPair maxy=one.pts.at(0);

	    	// find the maximum y
		    	for(int j=0;j<one.pts.size();j++)
		    	{
		    		if(lineDist(one.pt1, one.pt2, one.pts.at(j))>lineDist(one.pt1, one.pt2 ,maxy))
		    			maxy=one.pts.at(j);
		    	}

		    	#pragma omp critical
		    	{
		    		Result.push_back(maxy);
		    	}
		    	

				
		    	reg r1,r2;
				r1.pt1=one.pt1;
				r1.pt2=maxy;
				r2.pt1=maxy;
				r2.pt2=one.pt2;

				for(int j=0;j<one.pts.size();j++)
		    	{
		      		if((findSide(one.pt1, maxy, one.pts.at(j))==1 && findSide(one.pt1, maxy,one.pt2)==-1) || (findSide(one.pt1, maxy, one.pts.at(j))==-1 && findSide(one.pt1, maxy,one.pt2)==1))
		    		{
		    			r1.pts.push_back(one.pts.at(j));
		    		}
		    		
		    		else if((findSide(one.pt2, maxy, one.pts.at(j))==1 && findSide(one.pt2, maxy,one.pt1)==-1) || (findSide(one.pt2, maxy, one.pts.at(j))==-1 && findSide(one.pt2, maxy,one.pt1)==1)) 
		    		{
		    			r2.pts.push_back(one.pts.at(j));
		    		}
		    		
		    	}

		    	#pragma omp critical
		    	{
		    		regions.push_back(r1);
		    		regions.push_back(r2);
		    	}
		    	
		    	// equality condition
		    	// if(r1.pts.size()==0)
		    	// {
		    	// 	for(int j=0;j<one.pts.size();j++)
		    	// 	{
		    	// 		if(findSide(one.pt1, maxy, one.pts.at(j))==0 && one.pts.at(j)!=maxy && one.pts.at(j)!=one.pt1)
		    	// 		{
		    	// 			#pragma omp critical
		    	// 			{
		    	// 				Result.push_back(one.pts.at(j));
		    	// 			}	
		    	// 		}
		    	// 	}
		    	// }

		    	// if(r2.pts.size()==0)
		    	// {
		    	// 	for(int j=0;j<one.pts.size();j++)
		    	// 	{
		    	// 		if(findSide(one.pt2, maxy, one.pts.at(j))==0 && one.pts.at(j)!=maxy && one.pts.at(j)!=one.pt2)
		    	// 		{
		    	// 			#pragma omp critical
		    	// 			{
		    	// 				Result.push_back(one.pts.at(j));
		    	// 			}	
		    	// 		}
		    	// 	}
		    	// }


	    	}
	    
	    }

	  	    	
    }
    return Result;

}


