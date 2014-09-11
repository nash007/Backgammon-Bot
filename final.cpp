#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<algorithm>
#include<limits.h>

using namespace std;

typedef struct tree{
	int board[25],bar[2],move[2];
	double utility_val;
	struct tree *child,*sibling;
}tree;

tree* search(tree* root,int board);
void preorder(tree* p);
tree* createNode(int board);
tree* createnary(tree* root,int board[]);
void generateStates(tree *root, int die[]);
double setUtil(int board[],int bar[]);
int bearOff(tree *root,tree *temp,int move_counter,int num_moved,int die[]);
bool checkHome(tree *temp);

void preorder(tree* p)
{
	if(p==NULL)
		return;
	for(int i=1;i<25;i++)
		printf("%d ",p->board[i]);
	printf(" : %d %d ",p->move[0],p->move[1]);
	cout<<"Util : "<<(p->utility_val)<<endl;
	//preorder(p->child);
	preorder(p->sibling);
}

tree* search(tree* root,int board)
{
	if(root==NULL)
		return NULL;
	if(board==root->utility_val)
		return root;
	tree* t = search(root->child,board);
	if(t==NULL)
		t = search(root->sibling,board);
	return t;
}

tree* createNode(int board[25],int bar[2],int move[2])
{
	tree* newnode= (tree*)malloc(sizeof(tree));

	newnode->utility_val=setUtil(board,bar);										//Also add the board value on node creation.
	for(int i=1;i<25;i++)
		newnode->board[i] = board[i];

	newnode->bar[0]=bar[0];
	newnode->bar[1]=bar[1];

	newnode->move[0]=move[0];
	newnode->move[1]=move[1];

	newnode->child=NULL;
	newnode->sibling=NULL;

	return newnode;
}

tree* insertNode(tree* root,int board[25],int bar[2],int move[2])		//Root here should be at the level below which we want to insert
{
	tree* parent=root;

	if(root==NULL) 
	{
		root=(tree*)malloc(sizeof(tree));
		root = createNode(board,bar,move);
	}
	//for first child
	else if(parent->child == NULL)
		parent->child=createNode(board,bar,move);		
	// for all other childs
	else
	{
		parent = parent->child;
		while(parent->sibling != NULL)
			parent = parent->sibling;
		parent->sibling=createNode(board,bar,move);
	}
	return root;
}

void generateStates(tree *root,int die[])					// Root here should be at the level above which the states need to be generated
{
	tree *temp = (tree*)malloc(sizeof(tree));
	int move_counter = 0,num_moved=0;
	temp = createNode(root->board,root->bar,root->move);
	bool flag = 0;

	if(temp->bar[0] > 0)
	{
		if(temp->board[die[0]] >= -1 || temp->board[die[1]] >= -1 || temp->board[die[0]+die[1]] >= -1)			// Check for -1 or 0??
		{
			if(temp->board[die[0]] >=-1)
			{
				if (temp->board[die[0]] == -1) 		// a simple ++ here would make that value 0 while it should be 1.
				{
					temp->board[die[0]] = 1;
					temp->bar[0] --;
					temp->bar[1] ++;
				}
				else
				{
					temp->board[die[0]] ++;
					temp->bar[0] --;
				}
				temp->move[move_counter++] = 0*100 + die[0];
				num_moved += die[0];				//Storing the value of the die which has been used
			}

			if(temp->bar[0] > 0)
			{
				if(temp->board[die[1]] >= -1)
				{
					if (temp->board[die[1]] == -1) 		// a simple ++ here would make that value 0 while it should be 1.
					{
						temp->board[die[1]] = 1;
						temp->bar[0] --;
						temp->bar[1] ++;
					}
					else
					{
						temp->board[die[1]] ++;
						temp->bar[0] --;
					}
					temp->move[move_counter++] = 0*100 + die[1];
					num_moved += die[1];
					if(move_counter == 2)
					{
						move_counter = 0;
						num_moved = 0;
						flag = 1;
						root = insertNode(root,temp->board,temp->bar,temp->move);
						temp = createNode(root->board,root->bar,root->move);
					}
				}
				else								//Bar[0] is 1 but we can't make a valid move
				{
					temp->move[move_counter++] = 0;
					if(move_counter == 1)					// pass pass waala state
					{
						temp->move[1] = 0;
						move_counter = 0;
						flag = 1;
						num_moved = 0;
						insertNode(root,temp->board,temp->bar,temp->move);
						temp = createNode(root->board,root->bar,root->move);
					}
					else if(move_counter == 2)
					{
						move_counter = 0;
						num_moved = 0;
						flag = 1;
						root = insertNode(root,temp->board,temp->bar,temp->move);
						temp = createNode(root->board,root->bar,root->move);
					}
				}
			}
		}
		else
		{
			temp->move[0] = 0;
			temp->move[1] = 0;
			num_moved = 0;
			flag = 1;
			root = insertNode(root,temp->board,temp->bar,temp->move);
			temp = createNode(root->board,root->bar,root->move);
			return;					//No other valid states possible
		}
	}
	if(temp->bar[0] == 0)
	{
		if(move_counter == 1)
		{
			tree *temp2 = (tree*)malloc(sizeof(tree));
			temp2 = createNode(temp->board,temp->bar,temp->move);					
			int dice = die[0] + die[1] - num_moved;
			for(int i=1;i<25;i++)
			{
				if(temp->board[i] > 0)
				{
					if(temp->board[i+dice] >= -1 && i+dice < 25)
					{
						if(temp->board[i+dice] == -1)
						{
							temp->board[i+dice] ++;
							temp->bar[1] ++;
						}
						temp->board[i+dice] ++;
						temp->board[i] --;
						temp->move[move_counter++] = i*100 + dice;
						if(move_counter == 2)
						{
							move_counter = 1;
							flag = 1;
							root = insertNode(root,temp->board,temp->bar,temp->move);
							int d = temp->move[0];
							temp = createNode(temp2->board,temp2->bar,temp2->move);
						}

					}
				}
			}
		}
		else
		{
			for(int i=1;i<25;i++)
			{
				if(temp->board[i] > 0)
				{
					if(temp->board[i+die[0]] >= -1 && i+die[0] < 25)
					{
						temp->move[move_counter++] = i*100+die[0];
						temp->board[i] --;
						if(temp->board[i+die[0]] == -1)
						{
							temp->board[i+die[0]] ++;
							temp->bar[1] ++;
						}
						temp->board[i+die[0]] ++;
						num_moved += die[0];
						if(move_counter == 2)
						{
							move_counter = 0;
							num_moved = 0;
							flag = 1;
							root = insertNode(root,temp->board,temp->bar,temp->move);
							temp = createNode(root->board,root->bar,root->move);
						}
						else
						{
							for(int j=i;j<25;j++)
							{
								if(temp->board[j] > 0)
								{
									if(temp->board[j+die[1]] >= -1 && j+die[1] < 25)
									{
										temp->move[move_counter++]=j*100+die[1];
										temp->board[j] --;
										if(temp->board[j+die[1]] == -1)
										{
											temp->board[j+die[1]] ++;
											temp->bar[1] ++;
										}
										temp->board[j+die[1]] ++;
										num_moved += die[1];
										if(move_counter == 2)
										{
											move_counter = 1;
											num_moved = 0;
											flag = 1;
											root = insertNode(root,temp->board,temp->bar,temp->move);
											int d = temp->move[0];
											temp = createNode(root->board,root->bar,root->move);
											temp->move[0] = d;
											if(d/100 == 0)
												temp->bar[0] --;
											else
												temp->board[d/100]--;
											temp->board[d/100 + d%100]++;
										}
									}
								}
							}
							move_counter = 0;
							temp = createNode(root->board,root->bar,root->move);
						}
					}
					if(temp->board[i+die[1]] >= -1 && i+die[1] < 25)
					{
						temp->move[move_counter++]=i*100+die[1];
						temp->board[i] --;
						if(temp->board[i+die[1]] == -1)
						{
							temp->board[i+die[1]] ++;
							temp->bar[1] ++;
						}
						temp->board[i+die[1]] ++;
						num_moved += die[1];
						if(move_counter == 2)
						{
							move_counter = 0;
							num_moved = 0;
							flag = 1;
							root = insertNode(root,temp->board,temp->bar,temp->move);
							temp = createNode(root->board,root->bar,root->move);
						}
						else
						{
							for(int j=i;j<25;j++)
							{
								if(temp->board[j] > 0)
								{
									if(temp->board[j+die[0]] >= -1 && j+die[0] < 25)
									{
										temp->move[move_counter++]=j*100+die[0];
										temp->board[j] --;
										if(temp->board[j+die[0]] == -1)
										{
											temp->board[j+die[0]] ++;
											temp->bar[1] ++;
										}
										temp->board[j+die[0]] ++;
										num_moved += die[0];
										if(move_counter == 2)
										{
											move_counter = 1;
											num_moved = 0;
											flag = 1;
											int d = temp->move[0];
											root = insertNode(root,temp->board,temp->bar,temp->move);
											temp = createNode(root->board,root->bar,root->move);
											temp->move[0] = d;
											if(d/100 == 0)
												temp->bar[0] --;
											else
												temp->board[d/100]--;
											temp->board[d/100 + d%100]++;
										}
									}
								}
							}
							move_counter = 0;
							temp = createNode(root->board,root->bar,root->move);
						}
					}
				}
			}
			if(checkHome(temp) == 1)
			{
				move_counter = bearOff(root,temp,move_counter,num_moved,die);
			}
			if(move_counter == 0 &&  flag == 0)
			{
				temp->move[0] = 0;
				temp->move[1] = 0;
				root = insertNode(root,temp->board,temp->bar,temp->move);
				temp = createNode(root->board,root->bar,root->move);
			}
			if(move_counter == 1) 											
			{
				temp->move[move_counter++] = 0;
				move_counter = 0;
				root = insertNode(root,temp->board,temp->bar,temp->move);
				temp = createNode(root->board,root->bar,root->move);
			}
		}
	}
}

bool checkHome(tree *temp)
{
	bool homeFlag = 1;
	for(int i=1;i<19;i++)
		if(temp->board[i] > 0)
		{
			homeFlag = 0;
			break;
		}
	if(temp->bar[0] > 0)
		homeFlag = 0;
	return homeFlag;
}

int bearOff(tree *root,tree *temp,int move_counter,int num_moved,int die[])
{
	int dice;
	tree *temp2 = (tree*)malloc(sizeof(tree));
	temp2 = createNode(temp->board,temp->bar,temp->move);					

	if(move_counter == 1)
	{
		dice = die[0]+die[1]-num_moved;
		for(int i=19;i<25;i++)
		{
			if(temp->board[i] > 0)
			{
				if(temp->board[i+dice] >= -1)
				{
					if(i+dice < 25)
					{
						temp->board[i] --;
						if(temp->board[i+dice] == -1)
						{
							temp->board[i+dice] ++;
							temp->bar[1] ++;
						}
						temp->board[i+dice] ++;
						temp->move[move_counter++] = i*100+dice;
						move_counter = 0;	
						root = insertNode(root,temp->board,temp->bar,temp->move);
						temp = createNode(temp2->board,temp2->bar,temp2->move);
					}
					else
					{
						temp->board[i]--;
						temp->move[move_counter++] = i*100+dice;
						move_counter = 0;
						root = insertNode(root,temp->board,temp->bar,temp->move);
						temp = createNode(temp2->board,temp2->bar,temp2->move);
					}
				}
			}
		}
	}
	if(move_counter == 0)
	{
		for(int i=19;i<25;i++)
		{
			if(temp->board[i] > 0)
			{
				if(i+die[0] >= 25 || (temp->board[i+die[0]] >= -1 && i+die[0] < 25))
				{
					temp->move[move_counter++]=i*100+die[0];
					temp->board[i] --;
					if(i+die[0] < 25)
						if(temp->board[i+die[0]] == -1)
						{
							temp->board[i+die[0]] = 1;
							temp->bar[1] ++;
						}
						else
							temp->board[i+die[0]] ++;
					num_moved += die[0];
					if(move_counter == 2)
					{
						move_counter = 0;
						num_moved = 0;
						root = insertNode(root,temp->board,temp->bar,temp->move);
						temp = createNode(temp2->board,temp2->bar,temp2->move);
					}
					else
					{
						for(int j=i;j<25;j++)
						{
							if (temp->board[j] > 0)
							{
								if(j+die[1] >= 25 || (temp->board[j+die[1]] >= -1 && j+die[1] < 25))
								{
									temp->move[move_counter++]=j*100+die[1];
									temp->board[j] --;
									if(j+die[1] < 25)
										if(temp->board[j+die[1]] == -1)
										{
											temp->board[j+die[1]] = 1;
											temp->bar[1] ++;
										}
										else
											temp->board[j+die[1]] ++;
									num_moved += die[1];
									if(move_counter == 2)
									{
										move_counter = 1;
										num_moved = 0;
										int d = temp->move[0];
										root = insertNode(root,temp->board,temp->bar,temp->move);
										temp = createNode(temp2->board,temp2->bar,temp2->move);
										temp->move[0] = d;
										if(d/100 == 0)
											temp->bar[0] --;
										else
											temp->board[d/100]--;
										temp->board[d/100 + d%100]++;
									}
								}
							}
						}
						move_counter = 0;
						temp = createNode(temp2->board,temp2->bar,temp2->move);
					}
				}
				if(i+die[1] >=25 || (temp->board[i+die[1]] >= -1 && i+die[1] < 25))
				{
					temp->move[move_counter++]=i*100+die[1];
					temp->board[i] --;
					if(i+die[1] < 25)
						if(temp->board[i+die[1]] == -1)
						{
							temp->board[i+die[1]] = 1;
							temp->bar[1] ++;
						}
						else
							temp->board[i+die[1]] ++;
					num_moved += die[1];
					if(move_counter == 2)
					{
						move_counter = 0;
						num_moved = 0;
						root = insertNode(root,temp->board,temp->bar,temp->move);
						temp = createNode(temp2->board,temp2->bar,temp2->move);
					}
					else
					{
						for(int j=i;j<25;j++)
						{
							if (temp->board[j] > 0)
							{
								if(j+die[0] >= 25 || (temp->board[j+die[0]] >= -1 && j+die[0] < 25))
								{
									temp->move[move_counter++]=j*100+die[0];
									temp->board[j] --;
									if(j+die[0] < 25)
										if(temp->board[j+die[0]] == -1)
										{
											temp->board[j+die[0]] ++;
											temp->bar[1] ++;
										}
									temp->board[j+die[0]] ++;
									num_moved += die[0];
									if(move_counter == 2)
									{
										move_counter = 1;
										num_moved = 0;
										int d = temp->move[0];
										root = insertNode(root,temp->board,temp->bar,temp->move);
										temp = createNode(temp2->board,temp2->bar,temp2->move);
										temp->move[0] = d;
										if(d/100 == 0)
											temp->bar[0] --;
										else
											temp->board[d/100]--;
										temp->board[d/100 + d%100]++;
									}
								}
							}
						}
						move_counter = 0;
						temp = createNode(temp2->board,temp2->bar,temp2->move);
					}
				}
			}
		}
	}
	return move_counter;
}

double setUtil(int board[],int bar[])		//Haven't taken into account the value/bonus for sending bob's checker in the bar. 
{
	int count=0,count_bob=0;
	double val = 0;
	val += (-25*bar[0]);
	val += (25*bar[1]);
	for(int i=1;i<25;i++)
	{
		if(board[i] > 0)
		{
			count++;
			if(board[i] == 1)
				val -= 15;
			else if(i<19)
				val += (i*board[i]);
			else if(i >= 19)
				val += (2*i*board[i]);
		}
		else if(board[i] < 0)
		{
			count_bob ++;
			if(board[i] == -1)
				val += 15;
			else if(i > 6)
				val -= ((25-i)*board[i]);
			else if(i <= 6)
				val -= (2*(25-i)*board[i]);
		}
	}
	val += (25*(15-count-bar[0]));
	val -= (25*(15-count_bob-bar[1]));
	return val;
}

void compute_avg(tree *root)
{
	double sum;
	int count;
	tree *temp = root,*temp2;
	root=root->child;
	while(root!=NULL)
	{
		sum=0;
		count=0;
		temp2 = root;
		root=root->child;
		while(root!=NULL)
		{
			sum+=root->utility_val;
			count++;
			root=root->sibling;
		}
		root=temp2;
		root->utility_val+=sum/count;
		root=temp2->sibling;
	}
}

int main()
{
	char c,bar_temp[30];
	int die[2],bar[2]={0,0},board[25],move[2]={0,0};
	tree *root=NULL;

	for(int i=0;i<30;i++)
		bar_temp[i]='\0';

	//scan initial board state.
	for (int i=1; i<25; i++)
		scanf("%d",&(board[i]));

	//scan initial bar state.
	int i=0,min=INT_MAX,count=0,min_arr[50] = {0};
	double sum;
	c=getchar();
	while((c=getchar())!='\n')
		bar_temp[i++] = c;

	for(int i=0;bar_temp[i]!='\0';i++)
		if(bar_temp[i] == 'a')
			bar[0]++;
		else if(bar_temp[i] == 'b')
			bar[1]++;

	for (int i=0; i<2; i++)
		scanf("%d",&die[i]);

	root = insertNode(root,board,bar,move);

	generateStates(root,die);

	tree *temp = root,*prev,*original;
	root=root->child;
	prev = root->child;
	while(root!=NULL)
	{
		for(int i=1;i<7;i++)
			for(int j=1;j<7;j++)
			{
				count++;
				die[0] = i;
				die [1] = j;
				generateStates(root,die);
				original = root;
				root=prev;
				while(root!=NULL)
				{
					if(root->utility_val < min)
						min = root->utility_val;
					prev = root;
					root = root->sibling;
				}
				root = original;
				min_arr[count++] = min;
				min=INT_MAX;
			}
		for(int i=0;i<count;i++)
			sum+=min_arr[i];
		root->utility_val += (sum/count);
		count = 0;
		root = root->sibling;
	}
	root=temp;
	compute_avg(root);
	double arr[50];
	int counter = 0;
	root=root->child;
	while(root != NULL)
	{
		arr[counter++] = root->utility_val;
		root=root->sibling;
	}

	sort(arr,arr+counter);
	reverse(arr,arr+counter);

	double ans;
	if(counter >= 3)
	{
		double diff=arr[0]-arr[2];
		srand(time(NULL));								//Initializing the random number generator
		if((diff/arr[0]) <= 0.005)						//Difference between the max values should be less than 0.5%
			ans=arr[rand()%3];
		else
			ans = arr[0];
	}
	else
		ans = arr[0];
	//cout<<ans<<endl;
	root=temp;
	root=root->child;
	//preorder(root);
	while(root != NULL)
	{
		if(root->utility_val == ans)
		{
			move[0] = root->move[0];
			move[1] = root->move[1];
			break;
		}
		root=root->sibling;
	}

	for(int i=0;i<2;i++)
	{
		if(move[i] == 0)
			printf("pass\n");
		else if(move[i] / 100 == 0)
			printf("Z %d\n",move[i]%100);
		else
		{
			printf("%d ",move[i]/100);
			if(move[i] % 100 > 24)
				printf("O\n");
			else
				printf("%d\n",move[i]/100 + move[i]%100);
		}
	}
	return 0;
}
