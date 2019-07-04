# 1
bool Find(int target, vector<vector<int> > array) {
  	
  	int n = array.size(), m = array[0].size();
  	int x = n-1, y = 0;

  	while(x>=0&&y<m){

  		if(array[x][y]==target) return true;
  		else if(array[x][y] > target) x--;
  		else y++;
  	}
  	return false;

    }
# 2
void replaceSpace(char *str,int length) {
	
	if(str==NULL||length<0)return ;
	int cnt = 0;
	int oldl = 0;
	for(int i = 0; str[i]!='\0';i++){
		oldl++;
		if(str[i]==' ') cnt++;
	}
	int newl = oldl + cnt*2;
	if(newl > length) return;

	while(oldl>=0&&newl>oldl){

		if(str[oldl]==' '){
			str[newl--] = '0'; 
			str[newl--] = '2'; 
			str[newl--] = '%'; 
		}
		else str[newl--] = str[oldl];
		oldl--;
	}

	}

# 3
vector<int> printListFromTailToHead(ListNode* head) {
        
        if(!head) return {};
        stack<int> st;

        while(head){

        	st.push(head->val);
        	head = head->next;
        }
        vector<int> res;
        while(st.size()){
        	res.push_back(st.top());
        	st.pop();
        }
        return res;
    }


# 4
TreeNode* helper(vector<int> pre,vector<int> vin,int prel,int prer,int inl,int inr){
	
	if(prel>prer||inl>inr) return NULL;

	TreeNode* node = new TreeNode(pre[prel]);

	for(int i=inl;i<=inr;i++){
		if(pre[prel] == vin[i]){
			node->left = helper(pre,vin,prel+1,prel+i-inl,inl,i-1);
			node->right = helper(pre,vin,prel+i+1-inl,prer,i+1,inr);
		}
	}

	return node;

}
TreeNode* reConstructBinaryTree(vector<int> pre,vector<int> vin) {

	return helper(pre,vin,0,pre.size()-1,0,vin.size()-1);

    }

# 5
class Solution
{
public:
    void push(int node) {
        
        stack2.push(node);
    }

    int pop() {

    	if(stack1.empty()){
    		while(stack2.size()){

    			stack1.push(stack2.top());
    			stack1.pop();
    		}
    	}
    	int res = stack1.top();
    	stack1.pop();
    	return res;  
    }

private:
    stack<int> stack1;
    stack<int> stack2;
};

# 6
int minNumberInRotateArray(vector<int> rotateArray) {
        
        int n = rotateArray.size();
        if(n==0)return 0;

        int l = 0, r = n-1;

        while(l<=r){ /* 必须加等号 */

        	int mid = l + (r-l)/2;
        	if(rotateArray[mid]>rotateArray[0]) l = mid + 1;
        	else if(rotateArray[mid]<rotateArray[n-1]) r = mid - 1;
        	else if(rotateArray[mid]==rotateArray[0]&&rotateArray[mid]==rotateArray[n-1]){
        		int m = rotateArray[l];
        		for(int i = l;i>=r;i++){
        			m = min(m,rotateArray[i]);
        		}
        		return m;
        	}
        }
        return rotateArray[l];
    }

# 7
int Fibonacci(int n) {

	vector<int> data = {0,1};
	if(n<=1) return data[n];

	else{

		while(--n>0){

			int tmp = data.back();
			data[1] += data[0];
			data[0] = tmp;
		}
	}

	return data.back();

    }

# 8
int jumpFloor(int number) {
       
    vector<int> data = {0,1,2};

	if(number<=2) return data[number];

	else{

		number -= 1;
		while(--number>0){

			int tmp = data.back();
			data[2] += data[1];
			data[1] = tmp;
		}
	}

	return data.back()；

    }

# 9
int jumpFloorII(int number) {

    vector<int> data = {1,1};
    if(number<2) return data[number];
    else{

        while(--number>0){
            int tmp = data.back();
            data[1] += data[1];
            data[0] = tmp;
        }
    }
    return data.back();
    }

# 10
int rectCover(int number) {
    vector<int> data{0,1,2};
    if(number<=2) return data[number];
    else{
        while(--number>1){
            data[0] = data[1];
            data[1] = data[2];
            data[2] = data[0] + data[1];
        }
    }
    return data.back();

    }

# 11
int  NumberOf1(int n) {
    
    int cnt = 0;
    while(n){
        cnt++;
        n = n &( n -1);
    }
    return cnt;
    }

# 12  
double Power(double base, int exponent) {
    long long res;

    
    }








