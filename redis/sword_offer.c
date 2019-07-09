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

# 29
void helper(vector<int>& input,int l,int r,int k){

    int pivot = input[r];
    int i = l, j = r;
    while(i<j){

        while(i<j&&input[i]<=pivot)i++;
        input[j] = input[i];
        while(i<j&&input[j]>=pivot)j--;
        input[i] = input[j];
    }
    input[i] = pivot;
    if(i+1<k)
        helper(input,i+1,r,k);
    else if(i+1>k)
        helper(input,l,r-1,k);

}
vector<int> GetLeastNumbers_Solution(vector<int> input, int k) {

    if(input.size()<=k)
        return input;
    else
        helper(input,0,input.size()-1,k);
    return vector<int>(input.begin(),input.begin()+k);
}
// 方法二 上面方法超时未通过
vector<int> GetLeastNumbers_Solution(vector<int> input, int k) {

    if(input.size()<k)
        return {};
    else{
        make_heap(input.begin(),input.end(),less<int>());
        for(int i=0;i<input.size()-k;i++){
            pop_heap(input.begin(),input.begin()+input.size()-i);
        }
    }
    return vector<int>(input.begin(),input.begin()+k);
}

# 30
int FindGreatestSumOfSubArray(vector<int> array) {
    
    int amount = 0;
    int maxSum = INT_MIN;
    for(int i = 0;i<array.size();i++){
        amount = max(amount+array[i],array[i]);
        maxSum = max(maxSum,amount);
    }
    return maxSum;
}

# 31
int NumberOf1Between1AndN_Solution(int n){
    
    }

# 32
static bool cmp(const string& st1, const string& st2){

    return st1+st2<st2+st1;
}
string PrintMinNumber(vector<int> numbers) {

    vector<string> res;
    for(int i=0;i<numbers.size();i++){
        res.push_back(to_string(numbers[i]));
    }
    sort(res.begin(),res.end(),cmp);
    string ans;
    for(int i=0;i<res.size();i++){
        ans+=res[i];
    }
    return ans;
}

# 33
int GetUglyNumber_Solution(int index) {
    
    if(index < 7) return index;
    vector<int> data;
    data.push_back(1);
    int index2 = 0, index3 = 0, index5 = 0;
    while(--index){
        int minT = min(data[index2]*2,data[index3]*3);
        minT = min(minT,data[index5]*5);
        if(minT==data[index2]*2) index2++;
        if(minT==data[index3]*3) index3++;
        if(minT==data[index5]*5) index5++;
        data.push_back(minT);
    }
    return data.back();
}

# 34
int FirstNotRepeatingChar(string str) {
        
    unordered_map<char,int> mp;
    for(int i=0;i<str.size();i++){
        if(mp.find(str[i])==mp.end())
            mp[str[i]] = 1;
        else mp[str[i]] += 1;
    } 
    for(int i=0;i<str.size();i++){

        if(mp[str[i]]==1) return i;
    } 
    return -1;
}

