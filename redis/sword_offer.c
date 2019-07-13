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

# 34 找到第一个未重复的字符
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

# 35 数组中的逆序对
long long InversePairsCore(vector<int>&data,vector<int>&copy,int start,int end){
    if(start==end) {
        copy[start] = data[start];
        return 0;
    }
    int length = (end - start)/2;
    long long left = InversePairsCore(copy,data,start,start+length);
    long long right = InversePairsCore(copy,data,start+length+1,end);

    int i = start+length;
    int j = end;
    int indexcopy = end;
    long long cnt = 0;
    while(i>=start&&j>=start+length+1){
        if(data[i]>data[j]){
            copy[indexcopy--] = data[i--];
            cnt = cnt+j-start-length;
        }
        else{
            copy[indexcopy--] = data[j--];
        }
    }
    for(;i>=start;i--) copy[indexcopy--] = data[i];
    for(;j>=start+length+1;j--)copy[indexcopy--] = data[j];
    return left+right+cnt;
}
int InversePairs(vector<int> data) {
     
        int length = data.size();
        if(length<=0) return 0;

        vector<int> copy;
        for(int i=0;i<length;i++){
            copy.push_back(data[i]);
        }

        long long count=InversePairsCore(data,copy,0,length-1);
        return count%1000000007;
    }

# 36 找出两个链表的第一个公共结点
ListNode* FindFirstCommonNode( ListNode* pHead1, ListNode* pHead2) {
        int cnt1 = 0, cnt2 = 0;
        ListNode* node = pHead1;
        while(node){
        	node = node->next;
        	cnt1++;
        }
        node = pHead2;
        while(node){
        	node = node->next;
        	cnt2++;
        }

        if(cnt1>cnt2){
        	int dis = cnt1 - cnt2;
        	while(dis--) pHead1 = pHead1->next;
        }else if(cnt1<cnt2){
        	int dis = cnt2 - cnt1;
        	while(dis--) pHead2 = pHead2->next;
        }

        while(pHead2!=pHead1){
        	pHead2 = pHead2->next;
        	pHead1 = pHead1->next;
        }
        return pHead1;

    }

# 37 统计一个数字在排序数组中出现的次数
// 将上下界的代码合二为一
int biSearch(const vector<int> & data, double num){
        int s = 0, e = data.size()-1;     
        while(s <= e){
            int mid = (e - s)/2 + s;
            if(data[mid] < num)
                s = mid + 1;
            else if(data[mid] > num)
                e = mid - 1;
        }
        return s;
    }

int GetNumberOfK(vector<int> data ,int k) {
        return biSearch(data, k+0.5) - biSearch(data, k-0.5) ;
    }

# 38 树的深度
int TreeDepth(TreeNode* pRoot){
    
    if(pRoot==NULL) return 0;
    queue<TreeNode*> Q;
    Q.push(pRoot);
    int cnt = 0;
    while(Q.size()){
    	cnt++;
    	int size = Q.size();
    	for(int i = 0;i<size;i++){
    		TreeNode* node = Q.front();
    		if(node->left) Q.push(node->left);
    		if(node->right) Q.push(node->right);
    		Q.pop();
    	}
    }
    return cnt;
    }
// 方法二
int TreeDepth(TreeNode* pRoot){
	if(!pRoot) return 0;

	return max(1+TreeDepth(pRoot->left),1+TreeDepth(pRoot->right));
}

# 39 平衡二叉树判断
bool IsBalanced_Solution(TreeNode* root) {
    if(root == NULL) {
        return true;
    }
    return abs(maxDepth(root->left) - maxDepth(root->right)) <= 1 &&
        IsBalanced_Solution(root->left) && IsBalanced_Solution(root->right);
    }
      
    int maxDepth(TreeNode* root) {
        if(root == NULL) {
            return 0;
        }
        return 1 + max(maxDepth(root->left), maxDepth(root->right));
    }
// 方法二 简单
int getDepth(TreeNode* root) {
    if (root == NULL) return 0;
   	int left = getDepth(root->left);
    if (left == -1) return -1;
    int right = getDepth(root->right);
    if (right == -1) return -1;
    return abs(left - right) > 1 ? -1 : 1 + max(left, right);
    }
bool IsBalanced_Solution(TreeNode* pRoot) {
	 return getDepth(pRoot) != -1;
    }

# 40 查找只出现一次的两个数
void FindNumsAppearOnce(vector<int> data,int* num1,int *num2) {

    int n = data.size();
    if(data.size() < 2) return ;
    int index = 0,flag = 1;
    for(int i=0;i<n;i++){
        index ^= data[i];
    }
    while((index&flag)==0) flag<<=1;
    *num1 = index;
    *num2 = index;

    for(int i = 0; i < n; ++ i ){
            if((flag & data[i]) == 0) *num2 ^= data[i];
            else *num1 ^= data[i];
        }
    }

# 41 和为S的连续正整数序列
vector<vector<int> > FindContinuousSequence(int sum) {
        vector<vector<int> > res;
        deque<int> ans;
        int tmp = 0;
        for(int i = 1;i<sum;i++){
            tmp+=i; ans.push_back(i);
            while(tmp>sum){
                tmp-=ans.front();
                ans.pop_front();
            }
            if(tmp==sum) res.push_back(vector<int>(ans.begin(),ans.end()));
        }
        return res;
    }

# 42 和为S的两个数
vector<int> FindNumbersWithSum(vector<int> array,int sum) {
        
        vector<int> res;
        int l = 0, r = array.size()-1;
        while(l<r){
            while(l<r&&array[l]+array[r]>sum) r--;
            while(l<r&&array[l]+array[r]<sum) l++;
            if(array[l]+array[r]==sum){
                if(res.empty()) { 
                    res.push_back(array[l]);
                    res.push_back(array[r]);
                } else if(res[0]*res[1]>array[l]*array[r]){
                    res[0] = array[l];
                    res[1] = array[r];
                }
                l++;
            } 
        }
        return res;
    }

# 43 左旋转字符串
string LeftRotateString(string str, int n) {
        
        int length = str.size();
        if(length==0) return "";
        n = n%length;
        str+=str;
        return str.substr(n,length);
    }

# 44 翻转单词顺序列
void revesehelper(string& str, int l, int r) {
	while (l < r) { swap(str[l++], str[r--]); }
}
string ReverseSentence(string str) {

	int n = str.size();
	int front = -1, last = 0;
	for (; last < n; last++) {
		if (front != -1 && str[last] == ' ') {
			revesehelper(str, front, last - 1);
			front = -1;
		}
		else if (front == -1 && str[last] != ' ')front = last;
	}
	if (front != -1) revesehelper(str, front, last - 1);
	revesehelper(str, 0, n - 1);
	return str;
}

# 45 扑克牌顺子
bool IsContinuous( vector<int> numbers ) {
        
        if(numbers.empty()) return false;
        sort(numbers.begin(),numbers.end());
        int tmp = 0;
        int pos = 0;
        while(pos<numbers.size()&&numbers[pos]==0) {tmp++;pos++;}
        for(pos+=1;pos<numbers.size();pos++){

        	int dis = numbers[pos] - numbers[pos-1] - 1;
        	if(dis<0) return false;
        	tmp -= dis;
        	if(tmp>=0) continue;
        	else return false;
        }
        return true;
    }

# 46 圆圈中最后剩下的数 约瑟夫环问题
int LastRemaining_Solution(int n, int m){
        int p = 0;
        for(int i=2;i<=n;i++){
        	p = (p+m)%i;
        }
        return p;
    }

# 47 累加 短路求值原理
int Sum_Solution(int n) {
        int ans = n;
        ans&&(ans+=Sum_Solution(n-1));
        return ans;
    }

# 48 整数之和
int Add(int num1, int num2){

	int n1,n2;
	n1 = (num1&num2)<<1;  // 进位
	n2 = num1^num2;       // 非进位

	while(n1&n2){
		num1=n1;num2=n2;
        n1=(num1&num2)<<1;
        n2=num1^num2;
	}
	return n1|n2;
    }

# 49 字符串转整数
int StrToInt(string str) {
        int n = str.size(), s = 1;
        long long res = 0;
        if(!n) return 0;
        if(str[0] == '-') s = -1;
        for(int i = (str[0] ==  '-' || str[0] == '+') ? 1 : 0; i < n; ++i){
            if(!('0' <= str[i] && str[i] <= '9')) return 0;
            res = (res << 1) + (res << 3) + (str[i] & 0xf);//res=res*10+str[i]-'0';
        } 
        return res * s;
    }

# 50 找出任意重复数字 注意与leetcode中《数据中重复的数据》之间的区别
bool duplicate(int numbers[], int length, int* duplication) {
        
        for(int i = 0;i<length;i++){
        	while(numbers[i]!=i){

        		if(numbers[numbers[i]]==numbers[i]){
        		*duplication = numbers[i];
        		return true;
        		}
        		swap(numbers[i],numbers[numbers[i]]);
        	}
        }
        return false;
    }