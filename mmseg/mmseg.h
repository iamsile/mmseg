//
//  mmseg.h
//  mmseg
//
//  Created by taowei on 13-11-27.
//  Copyright (c) 2013年 taowei. All rights reserved.
//

#ifndef __mmseg__mmseg__
#define __mmseg__mmseg__

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

map<string, int> freq; //frequence
size_t chineseLength;

class Word {
public:
	Word() {

	}
	void setValue(string src) {
		value = src;
		/* 单个字符需要找到他的频率，默认值是1 */
		if (value.length() == chineseLength) {
			map<string, int>::iterator it = freq.find(value);
			if (it == freq.end())
				frequence = 1;
			else
				frequence = freq[value];
		} else
			frequence = 1;

	}
	string getValue() {
		return value;
	}
	int getLength() {
		return value.length();
	}
	int getFrequence() {
		return frequence;
	}
private:
	string value;
	int frequence;
};

class Chunk {
public:
	vector<Word> list;
	Chunk() {

	}
	/* 添加一个词到list里来 */
	void addWord(Word w) {
		list.push_back(w);
	}
	int getWordNumber() {
		return list.size();
	}
	double getVariance() {
		double avgLen = 0.0;
		int listSize = list.size();
		for (int i = 0; i < listSize; i++)
			avgLen += list.at(i).getLength();
		avgLen = 1.0 * avgLen / listSize;
		double variance = 1.0;
		for (int i = 0; i < listSize; i++) {
			double temp = (avgLen - list.at(i).getLength());
			variance += temp * temp;
		}
		return variance;
	}
	long getFrequence() {
		long freqValue = 0;
		int listSize = list.size();
		for (int i = 0; i < listSize; i++)
			freqValue += list.at(i).getFrequence();
		return freqValue;
	}
	vector<string> getVectorString() {
		vector<string> res;
		int size = list.size();
		for (int i = 0; i < size; i++)
			res.push_back(list.at(i).getValue());
		return res;
	}
};

#endif /* defined(__mmseg__mmseg__) */
