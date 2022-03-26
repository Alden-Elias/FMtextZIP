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
//	priority_queue < pair<int, HCTNode *>, vector<pair<int, HCTNode *>>, decltype([](const auto & a, const auto & b) {
//		if (a.first != b.first)
//			return a.first < b.first;
//		return (a.second->ch) < (b.second->ch);
//	} ) > q;
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
void getHCode(HCTNode *root, string u, unordered_map<char, string> &HCode) {
	if (!root)
		return;
	if (root->ch) {
		HCode[root->ch] = u;
	}
	getHCode(root->left, u + "1", HCode);
	getHCode(root->right, u + "0", HCode);
}


//总体上构建哈夫曼编码
void buildHCode(string &text) {
	unordered_map<char, int> freq = getFreq(text);
	HCTNode *root = getHCodeTree(freq);

	unordered_map<char, string> HCode;
	getHCode(root, "", HCode);
}

//通过哈夫曼编码转化文本
string compressText(string &text, unordered_map<char, string> &HCode) {
	string comp;
	int idx = 0;
	char comp_t = 0;

	for (char ch : text) {
		for (char t : HCode[ch]) {
			comp_t |= (t == '1' ? 1 << (idx & 7) : 0);
			if (((idx++) & 7) == 7) {
				comp.push_back(comp_t);
				comp_t = 0;
			}
		}
	}

	if (idx % 8)
		comp.push_back(comp_t);


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

//解压压缩的文本
string getOrgText(string &compText, HCTNode *root) {
	string orgText;
	long long idx = 0;
	int len = compText.length();

	HCTNode *tmp = root;
	for (long long idx = 0; (idx / 8) < len; idx++) {
		tmp = (compText[idx / 8] & (1 << (idx & 7)) ? tmp->left : tmp->right);

		if (tmp->ch) {
			orgText.push_back(tmp->ch);
			tmp = root;
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

//将当前词频信息转换为string
string freq2str(unordered_map<char, int> &freq) {
	string ans;
	ans.push_back((char)(freq.size()));
	for (auto &t : freq) {
		ans.push_back(t.first);
		ans += num2str(t.second);
	}
	return ans;
}


//将词频字符串转换为词频表
unordered_map<char, int> str2freq(string &s) {
	int len = s.length();
	unordered_map<char, int> ans;
	if (!len || len % 5)
		return ans;

	for (int i = 0; i < len; i += 5) {
		ans.emplace(s[i], str2num(s.substr(i + 1, 4)));
	}

	return ans;
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

	string text;
	if (argc > 1) {
		stringstream buffer;
		buffer << fin.rdbuf();
		text = buffer.str();
	} else {
		string tmp_str;
		while (getline(cin, tmp_str)) {
			text += (tmp_str + "\n");
		}
	}

//	buildHCode();

	unordered_map<char, int> freq = getFreq(text);
	HCTNode *root = getHCodeTree(freq);
	unordered_map<char, string> HCode;
	getHCode(root, "", HCode);



	//压缩后的文本
	string compText = compressText(text, HCode);
	if (argc > 1)
		fout.write(compText.c_str(), compText.length());
	else
		cout.write(compText.c_str(), compText.length());


	//通过哈夫曼树还原文本
	cout << getOrgText(compText, root) << endl;
	string freqStr = freq2str(freq).substr(1);


	unordered_map<char, int> m = str2freq(freqStr);
	HCTNode *orgRoot = getHCodeTree(m);
	unordered_map<char, string> orgHCode;
	getHCode(orgRoot, "", orgHCode);


	cout << "*********************" << endl;
	for (auto &t : HCode) {
		cout << t.first << " " << t.second << endl;
	}
	cout << "*********************" << endl;
	for (auto &t : orgHCode) {
		cout << t.first << " " << t.second << endl;
	}
	cout << "*********************" << endl;

//	cout << "*********************" << endl;
//	for (auto &t : freq_char) {
//		cout << t.first << " " << t.second << endl;
//	}
//	cout << "*********************" << endl;
//	for (auto &t : m) {
//		cout << t.first << " " << t.second << endl;
//	}
//	cout << "*********************" << endl;

	fin.close();
	fout.close();
}