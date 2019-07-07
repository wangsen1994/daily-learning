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
    long long res = 1;

    if(base==0) return 0;
    if(exponent==0) return 1;
    if(exponent>0){
        while(exponent--){
            res *= base;
        }
        return res;
    }
    else {
        while(exponent++){
            res *= base;
        }
        return 1.0/res;
    }
    }

# 13
void reOrderArray(vector<int> &array) {
        
        int n = array.size();

        bool flag = true;
        for(int i = 0;i<n-1&&flag;i++){

            flag = false;
            for(int j = n - 1;j>i;j--){
                if(array[j]%2==1&&array[j-1]%2==0){
                    swap(array[j],array[j-1]);
                    flag = true;
                }
            }
        }
    }

# 14
ListNode* FindKthToTail(ListNode* pListHead, unsigned int k) {
    
    ListNode* node = pListHead;
    while(--k&&node) node = node->next;
    if(!node) return NULL;

    while(node->next){
        pListHead = pListHead->next;
        node = node->next;
    }
    return pListHead;
    }

# 15
ListNode* ReverseList(ListNode* pHead) {

    if(!pHead||!pHead->next) return pHead;
    ListNode* node = ReverseList(pHead->next);
    pHead->next->next = pHead;
    pHead->next = NULL;
    return node;
    }

# 16
ListNode* Merge(ListNode* pHead1, ListNode* pHead2){
        
		ListNode* dummy = new ListNode(-1);
		ListNode* cur = dummy;

		while(pHead1||pHead2){

			int tmp1 = pHead1?pHead1->val:INT_MAX;
			int tmp2 = pHead2?pHead2->val:INT_MAX;

			if(tmp1>tmp2){

				cur->next = pHead2;
				pHead2 = pHead2->next;
				cur = cur->next;
			}else {
				cur->next = pHead1;
				pHead1 = pHead1->next;
				cur = cur->next;
			}
		}
		return dummy->next;
    }

# 17
bool helper(TreeNode* pRoot1, TreeNode* pRoot2){
	if(!pRoot2) return true;
	if(!pRoot1) return false;
	if(pRoot1->val==pRoot2->val){
		return helper(pRoot1->left,pRoot2->left)&&helper(pRoot1->right,pRoot2->right);
	}else return false;
}
bool HasSubtree(TreeNode* pRoot1, TreeNode* pRoot2){

	if(pRoot1==NULL||pRoot2==NULL) return false;

	return helper(pRoot1,pRoot2)|| HasSubtree(pRoot1->left,pRoot2)||HasSubtree(pRoot1->right,pRoot2);
}

# 18
void Mirror(TreeNode *pRoot) {
	if(pRoot==NULL) return;
	if(!pRoot->left&& !pRoot->right) return;
	TreeNode* tmp = pRoot->left;
    pRoot->left = pRoot->right;
    pRoot->right = tmp;
         
    if(pRoot->left) Mirror(pRoot->left);
    if(pRoot->right) Mirror(pRoot->right);
    }

# 19
vector<int> printMatrix(vector<vector<int> > matrix) {
	int n = matrix.size(),m=matrix[0].size();
	if(n==1) return matrix[0];
	int circle=((n<m?n:m)-1)/2+1; /* 重点 */
	vector<int> res;
	for(int i = 0;i<circle;i++){

		for(int j=i;i<m-i;j++)res.push_back(matrix[i][j]);
		for(int j=i+1;k<n-i;j++)res.push_back(matrix[j][m-i-1]);
		for(int j=m-i-2;j>=i&&(n-i-1!=i);j--)res.push_back(matrix[n-i-1][j]);
		for(int j=n-i-2;j>i&&(m-i-1!=i);j--)res.push_back(matrix[j][i]);
	}
	return res;
    }
# 20
class Solution {
public:
    void push(int value) {
        st.push(value);
        if(stmin.size()&&value>stmin.top())stmin.push(stmin.top());
        else stmin.push(value);
    }
    void pop() {

    	st.pop();
    	stmin.pop();
        
    }
    int top() {
        return st.top();
    }
    int min() {
        return stmin.top();
    }
    stack<int> st;
    stack<int> stmin;
};

# 21
bool IsPopOrder(vector<int> pushV,vector<int> popV) {
        
        stack<int> st;
        int pos = 0;
        for(int i=0;i<pushV.size();i++){
        	if(st.empty()||st.top()!=popV[pos]) {	
        		st.push(pushV[i]);
        	}
        	while(st.size()&&pos<popV.size()&&st.top()==popV[pos]){
        		st.pop();
        		pos++;
        	}
       	}
       	return st.empty()?true:false;
}

# 22
vector<int> PrintFromTopToBottom(TreeNode* root) {

	if(!root) return {}; 
	queue<TreeNode*> qu;
	qu.push(root);
	vector<int> res;
	while(!qu.empty()){

		int size = qu.size();
		for(int i=0;i<size;i++){

			TreeNode* node = qu.front();
			res.push_back(node->val);
			if(node->left) qu.push(node->left);
			if(node->right) qu.push(node->right);
			qu.pop();
		}
	}
	return res;
    }

# 23
bool helper(vector<int> sequence,int start,int end){

	if(start>=end) return true;
	int i = end;
	while(i>start&&sequence[i-1]>sequence[end])--i;
	for(int j = i-1;j>=start;j--)if(sequence[j]>sequence[end]) return false;
	return helper(sequence,start,i-1)&&helper(sequence,i,end-1);
}

bool VerifySquenceOfBST(vector<int> sequence) {
	if(sequence.size()==0) return false;
	return helper(sequence,0,sequence.size()-1);
}

# 24
void search(TreeNode* root,vector<vector<int>>& res,vector<int>& ans,int expectNumber,int tmp){

	ans.push_back(root->val);tmp+=root->val;
	if(!root->left&&!root->right&&tmp==expectNumber)res.push_back(ans);
	else {

		if(root->left) search(root->left,res,ans,expectNumber,tmp);
		if(root->right) search(root->right,res,ans,expectNumber,tmp);
	}
	ans.pop_back();
	tmp-=root->val;
}
vector<vector<int> > FindPath(TreeNode* root,int expectNumber) {
	vector<vector<int> > res;
	vector<int> ans;
	if(root) search(root,res,ans,expectNumber,0);
	return res;
    }

# 25
RandomListNode* Clone(RandomListNode* pHead){
        
    }

# 26
TreeNode* Convert(TreeNode* pRootOfTree){
        
    }

# 27
void helper(string str,vector<string>&res,int pos){
	if(pos==str.size()-1) res.push_back(str);
	else{
		set<char> st;
		for(int i=pos;i<str.size();i++)
		{
			if(set.empty()||set.find(str[i])==st.end()){
				swap(str[i],str[pos]);
				helper(str,res,pos+1);
				swap(str[i],str[pos]);
				st.insert(str[i]);
			}
		}
	}
}
vector<string> Permutation(string str) {
        
        if(str.size()==1) return {str};
        vector<string> res;
        helper(str,res,0);
        sort(res.begin(),res.end());
        return res;
    } 

# 28
int MoreThanHalfNum_Solution(vector<int> numbers) {
   
	map<int,int>m;
	for(int i=0;i<numbers.size();i++){
		if(m.find(numbers[i])==m.end()) m[numbers[i]] = 1;
		else m[numbers[i]] += 1;
	}
	for(int i = 0;i<m.size();i++){
		if(m[numbers[i]]>numbers.size()/2) return numbers[i];
	}
	return 0;
    } 
# 29