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



//ͨ���ı���ȡ��Ƶ
unordered_map<char, int> getFreq(string &text) {
	unordered_map<char, int> freq;
	for (char ch : text) {
		freq[ch]++;
	}
	return freq;
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

//ͨ��˳��洢����������������������
void getHCode(vector<HCANode *> &arry, int idx, string u, unordered_map<char, string> &HCode) {
	if (idx == -1)
		return;
	if (arry[idx]->ch) {
		HCode[arry[idx]->ch] = u;
	}

	getHCode(arry, arry[idx]->left, u + "1", HCode);
	getHCode(arry, arry[idx]->right, u + "0", HCode);
}


//�����Ϲ�������������
void buildHCode(string &text) {
	unordered_map<char, int> freq = getFreq(text);
	HCTNode *root = getHCodeTree(freq);

	unordered_map<char, string> HCode;
	getHCode(root, "", HCode);
}

//ͨ������������ת���ı�
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

//ͨ����״����������ѹѹ�����ı�
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

//ͨ��˳�����������ѹѹ�����ı�
string getOrgText(string &compText, vector<HCANode *> HArry) {
	string orgText;
	long long idx = 0;
	int len = compText.length();

	HCANode *tmp = HArry[0];
	for (long long idx = 0; (idx / 8) < len; idx++) {
		tmp = (compText[idx / 8] & (1 << (idx & 7)) ? HArry[tmp->left] : HArry[tmp->right]);

		if (tmp->ch) {
			orgText.push_back(tmp->ch);
			tmp = HArry[0];
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

bool compress(string fileName) {
	ifstream fin;
	ofstream fout;


	//�ļ�����
	fin.open(fileName);
	if (!fin.is_open()) {
		cout << "�ļ���ȡʧ�ܣ�" << endl;
		return false;
	}
	fout.open(getCompFileName(fileName));


	//��ȡ�ı�
	string text;
	stringstream buffer;
	buffer << fin.rdbuf();
	text = buffer.str();


	//���ݴ�Ƶ���������
	unordered_map<char, int> freq = getFreq(text);
	HCTNode *root = getHCodeTree(freq);
	unordered_map<char, string> HCode;
	getHCode(root, "", HCode);


	//��˳�򻯹�������
	vector<HCANode *>  HArry;
	HTree2HArry(root, HArry);
	string HArrystr = HArry2str(HArry);


	//���ݱ����ѹ���ı����������������Ϊѹ�����
	string compText = HArrystr + compressText(text, HCode);


	//��������
	fout.write(compText.c_str(), compText.length());


	//�������ر�
	fin.close();
	fout.close();
	return true;
}

bool decompress(string fileName) {
	ifstream fin;
	ofstream fout;

	//�ļ�����
	fin.open(fileName);
	if (!fin.is_open()) {
		cout << "�ļ���ȡʧ�ܣ�" << endl;
		return false;
	}
	fout.open(getDecompFileName(fileName));


	//��ȡѹ���ı�
	string text;
	stringstream buffer;
	buffer << fin.rdbuf();
	text = buffer.str();


	//��ѹ���ļ�������ָ����
	int len = str2num(text.substr(0, 4));
	string HArrystr = text.substr(4, len);
	string compText = text.substr(4 + len);


	//ͨ��ѹ���ļ��е���Ϣ�ָ��ı�
	vector<HCANode *> orgHArry = str2HArry(HArrystr);
	string orgText = getOrgText(compText, orgHArry);


	//������
	fout.write(compText.c_str(), compText.length());


	//�������ر�
	fin.close();
	fout.close();
	return true;
}

void help() {
	cout << "* -h\t\t\t����" << endl;
	cout << "* -zip filename\t\tѹ��" << endl;
	cout << "* -unzip filename\t��ѹ" << endl;
}

int main(int argc, const char *argv[]) {
	if (argc <= 1) {
		cout << "���������������'-h'��ð���" << endl;
	} else if (!strcmp(argv[1], "-h")) {
		help();
	} else if (!strcmp(argv[1], "-zip") && argc >= 3) {
		if (!compress(argv[2]))
			cout << "ѹ��ʧ��" << endl;
		else
			cout << "ѹ���ɹ�" << endl;
	} else if (!strcmp(argv[1], "-unzip") && argc >= 3) {
		if (!decompress(argv[2]))
			cout << "��ѹʧ��" << endl;
		else
			cout << "��ѹ�ɹ�" << endl;
	} else {
		cout << "������������'-h'��ð���" << endl;
	}

	return 0;
}