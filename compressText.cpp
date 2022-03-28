#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include <unordered_map>
#include <algorithm>
using namespace std;


//哈夫曼树节点
struct HCTNode {
	char ch;
	HCTNode *left;
	HCTNode *right;

	HCTNode(): ch(0), left(nullptr), right(nullptr) {}
	HCTNode(char _ch): ch(_ch), left(nullptr), right(nullptr) {}
	HCTNode(HCTNode *_left, HCTNode *_right): ch(0), left(_left), right(_right) {}
	HCTNode(char _ch, HCTNode *_left, HCTNode *_right): ch(_ch), left(_left), right(_right) {}
};

//顺序哈夫曼树节点
struct HCANode {
	char ch;
	int left;
	int right;
	HCANode(): ch(0), left(-1), right(-1) {}
	HCANode(char _ch): ch(_ch), left(-1), right(-1) {}
	HCANode(int _left, int _right): ch(0), left(_left), right(_right) {}
	HCANode(char _ch, int _left, int _right): ch(_ch), left(_left), right(_right) {}
};


//通过文本获取词频
unordered_map<char, int> getFreq(string &text);
//通过词频构建哈夫曼树
HCTNode *getHCodeTree(unordered_map<char, int> &freq);
//通过树状存储哈夫曼树构建哈夫曼编码
void getHCode(HCTNode *root, string u, unordered_map<char, string> &HCode);
//通过顺序存储哈夫曼树构建哈夫曼编码
void getHCode(vector<HCANode *> &arry, int idx, string u, unordered_map<char, string> &HCode);
//通过哈夫曼编码转化文本
string compressText(string &text, unordered_map<char, string> &HCode);
//获得压缩文本的文件名
string getCompFileName(string fileName);
//获得解压缩文本的文件名
string getDecompFileName(string fileName);
//通过顺序哈夫曼树解压压缩的文本
string getOrgText(string &compText, vector<HCANode *> HArry);
//将int按位分割为四位的string
string num2str(int num);
//将四位字符串转换为int
int str2num(string s);
//将哈夫曼树转换为数组形式
void HTree2HArry(HCTNode *root, vector<HCANode *> &HArry);
//将顺序哈夫曼树转换字符串
string HArry2str(vector<HCANode *> &HArry);
//将字符串还原为顺序哈夫曼树
vector<HCANode *> str2HArry(string &s);


bool compress(string fileName);
bool decompress(string fileName);
void help();


int main(int argc, const char *argv[]) {
	if (argc <= 1) {
		cout << "请输入参数，输入'-h'获得帮助" << endl;
	} else if (!strcmp(argv[1], "-h")) {
		help();
	} else if (!strcmp(argv[1], "-zip") && argc >= 3) {
		if (!compress(argv[2]))
			cout << "压缩失败" << endl;
		else
			cout << "压缩成功" << endl;
	} else if (!strcmp(argv[1], "-unzip") && argc >= 3) {
		if (!decompress(argv[2]))
			cout << "解压失败" << endl;
		else
			cout << "解压成功" << endl;
	} else {
		cout << "参数错误，输入'-h'获得帮助" << endl;
	}
//	compress("./text.txt");
//	decompress("./text.cod");
	return 0;
}


//通过文本获取词频
unordered_map<char, int> getFreq(string &text) {
	unordered_map<char, int> freq;
	for (char ch : text) {
		freq[ch]++;
	}
	return freq;
}


//通过词频构建哈夫曼树
HCTNode *getHCodeTree(unordered_map<char, int> &freq) {
	priority_queue <pair<int, HCTNode *>, vector<pair<int, HCTNode *>>, greater<pair<int, HCTNode *>>> q;

	for (auto &[ch, f] : freq) {
		q.emplace(f, new HCTNode(ch));
	}

	while (q.size() > 1) {
		auto [num1, node1] = q.top();
		q.pop();
		auto [num2, node2] = q.top();
		q.pop();
		q.emplace(num1 + num2, new HCTNode(node1, node2));
	}

	return q.top().second;
}


//通过树状存储哈夫曼树构建哈夫曼编码
void getHCode(HCTNode *root, string u, unordered_map<char, string> &HCode) {
	if (!root)
		return;
	if (root->ch) {
		HCode[root->ch] = u;
	}
	getHCode(root->left, u + "1", HCode);
	getHCode(root->right, u + "0", HCode);
}


//通过顺序存储哈夫曼树构建哈夫曼编码
void getHCode(vector<HCANode *> &arry, int idx, string u, unordered_map<char, string> &HCode) {
	if (idx == -1)
		return;
	if (arry[idx]->ch) {
		HCode[arry[idx]->ch] = u;
	}

	getHCode(arry, arry[idx]->left, u + "1", HCode);
	getHCode(arry, arry[idx]->right, u + "0", HCode);
}


//通过哈夫曼编码转化文本
string compressText(string &text, unordered_map<char, string> &HCode) {
	string comp;
	int idx = 0;
	char comp_t = 0;

	for (char ch : text) {
		for (char t : HCode[ch]) {
			comp_t |= (t == '1' ? (1 << (idx & 7)) : 0);
			if (((idx++) & 7) == 7) {
				comp.push_back(comp_t);
				comp_t = 0;
			}
		}
	}

	if (idx & 7)
		comp.push_back(comp_t);

	comp.push_back((char)((8 - idx % 8) % 8));

	return comp;
}


//获得压缩文本的文件名
string getCompFileName(string fileName) {
	int len = fileName.length();
	long long idx = len - 1;

	while (idx >= 0 && fileName[idx] != '.')
		idx--;
	if (!idx)
		return fileName + ".cod";

	return fileName.substr(0, idx) + ".cod";
}


//获得解压缩文本的文件名
string getDecompFileName(string fileName) {
	int len = fileName.length();
	long long idx = len - 1;

	while (idx >= 0 && fileName[idx] != '.')
		idx--;
	if (!idx)
		return fileName + ".txt";

	return fileName.substr(0, idx) + ".txt";
}


//通过顺序哈夫曼树解压压缩的文本
string getOrgText(string &compText, vector<HCANode *> HArry) {
	string orgText;
	int redun = compText.back() + 8;
	long long len = (long long)(compText.length()) * 8 - redun;

	HCANode *tmp = HArry[0];
	for (long long idx = 0; idx < len; idx++) {
		tmp = ((compText[idx / 8] & (1 << (idx % 8))) ? HArry[tmp->left] : HArry[tmp->right]);

		if (tmp->ch) {
			orgText.push_back(tmp->ch);
			tmp = HArry[0];
		}
	}

	return orgText;
}


//将int按位分割为四位的string
string num2str(int num) {
	string ans(4, 0);
	char ans_t[4];
	memcpy(ans_t, &num, 4);
	for (int i = 0; i < 4; i++)
		ans[i] = ans_t[i];
	return ans;
}


//将四位字符串转换为int
int str2num(string s) {
	int ans = 0;
	memcpy(&ans, s.c_str(), 4);
	return ans;
}


//将哈夫曼树转换为数组形式
void HTree2HArry(HCTNode *root, vector<HCANode *> &HArry) {
	HCANode *node = new HCANode(root->ch);
	HArry.push_back(node);

	if (root->left) {
		node->left = HArry.size();
		HTree2HArry(root->left, HArry);
	}
	if (root->right) {
		node->right = HArry.size();
		HTree2HArry(root->right, HArry);
	}
}

//将顺序哈夫曼树转换字符串
string HArry2str(vector<HCANode *> &HArry) {
	string ans = num2str(HArry.size());
	for (auto &node : HArry) {
		ans.push_back(node->ch);
		ans += num2str(node->left) + num2str(node->right);
	}
	return ans;
}

//将字符串还原为顺序哈夫曼树
vector<HCANode *> str2HArry(string &s) {
	vector<HCANode *> ans;
	int len = s.length();
	if (!len || len % 9)
		return ans;
	for (int i = 0;  i < len; i += 9) {
		ans.push_back(new HCANode(s[i], str2num(s.substr(i + 1, 4)), str2num(s.substr(i + 5, 4))));
	}
	return ans;
}

bool compress(string fileName) {
	ifstream fin;
	ofstream fout;


	//文件输入
	fin.open(fileName, ifstream::binary);
	if (!fin.is_open()) {
		cout << "文件读取失败！" << endl;
		return false;
	}
	fout.open(getCompFileName(fileName), ofstream::binary);


	//读取文本
	string text;
	stringstream buffer;
	buffer << fin.rdbuf();
	text = buffer.str();


	//根据词频创建编码表
	unordered_map<char, int> freq = getFreq(text);
	HCTNode *root = getHCodeTree(freq);
	unordered_map<char, string> HCode;
	getHCode(root, "", HCode);


	//将顺序化哈夫曼树
	vector<HCANode *>  HArry;
	HTree2HArry(root, HArry);
	string HArrystr = HArry2str(HArry);


	//根据编码表压缩文本并存入哈夫曼树作为压缩结果
	string compText = HArrystr + compressText(text, HCode);


	//将结果输出
	fout.write(compText.c_str(), compText.length());


	//数据流关闭
	fin.close();
	fout.close();
	return true;
}

bool decompress(string fileName) {
	ifstream fin;
	ofstream fout;

	//文件输入
	fin.open(fileName, ifstream::binary);
	if (!fin.is_open()) {
		cout << "文件读取失败！" << endl;
		return false;
	}
	fout.open(getDecompFileName(fileName), ofstream::binary);


	//读取压缩文本
	string text;
	stringstream buffer;
	buffer << fin.rdbuf();
	text = buffer.str();


	//将压缩文件各组件分割出来
	int len = str2num(text.substr(0, 4));
	string HArrystr = text.substr(4, len * 9);
	string compText = text.substr(4 + len * 9);


	//通过压缩文件中的信息恢复文本
	vector<HCANode *> orgHArry = str2HArry(HArrystr);
	string orgText = getOrgText(compText, orgHArry);

//	for (auto &t : orgHArry) {
//		cout << t->ch << " " << t->left << " " << t->right << endl;
//	}

	//结果输出
	fout.write(orgText.c_str(), orgText.length());


	//数据流关闭
	fin.close();
	fout.close();
	return true;
}

void help() {
	cout << "* -h\t\t\t帮助" << endl;
	cout << "* -zip filename\t\t压缩" << endl;
	cout << "* -unzip filename\t解压" << endl;
}