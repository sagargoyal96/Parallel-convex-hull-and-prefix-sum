#include <utility>

vector<int> calcPrefixSum ( vector<int> input, int num_threads )
{
	int lastelems[num_threads];
	int p;
	omp_set_num_threads(num_threads);
	int n=input.size();
	vector<int> prefix;

	prefix.resize(n);


	#pragma omp parallel
	{
		int i=0;
		int id=omp_get_thread_num();
		p=omp_get_num_threads();

		for(i=id*(n/p);i<id*(n/p)+(n/p);i++)
		{
			// printf(" %d %d ", id, i);

			if(i==id*n/p)
				prefix[i]=input[i];
			else
			{	
				prefix[i]=prefix[i-1]+input[i];
			}
		}
		lastelems[id]=prefix[i-1];
	}


	// breaking into p num_threads O(n/p) linear prefix

	if(num_threads>1)
	{

		int index=1;
		while(1)
		{
			#pragma omp parallel
			{
				int id=omp_get_thread_num()+1;

				if(id%(2*index)==0)
				{
					lastelems[id-1]=lastelems[id-index-1]+ lastelems[id-1];
				}
				
			}
			index*=2;
			if(index>p)
				break;
			
		}



		// first upward movemnet in binary tree

		// int last=prefix[num_threads-1];
		// prefix[num_threads-1]=0;

		index=num_threads/2;

		

		while(1)
		{
			#pragma omp parallel
			{
				int id=omp_get_thread_num()+1;

				if(id%(2*index)==0)
				{
					int temp=lastelems[id-index-1];
					lastelems[id-index-1]=lastelems[id-1];
					lastelems[id-1] = temp + lastelems[id-1];
				}
				
			}
			index/=2;
			if(index<1)
				break;		
		}
	


	// second coverage of binary tree moving downward |^


	// final addition of the found values in the array

		#pragma omp parallel
		{
			int i=0;
			int id=omp_get_thread_num();
			p=omp_get_num_threads();

			for(i=id*(n/p);i<id*(n/p)+(n/p);i++)
			{
				// printf(" %d %d ", id, i);

				prefix[i]+=lastelems[id];
			}

		}
	}

	return prefix;

}
