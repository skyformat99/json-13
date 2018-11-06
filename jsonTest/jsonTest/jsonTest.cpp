/*!
 * Licensed under the MIT License.See License for details.
 * Copyright (c) 2018 漓江里的大虾.
 * All rights reserved.
 *
 * \file jsonTest.cpp
 * \author 漓江里的大虾
 * \date 十一月 2018
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
	// 所有需要json序列化的字段需要用reflect模板类进行包装
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
	// 没用reflect模板类进行包装的字段不参与序列化
	int id;
	reflect<string> addr;
	reflect<peple> father;
	reflect<peple> mother;
	reflect<vector<peple>> children;

	// reflect模板类的构造函数需要传入string类型的标签。
	// 标签可以认为是该变量的扩展信息，用来字段映射、对象关系映射等。
	// 标签格式为"xxx:xxx"
	// 如json库指定的标签格式为"json:xxx"，orm库指定的标签格式为"orm:xxx"
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
	// 构造测试数据
	// reflect已重载操作符=，可以直接使用=赋值
	peple father;father.age = 40;father.name = "王大锤";
	peple mother;mother.age = 35;mother.name = "柳青青";
	peple child1;child1.age = 3;child1.name = "王旺财";
	peple child2;child2.age = 4;child2.name = "王来福";

	// 构造测试数据
	family myfamily;
	myfamily.id = 0;
	myfamily.addr = "地球村中国";
	myfamily.father = father;
	myfamily.mother = mother;

	// 获取reflect包装的数据需要使用重载操作符()
	myfamily.children().push_back(child1);
	myfamily.children().push_back(child2); 
	
	// 进行json编码
	cout << "-------------json编码:--------------" << endl;
	string jsonStr = json::Marshal(myfamily, true);
	cout << jsonStr << endl;

	// 进行json解码
	family myfamily2;
	json::Unmarshal(jsonStr, myfamily2);
	
	// 测试打印json解码结果
	cout << "-------------json解码:--------------" << endl;
	cout << "id:" << myfamily2.id 
		<< "\t// 这个id是未初始化的，因为没用reflect包装，不参与json编解码" << endl;
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

