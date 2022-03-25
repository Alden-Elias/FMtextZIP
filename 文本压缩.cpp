#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <queue>
#include <unordered_map>
#include <stringstream>
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



//整个文本
//词频
//哈夫曼编码
vector<string> text;
unordered_map<char, int> freq_char;
unordered_map<char, string> HCode;



//通过文本获取词频
void getFreq(vector<string> &text) {
	for (string &s : text) {
		for (char ch : s) {
			freq_char[ch]++;
		}
	}
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


//通过哈夫曼树构建哈夫曼编码
void getHCode(HCTNode *root, string u = "") {
	if (!root)
		return;
	if (root->ch) {
		HCode[root->ch] = u;
	}
	getHCode(root->left, u + "1");
	getHCode(root->right, u + "0");
}


//总体上构建哈夫曼编码
void buildHCode() {
	getFreq(text);
	HCTNode *root = getHCodeTree(freq_char);
	getHCode(root);
}

//通过哈夫曼编码转化文本
vector<string> compressText(vector<string> text) {
	vector<string> ans;
	for (string &s : text) {
		int idx = 0;
		string comp;
		char comp_t = 0;
		for (char ch : s) {
			for (char t : HCode[ch]) {
				comp_t |= (t == '1' ? 1 << (idx % 8) : 0);
				if (++idx % 8 == 7) {
					comp.push_back(comp_t);
					comp_t = 0;
				}
			}
		}

		if (idx % 8)
			comp.push_back(comp_t);

		ans.emplace_back(comp);
	}
	return ans;
}

//获得压缩文本的
string getCompFileName(string fileName) {
	int len = fileName.length();
	int idx = len - 1;

	while (idx >= 0 && fileName[idx] != '.')
		idx--;
	if (!idx)
		return fileName + ".cod";

	return fileName.substr(0, idx) + ".cod";
}

int main(int argc, const char *argv[]) {
	ifstream fin;
	ofstream fout;

	//文件输入
	if (argc > 1) {
		fin.open(argv[1]);
		if (!fin.is_open()) {
			cout << "文件读取失败！" << endl;
			return 0;
		}
		fout.open(getCompFileName(argv[1]), ios::binary);
	}


	string tmp_str;
	while (getline((argc > 1 ? fin : cin), tmp_str)) {
		text.emplace_back(tmp_str + "\n");
	}

	buildHCode();

	//压缩后的代码
	vector<string> compText = compressText(text);
	for (auto &s : compText) {
		if (argc > 1)
			fout.write(s.c_str(), s.length());
		else
			cout.write(s.c_str(), s.length());
	}


	fin.close();
	fout.close();
}