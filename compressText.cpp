#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
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



//构建哈夫曼树
HCTNode *buildHCodeTree(string &text);
//通过词频构建哈夫曼树
HCTNode *getHCodeTree(unordered_map<char, int> &freq);
//通过树状存储哈夫曼树构建哈夫曼编码
void getHCode(HCTNode *root, string u, unordered_map<char, string> &HCode);
//通过哈夫曼编码转化文本
string compressText(string &text, HCTNode *root);
//获得压缩文本
string getCompressText(string &text);
//获得压缩文本的文件名
string getCompFileName(string fileName);
//获得解压缩文本的文件名
string getDecompFileName(string fileName);
//通过顺序哈夫曼树解压压缩的文本
string getOrgText(string &text);
//将int按位分割为四位的string
string num2str(int num);
//将四位字符串转换为int
int str2num(string s);
//将哈夫曼树转换为字符串
string HTree2str(HCTNode *root);
//将哈夫曼树转换为数组形式
void HTree2HArry(HCTNode *root, vector<HCANode *> &HArry);
//将顺序哈夫曼树转换字符串
string HArry2str(vector<HCANode *> &HArry);
//将字符串还原为顺序哈夫曼树
vector<HCANode *> str2HArry(string &s);

//通过秘钥对文本解密
bool encipher(string &text, string &passwd);
//通过秘钥对文本解密
bool dencipher(string &text, string &passwd);
//通过字节数得到格式化的存储大小
string textSize(int size);


bool compress(string &fileName, string &passwd);
bool decompress(string &fileName, string &passwd);
void help();


int main(int argc, const char *argv[]) {
	string parameter = (argc > 1 ? argv[1] : "");
	string fileName = (argc > 2 ? argv[2] : "");
	string passwd = (argc > 3 ? argv[3] : "");

	if (argc <= 1) {
		cout << "Please enter parameters, enter '-h' for help." << endl;
	} else if (parameter == "-h") {
		help();
	} else if (parameter == "-zip" && argc >= 3) {
		if (!compress(fileName, passwd))
			cout << "Compress failed !" << endl;
		else
			cout << "Compress success !" << endl;
	} else if (parameter == "-unzip" && argc >= 3) {
		if (!decompress(fileName, passwd))
			cout << "Decompress failed !" << endl;
		else
			cout << "Decompress success !" << endl;
	} else {
		cout << "Incorrect parameter, enter '-h' for help." << endl;
	}
//	compress("./text.txt");
//	decompress("./text.cod");
	return 0;
}


bool compress(string &fileName, string &passwd) {
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

	//获取压缩文本
	string compText = getCompressText(text);

	//加密压缩
	if (passwd != "") {
		if (encipher(compText, passwd)) {
			cout << "Encipher success !" << endl;
		} else {
			cout << "Encipher failed !" << endl;
		}
	}

	//将结果输出
	fout.write(compText.c_str(), compText.length());

	//压缩率计算
	int orgSize = text.size();
	int cmpSize = compText.size();
	cout << "Original text size:\t" << textSize(orgSize) << endl;
	cout << "Compressed text size:\t" << textSize(cmpSize) << endl;
	cout << "Compression rate:\t" << (cmpSize > orgSize ? '+' : ' ');
	cout << setprecision(4) << (double)( cmpSize - orgSize) / orgSize * 100 << '\%' << endl << endl;

	//数据流关闭
	fin.close();
	fout.close();
	return orgSize > cmpSize;
}

bool decompress(string &fileName, string &passwd) {
	ifstream fin;
	ofstream fout;

	//文件输入
	fin.open(fileName, ifstream::binary);
	if (!fin.is_open()) {
		cout << "file read error !" << endl;
		return false;
	}
	fout.open(getDecompFileName(fileName), ofstream::binary);


	//读取压缩文本
	string text;
	stringstream buffer;
	buffer << fin.rdbuf();
	text = buffer.str();

	//解密
	if (passwd != "") {
		dencipher(text, passwd);
	}

	string orgText = getOrgText(text);


	//结果输出
	fout.write(orgText.c_str(), orgText.length());


	//解压结果
	int orgSize = orgText.size();
	int cmpSize = text.size();
	cout << "Compressed text size:\t" << textSize(cmpSize) << endl;
	cout << "Original text size:\t" << textSize(orgSize) << endl << endl;


	//数据流关闭
	fin.close();
	fout.close();
	return true;
}

void help() {
	cout << "* -h\t\t\tGet help" << endl << endl;
	cout << "* -zip\t\t\tCompress" << endl;
	cout << "\t\t\tfilename" << endl;
	cout << "\t\t\tpasswd" << endl << endl;
	cout << "* -unzip\t\tDecompress" << endl;
	cout << "\t\t\tfilename" << endl;
	cout << "\t\t\tpasswd" << endl << endl;
}


//获得压缩文本
string getCompressText(string &text) {

	//根据文本构建哈夫曼树
	HCTNode *root = buildHCodeTree(text);


	//顺序化哈夫曼树
	string HArrystr = HTree2str(root);


	//根据哈夫曼树压缩文本
	return HArrystr + compressText(text, root);
}

//构建哈夫曼树
HCTNode *buildHCodeTree(string &text) {
	unordered_map<char, int> freq;
	for (char ch : text) {
		freq[ch]++;
	}

	return getHCodeTree(freq);
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


//通过哈夫曼编码转化文本
string compressText(string &text, HCTNode *root) {

	//构建编码表
	unordered_map<char, string> HCode;
	getHCode(root, "", HCode);

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
	return fileName + ".cod";
}


//获得解压缩文本的文件名
string getDecompFileName(string fileName) {
	int len = fileName.size();
	if (len < 4 || fileName.substr(len - 4, 4) != ".cod")
		return fileName + ".txt";
	return fileName.substr(0, len - 4);
}


//通过顺序哈夫曼树解压压缩的文本
string getOrgText(string &text) {

	//将压缩文件各组件分割出来
	int len_HArry = str2num(text.substr(0, 4));
	string HArrystr = text.substr(4, len_HArry * 9);
	string compText = text.substr(4 + len_HArry * 9);


	//通过压缩文件中的信息恢复文本
	vector<HCANode *> orgHArry = str2HArry(HArrystr);

	//还原原文本内容
	string orgText;
	int redun = compText.back() + 8;
	long long len = (long long)(compText.length()) * 8 - redun;

	HCANode *tmp = orgHArry[0];
	for (long long idx = 0; idx < len; idx++) {
		tmp = ((compText[idx / 8] & (1 << (idx % 8))) ? orgHArry[tmp->left] : orgHArry[tmp->right]);

		if (tmp->ch) {
			orgText.push_back(tmp->ch);
			tmp = orgHArry[0];
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


//将哈夫曼树转换为字符串
string HTree2str(HCTNode *root) {
	vector<HCANode *>  HArry;
	HTree2HArry(root, HArry);
	return HArry2str(HArry);
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

//通过秘钥对文本加密
bool encipher(string &text, string &passwd) {
	int len1 = text.size(), len2 = passwd.size();
	if (len2 > len1)
		return false;

	for (int i = 0; i < len2; i++) {
		text[i] ^= passwd[i];
	}

	return true;
}


//通过秘钥对文本解密
bool dencipher(string &text, string &passwd) {
	return encipher(text, passwd);
}


//通过字节数得到格式化的存储大小
string textSize(int size) {
	double ans = size;

	const string CompSize[4] = {"B", "KB", "MB", "GB"};
	int idx = 0;
	while (ans > 1024) {
		ans /= 1024;
		idx++;
	}

	stringstream ss;
	ss << setprecision(5) << ans;

	return ss.str() + CompSize[idx];
}