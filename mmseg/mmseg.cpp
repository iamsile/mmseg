//
//  mmseg.cpp
//  mmseg
//
//  Created by taowei on 13-11-27.
//  Copyright (c) 2013年 taowei. All rights reserved.
//

#include "mmseg.h"
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <time.h>
#include <stdio.h>
using namespace std;

map<string, set<string> > content;

void init() {
	string test = "测试";
	chineseLength = test.length()/2;
}

void read_terms_from_Dic() {
	ifstream fcin("/Users/taowei/Documents/工程/mmseg/mmseg/dic.txt");
	string dot = "·";
	string s,temp;
	map<string, set<string> >::iterator DicIterator;
	s.clear();
	//这里有点小问题，总是忽略第一个字符，待排查
	fcin >> s;
	cout << "从词典里找单词" << endl;
	cout << "term is: \n";

	while (fcin >> s) {
		set<string> tempSet;
		temp.clear();
		temp = s.substr(0, chineseLength);
		DicIterator = content.find(temp);
		if (DicIterator == content.end()) {
			tempSet.clear();
			tempSet.insert(s);
		} else {
			tempSet = content[temp];
			tempSet.insert(s);
		}
		content[temp] = tempSet;
	}
	cout << "从字典里读取完毕" << endl;
	cout << "从字典里读取的大小是: " << content.size() << endl;

	fcin.close();
}

void show_Dic() {
	map<string, set<string> >::iterator DicIterator;
	int count = 0;
	for (DicIterator = content.begin(); DicIterator != content.end(); DicIterator++) {
		string first = DicIterator->first;
		set<string> second = DicIterator->second;
		cout << first << ":";
		set<string>::iterator setIt;
		for (setIt = second.begin(); setIt != second.end(); setIt++)
			cout << *setIt << " ";
		cout << endl;
		count++;
		if (count == 10)
			break;
	}
	cout << "Dic size: " << content.size() << endl;
}

void write_index() {
	fstream outputFile("/Users/taowei/Documents/工程/mmseg/mmseg/freq.index", fstream::out);
	map<string, set<string> >::iterator DicIterator;
	for (DicIterator = content.begin(); DicIterator != content.end(); DicIterator++) {
		string first = DicIterator->first;
		set<string> second = DicIterator->second;
		outputFile << "#:" << first << endl;
		set<string>::iterator setIt;
		for (setIt = second.begin(); setIt != second.end(); setIt++)
			outputFile << *setIt << " ";
		outputFile << endl;
	}
	cout << "write index: Dic size:" << content.size() << endl;
	outputFile.close();
}

void write_frequence() {
	fstream outputFile("/Users/taowei/Documents/工程/mmseg/mmseg/freq.index", fstream::out);
	map<string, int>::iterator freqIterator;
	for (freqIterator = freq.begin(); freqIterator != freq.end(); freqIterator++) {
		string first = freqIterator->first;
		int second = freqIterator->second;
		outputFile << first << " " << second;
		outputFile << endl;
	}
	cout << "write index: freq size:" << freq.size() << endl;
	outputFile.close();
}

void build_frequence() {
	ifstream fcin("/Users/taowei/Documents/工程/mmseg/mmseg/freq.index");
	string tempStr;
	int tempFreq;
	while (fcin >> tempStr) {
		fcin >> tempFreq;
		if (tempFreq > 1 || tempStr.length() == chineseLength)
			freq[tempStr] = tempFreq;
	}
	fcin.close();
	cout << "index: freq size:" << freq.size() << endl;
}

void build_index() {
	ifstream fcin("/Users/taowei/Documents/工程/mmseg/mmseg/freq.index");
	if (fcin.good() == 0) {
		cout << "build index, need some time, please wait for a moment!\n";
		read_terms_from_Dic();
		write_index();
		fcin.open("/Users/taowei/Documents/工程/mmseg/mmseg/freq.index", ifstream::in);
	}
	cout << "hello, begin load index\n";
	string tempStr;
	set<string> tempSet;
	string key = "";
	while (fcin >> tempStr) {
		if (tempStr.find("#", 0) == 0) {
			if (key != "")
				content[key] = tempSet;
			tempSet.clear();
			key = tempStr.substr(2);
		} else
			tempSet.insert(tempStr);
	}
	content[key] = tempSet;
	cout << "Dic size:" << content.size() << endl;
	fcin.close();
	build_frequence();
}

vector<Chunk> chunklist;
int minChunkWordNumber;

void mmseg_recursion(string src, Chunk tempChunk) {
	set<string> termList;
	int i = 0;
	int len = src.length();
	/* 获得单个字符 */
	string singleWordStr;
	Word tempWord;
	singleWordStr = src.substr(0, chineseLength);
	tempWord.setValue(singleWordStr);
	/* 如果是末尾的字符，就直接返回 */
	if ((i + chineseLength) >= len) {
		tempChunk.addWord(tempWord);
		chunklist.push_back(tempChunk);
		if (minChunkWordNumber > tempChunk.getWordNumber())
			minChunkWordNumber = tempChunk.getWordNumber();
		return ;
	} else {
		string tempStr;
		map<string, set<string> >::iterator DicIt;
		DicIt = content.find(singleWordStr);
		/* 如果字典里没有就让它称为单独的一个 */
		if (DicIt == content.end()) {
			tempChunk.addWord(tempWord);
			string remain = src.substr(chineseLength);
			mmseg_recursion(remain, tempChunk);
			tempChunk.list.pop_back();
		} else {
			termList = content[singleWordStr];
			set<string>::iterator setIt;
			vector<string> termVector;
			//for (setIt = termList.begin(); setIt != termList.end(); setIt++)
			//	termsVector.push_back(*setIt);
			//sort(termsVector.begin(), termsVector.end());
			//int sizeVec = termsVector.size();
			for (setIt = termList.begin(); setIt != termList.end(); setIt++) {
				tempStr = *setIt;
				size_t foundit = src.find(tempStr, 0);
				if (foundit == 0 && tempStr != singleWordStr) {
					tempWord.setValue(tempStr);
					tempChunk.addWord(tempWord);
					if (tempChunk.getWordNumber() > minChunkWordNumber) {
						tempChunk.list.pop_back();
						return ;
					}

					/* 如果term里有所有的字符，则直接返回 */
					if (tempStr.length() == src.length()) {
						chunklist.push_back(tempChunk);
						if (minChunkWordNumber > tempChunk.getWordNumber())
							minChunkWordNumber = tempChunk.getWordNumber();
						return ;
					}
					string remain = src.substr(tempStr.length());
					mmseg_recursion(remain, tempChunk);
					tempChunk.list.pop_back();
				}
			}
			/* 继续进行单个字符的状态 */
			tempStr = singleWordStr;
			tempWord.setValue(tempStr);
			tempChunk.addWord(tempWord);
			if (tempChunk.getWordNumber() > minChunkWordNumber) {
				tempChunk.list.pop_back();
				return ;
			}
			string remain = src.substr(tempStr.length());
			mmseg_recursion(remain, tempChunk);
			tempChunk.list.pop_back();
		}
	}
}

vector<string> mmseg(string src) {
	vector<string> res;
	chunklist.clear();
	minChunkWordNumber = 0x7ffffff0;
	Chunk tempChunk;
	vector<int> indexInChunkList;
	int min = 0x7fffffff;

	mmseg_recursion(src, tempChunk);
	int chunkListSize = chunklist.size();
	if (chunkListSize == 1)
		return chunklist.at(0).getVectorString();
	else {
		for (int i = 0; i < chunkListSize; i++) {
			if (chunklist.at(i).getWordNumber() < min) {
				min = chunklist.at(i).getWordNumber();
				indexInChunkList.clear();
				indexInChunkList.push_back(i);
			} else if (chunklist.at(i).getWordNumber() == min)
				indexInChunkList.push_back(i);
		}
		/* 查找最大平俊距离的chunk */
		if (indexInChunkList.size() == 1)
			return chunklist.at(indexInChunkList.at(0)).getVectorString();
		else {
			/* 找最大variance的chunk */
			double minVariance = min * src.length() * src.length();
			vector<int> tempIndex = indexInChunkList;
			indexInChunkList.clear();
			for (size_t index = 0; index < tempIndex.size(); index++) {
				int i = tempIndex.at(index);
				if (chunklist.at(i).getVariance() < minVariance) {
					minVariance = chunklist.at(i).getVariance();
					indexInChunkList.clear();
					indexInChunkList.push_back(i);
				} else if (chunklist.at(i).getVariance() == minVariance)
					indexInChunkList.push_back(i);
			}
		}

		if (indexInChunkList.size() == 1)
			return chunklist.at(indexInChunkList.at(0)).getVectorString();
		else {
			/* 找到概率最大的term */
			vector<int> tempIndex = indexInChunkList;
			indexInChunkList.clear();
			long max = 0;
			int tempIndexSize = tempIndex.size();
			for (int index = 0; index < tempIndexSize; index++) {
				int i = tempIndex.at(index);
				if (chunklist.at(i).getFrequence() > max) {
					max = chunklist.at(i).getFrequence();
					indexInChunkList.clear();
					indexInChunkList.push_back(i);
				} else if (chunklist.at(i).getFrequence() == max)
					indexInChunkList.push_back(i);
			}
			return chunklist.at(indexInChunkList.at(0)).getVectorString();
		}
	}
}

void showTermSegment(vector<string> src) {
	cout << "结果如下:";
	int size = src.size();
	for (int i = 0; i < size; i++)
		cout << src.at(i) << " ";
	cout << endl;
}