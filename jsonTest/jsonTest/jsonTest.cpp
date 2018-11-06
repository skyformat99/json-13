/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 �콭��Ĵ�Ϻ.
 * All rights reserved.
 *
 * \file jsonTest.cpp
 * \author �콭��Ĵ�Ϻ
 * \date ʮһ�� 2018
 *
 * 
 *
 */

#include "stdafx.h"
#include <iostream>
#include <string>
#include <daxia/encode/json.hpp>

using namespace std;
using namespace daxia::reflect;
using namespace daxia::encode;

struct peple
{
	// ������Ҫjson���л����ֶ���Ҫ��reflectģ������а�װ
	reflect<int> age;	
	reflect<string> name;
	peple()
		: age("json:age")
		, name("json:name")
	{

	}
};

struct family
{
	// û��reflectģ������а�װ���ֶβ��������л�
	int id;
	reflect<string> addr;
	reflect<peple> father;
	reflect<peple> mother;
	reflect<vector<peple>> children;

	// reflectģ����Ĺ��캯����Ҫ����string���͵ı�ǩ��
	// ��ǩ������Ϊ�Ǹñ�������չ��Ϣ�������ֶ�ӳ�䡢�����ϵӳ��ȡ�
	// ��ǩ��ʽΪ"xxx:xxx"
	// ��json��ָ���ı�ǩ��ʽΪ"json:xxx"��orm��ָ���ı�ǩ��ʽΪ"orm:xxx"
	family()
		: addr("json:addr orm:address")
		, father("json:father")
		, mother("json:mother")
		, children("json:children")
	{

	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	// �����������
	// reflect�����ز�����=������ֱ��ʹ��=��ֵ
	peple father;father.age = 40;father.name = "����";
	peple mother;mother.age = 35;mother.name = "������";
	peple child1;child1.age = 3;child1.name = "������";
	peple child2;child2.age = 4;child2.name = "������";

	// �����������
	family myfamily;
	myfamily.id = 0;
	myfamily.addr = "������й�";
	myfamily.father = father;
	myfamily.mother = mother;

	// ��ȡreflect��װ��������Ҫʹ�����ز�����()
	myfamily.children().push_back(child1);
	myfamily.children().push_back(child2); 
	
	// ����json����
	cout << "-------------json����:--------------" << endl;
	string jsonStr = json::Marshal(myfamily, true);
	cout << jsonStr << endl;

	// ����json����
	family myfamily2;
	json::Unmarshal(jsonStr, myfamily2);
	
	// ���Դ�ӡjson������
	cout << "-------------json����:--------------" << endl;
	cout << "id:" << myfamily2.id 
		<< "\t// ���id��δ��ʼ���ģ���Ϊû��reflect��װ��������json�����" << endl;
	cout << "addr:" << myfamily2.addr() << endl;
	cout << "father:" << endl;
	cout << "\tage:" << myfamily2.father().age() << endl;
	cout << "\tname:" << myfamily2.father().name() << endl;
	cout << "mother:" << endl;
	cout << "\tage:" << myfamily2.mother().age() << endl;
	cout << "\tname:" << myfamily2.mother().name() << endl;
	cout << "children:" << endl;
	for (auto iter = myfamily2.children().begin(); iter != myfamily2.children().end(); ++iter)
	{
		cout << "\tage:" << iter->age() << endl;
		cout << "\tname:" << iter->name() << endl;
		cout << "\t---------------" << endl;
	}

	return 0;
}

