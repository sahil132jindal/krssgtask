#include<bits/stdc++.h>
using namespace std;
struct outbut
{
	int floor;
	int direction;	
};
class lift
{
	int currentfloor,countout,upmax,upmin,downmax,downmin,iter,buttonmax,buttonmin,minn,maxx,state; //UP-1,DOWN-2,STAY-3
	vector<outbut> buttons,outside;
	outbut input;
	string in;
public:
	lift()
	{
		currentfloor=0;
		countout=0;
	}
	void start()
	{
		minn= min(buttonmin,min(downmin,upmin));
		maxx=max(buttonmax,max(downmax,upmax));
		if(maxx<currentfloor)
			down();
		else if (minn>currentfloor)
			up();

		
		else if( currentfloor-upmin>0&&downmax-currentfloor>0)
				{
					if(downmax-currentfloor>minn)
						down();
					else
						up();

				}
		else	if( currentfloor-upmin<0&&downmax-currentfloor>0)
				{
						if(currentfloor-upmin>maxx)
						up();
					else
						down();
				}
		else	if( currentfloor-upmin>0&&downmax-currentfloor<0)
				{
					if(currentfloor-upmin>downmax-currentfloor)
						up();
					else
						down();	
				}
		else	if( currentfloor-upmin<0&&downmax-currentfloor<0)
				{
					if(maxx>minn)
						down();
					else
						up();
				}
				
				
	}
	void up()
	{
		state=1;
	//	cout<<"\nGoing Up\n";
		currentfloor++;
		if(transition()==1)
		{
			down();
		}
		else if(transition()==2)
		{
			cout<<"\nFinished";
			return ;
		}
		else
			up();
	}
	void down()
	{
		state=2;
	//	cout<<"\nGoing Down\n";
		currentfloor--;
		if(transition()==1)
		{
			up();
		}
		else if(transition()==2)
		{
			cout<<"\nFinished";
			return ;
		}
		else
			down();
	}	
	void initialise()
	{
		countout=iter=0;
		buttonmax=upmax=downmax=0;
		buttonmin=upmin=downmin=999;
		cout<<"\nEnter currentfloor: ";
		cin>>currentfloor;
		cout<<"Enter the lift buttons pressed by you: ";
		do
			{ if(cin>>input.floor)
				
				{input.direction= 3;
				buttons.push_back(input);
				iter++;
				if(buttonmin>input.floor)
					buttonmin=input.floor;
				else if(buttonmax<input.floor)
					buttonmax=input.floor;
				}
			}while(cin&&cin.peek()!='\n');
		cin.ignore();	
		cout<<"\nEnter the lift buttons pressed from outside: ";
		do
			{	if(cin>>input.floor)
				{input.direction= 3;
				outside.push_back(input);
				iter++;
				countout++;
				}
			}while(cin&&cin.peek()!='\n');
		cin.ignore();	

		for(int i=0;i<countout;++i)
		{
			cout<<"\n Direction for "<<outside[i].floor<<": ";
			cin>>in;
			if(in=="UP")
				outside[i].direction=1;
			else
				outside[i].direction=2;
		}	
		cout<<"\nEntered data: \n";
		for(int i=0;i<countout;++i)
		{
			if(outside[i].direction==2)
				{
					if(outside[i].floor>downmax)
					downmax= outside[i].floor;

					if(outside[i].floor<downmin)
						downmin=outside[i].floor;
				}

			if(outside[i].direction==1)
				{
					if(outside[i].floor>upmax)
					upmax= outside[i].floor;

					if(outside[i].floor<upmin)
						upmin=outside[i].floor;
				}		
			cout<<outside[i].floor<<" "<<outside[i].direction<<"\t";
		}
		cout<<"\n"; 
		start();
	}

	int transition()
	{
		int f=1;
		if(iter==0)
			return 2;

		for(int i=0;i<iter-countout;++i)
		{
			if(currentfloor==buttons[i].floor)
				{
					cout<<currentfloor;
					iter--;
					f=0;		
					buttons.erase(buttons.begin()+i);
					continue;
				}
		}
		for(int i=0;i<countout;++i)
		{
			if(currentfloor==outside[i].floor&&state==outside[i].direction)
			{
				if(f)
					cout<<currentfloor;
				iter--;
				countout--;
				outside.erase(outside.begin()+i);	
				continue;			
			}	
		}
		if(state==1)
		{if(currentfloor==maxx)
			return 1;
		else 
			return 0;
		}
		if(state==2)
		{
		if(currentfloor==minn)
			return 1;
		else 
			return 0;
		}
	}
	
};
int main()
{
	lift l;
	l.initialise();

	return 0;
}