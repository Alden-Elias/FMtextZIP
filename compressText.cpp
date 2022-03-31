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


//���������ڵ�
struct HCTNode {
	char ch;
	HCTNode *left;
	HCTNode *right;

	HCTNode(): ch(0), left(nullptr), right(nullptr) {}
	HCTNode(char _ch): ch(_ch), left(nullptr), right(nullptr) {}
	HCTNode(HCTNode *_left, HCTNode *_right): ch(0), left(_left), right(_right) {}
	HCTNode(char _ch, HCTNode *_left, HCTNode *_right): ch(_ch), left(_left), right(_right) {}
};

//˳����������ڵ�
struct HCANode {
	char ch;
	int left;
	int right;
	HCANode(): ch(0), left(-1), right(-1) {}
	HCANode(char _ch): ch(_ch), left(-1), right(-1) {}
	HCANode(int _left, int _right): ch(0), left(_left), right(_right) {}
	HCANode(char _ch, int _left, int _right): ch(_ch), left(_left), right(_right) {}
};



//������������
HCTNode *buildHCodeTree(string &text);
//ͨ����Ƶ������������
HCTNode *getHCodeTree(unordered_map<char, int> &freq);
//ͨ����״�洢����������������������
void getHCode(HCTNode *root, string u, unordered_map<char, string> &HCode);
//ͨ������������ת���ı�
string compressText(string &text, HCTNode *root);
//���ѹ���ı�
string getCompressText(string &text);
//���ѹ���ı����ļ���
string getCompFileName(string fileName);
//��ý�ѹ���ı����ļ���
string getDecompFileName(string fileName);
//ͨ��˳�����������ѹѹ�����ı�
string getOrgText(string &text);
//��int��λ�ָ�Ϊ��λ��string
string num2str(int num);
//����λ�ַ���ת��Ϊint
int str2num(string s);
//����������ת��Ϊ�ַ���
string HTree2str(HCTNode *root);
//����������ת��Ϊ������ʽ
void HTree2HArry(HCTNode *root, vector<HCANode *> &HArry);
//��˳���������ת���ַ���
string HArry2str(vector<HCANode *> &HArry);
//���ַ�����ԭΪ˳���������
vector<HCANode *> str2HArry(string &s);

//ͨ����Կ���ı�����
bool encipher(string &text, string &passwd);
//ͨ����Կ���ı�����
bool dencipher(string &text, string &passwd);
//ͨ���ֽ����õ���ʽ���Ĵ洢��С
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


	//�ļ�����
	fin.open(fileName, ifstream::binary);
	if (!fin.is_open()) {
		cout << "�ļ���ȡʧ�ܣ�" << endl;
		return false;
	}
	fout.open(getCompFileName(fileName), ofstream::binary);


	//��ȡ�ı�
	string text;
	stringstream buffer;
	buffer << fin.rdbuf();
	text = buffer.str();

	//��ȡѹ���ı�
	string compText = getCompressText(text);

	//����ѹ��
	if (passwd != "") {
		if (encipher(compText, passwd)) {
			cout << "Encipher success !" << endl;
		} else {
			cout << "Encipher failed !" << endl;
		}
	}

	//��������
	fout.write(compText.c_str(), compText.length());

	//ѹ���ʼ���
	int orgSize = text.size();
	int cmpSize = compText.size();
	cout << "Original text size:\t" << textSize(orgSize) << endl;
	cout << "Compressed text size:\t" << textSize(cmpSize) << endl;
	cout << "Compression rate:\t" << (cmpSize > orgSize ? '+' : ' ');
	cout << setprecision(4) << (double)( cmpSize - orgSize) / orgSize * 100 << '\%' << endl << endl;

	//�������ر�
	fin.close();
	fout.close();
	return orgSize > cmpSize;
}

bool decompress(string &fileName, string &passwd) {
	ifstream fin;
	ofstream fout;

	//�ļ�����
	fin.open(fileName, ifstream::binary);
	if (!fin.is_open()) {
		cout << "file read error !" << endl;
		return false;
	}
	fout.open(getDecompFileName(fileName), ofstream::binary);


	//��ȡѹ���ı�
	string text;
	stringstream buffer;
	buffer << fin.rdbuf();
	text = buffer.str();

	//����
	if (passwd != "") {
		dencipher(text, passwd);
	}

	string orgText = getOrgText(text);


	//������
	fout.write(orgText.c_str(), orgText.length());


	//��ѹ���
	int orgSize = orgText.size();
	int cmpSize = text.size();
	cout << "Compressed text size:\t" << textSize(cmpSize) << endl;
	cout << "Original text size:\t" << textSize(orgSize) << endl << endl;


	//�������ر�
	fin.close();
	fout.close();
	return true;
}

void help() {
	cout << "* -h\t\t\tGet help" << endl << endl;
	cout << "* -zip filename\t\tCompress" << endl;
	cout << "\t\t\t-filename" << endl;
	cout << "\t\t\t-passwd" << endl << endl;
	cout << "* -unzip filename\tDecompress" << endl;
	cout << "\t\t\t-filename" << endl;
	cout << "\t\t\t-passwd" << endl << endl;
}


//���ѹ���ı�
string getCompressText(string &text) {

	//�����ı�������������
	HCTNode *root = buildHCodeTree(text);


	//˳�򻯹�������
	string HArrystr = HTree2str(root);


	//���ݹ�������ѹ���ı�
	return HArrystr + compressText(text, root);
}

//������������
HCTNode *buildHCodeTree(string &text) {
	unordered_map<char, int> freq;
	for (char ch : text) {
		freq[ch]++;
	}

	return getHCodeTree(freq);
}


//ͨ����Ƶ������������
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


//ͨ����״�洢����������������������
void getHCode(HCTNode *root, string u, unordered_map<char, string> &HCode) {
	if (!root)
		return;
	if (root->ch) {
		HCode[root->ch] = u;
	}
	getHCode(root->left, u + "1", HCode);
	getHCode(root->right, u + "0", HCode);
}


//ͨ������������ת���ı�
string compressText(string &text, HCTNode *root) {

	//���������
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


//���ѹ���ı����ļ���
string getCompFileName(string fileName) {
	int len = fileName.length();
	long long idx = len - 1;

	while (idx >= 0 && fileName[idx] != '.')
		idx--;
	if (!idx)
		return fileName + ".cod";

	return fileName.substr(0, idx) + ".cod";
}


//��ý�ѹ���ı����ļ���
string getDecompFileName(string fileName) {
	int len = fileName.length();
	long long idx = len - 1;

	while (idx >= 0 && fileName[idx] != '.')
		idx--;
	if (!idx)
		return fileName + ".txt";

	return fileName.substr(0, idx) + ".txt";
}


//ͨ��˳�����������ѹѹ�����ı�
string getOrgText(string &text) {

	//��ѹ���ļ�������ָ����
	int len_HArry = str2num(text.substr(0, 4));
	string HArrystr = text.substr(4, len_HArry * 9);
	string compText = text.substr(4 + len_HArry * 9);


	//ͨ��ѹ���ļ��е���Ϣ�ָ��ı�
	vector<HCANode *> orgHArry = str2HArry(HArrystr);

	//��ԭԭ�ı�����
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


//��int��λ�ָ�Ϊ��λ��string
string num2str(int num) {
	string ans(4, 0);
	char ans_t[4];
	memcpy(ans_t, &num, 4);
	for (int i = 0; i < 4; i++)
		ans[i] = ans_t[i];
	return ans;
}


//����λ�ַ���ת��Ϊint
int str2num(string s) {
	int ans = 0;
	memcpy(&ans, s.c_str(), 4);
	return ans;
}


//����������ת��Ϊ�ַ���
string HTree2str(HCTNode *root) {
	vector<HCANode *>  HArry;
	HTree2HArry(root, HArry);
	return HArry2str(HArry);
}

//����������ת��Ϊ������ʽ
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

//��˳���������ת���ַ���
string HArry2str(vector<HCANode *> &HArry) {
	string ans = num2str(HArry.size());
	for (auto &node : HArry) {
		ans.push_back(node->ch);
		ans += num2str(node->left) + num2str(node->right);
	}
	return ans;
}

//���ַ�����ԭΪ˳���������
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

//ͨ����Կ���ı�����
bool encipher(string &text, string &passwd) {
	int len1 = text.size(), len2 = passwd.size();
	if (len2 > len1)
		return false;

	for (int i = 0; i < len2; i++) {
		text[i] ^= passwd[i];
	}

	return true;
}


//ͨ����Կ���ı�����
bool dencipher(string &text, string &passwd) {
	return encipher(text, passwd);
}


//ͨ���ֽ����õ���ʽ���Ĵ洢��С
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