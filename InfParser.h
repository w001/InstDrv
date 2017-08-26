// InfParser.h: interface for the CInfParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFPARSER_H__C2C032E4_255F_4BE6_A16F_1A1FA9AC20C3__INCLUDED_)
#define AFX_INFPARSER_H__C2C032E4_255F_4BE6_A16F_1A1FA9AC20C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <vector>
#include <iostream>
#include <string>
using namespace std;

class CInfParser  
{
public:
	CInfParser();
	virtual ~CInfParser();

public:
	//打开文件
	BOOL Open(LPCTSTR lpFileName);
	//关闭文件
	void Close();
	//解析文件,在调用其他函数进行操作之前，必须先调用此函数解析文件
	BOOL Parse();
	//判断是否解析完成,只有在解析完后才能进行后续操作
	BOOL IsParseComplete();
	//获取节名
	CString GetSectionNameFromIndex(UINT nSecIndex);
	//读取节名下的所有Field
	//e.g: [Section]
	//	   Field1=Value1
	//	   Field2=Value2
	//return: Field1, Field2
	vector<CString> GetFiledFromSection(CString strSectionName);
	//由Field读取值
	//e.g: [Section]
	//	   Field1=Value1
	//return: Value1
	CString GetValueStringFromField(CString strFieldName);
	//取得文件共有多少节
	UINT GetSectionCount();
	//根据某个字符分割字符串，只进行一次分割，分成2部分
	BOOL SplitStrByCh(CString strSrc, TCHAR ch, vector<CString>& arOut);
	//根据某个字符分割字符串，进行循环分割
	void SplitStrByChEx(CString strSrc, TCHAR ch, vector<CString>& arOut);
	//修改启动方式
	void SetValueToStartTypeField(UINT nStartType = 3);

private:
	//去掉前后空格
	void TrimAll(CString& str);
	//去掉前后的某个匹配字符
	void TrimChar(CString& str, TCHAR ch);
	//是否是空行
	BOOL IsNewLine(CString str);
	//是否是注释行
	BOOL IsComment(CString str);
	//获取节名
	void GetSectionName(CString& strSection);
	//重置数据
	void Reset();

private:
	CStdioFile m_stdioFile;
	//文件是否打开
	BOOL m_bOpenFile;
	//节数
	UINT m_nSection;
	//节名集合
	CArray<CString, LPCTSTR> m_pSectNameArray;
	//域对应的值
	CMap<CString, LPCTSTR, CString, LPCTSTR> m_pFieldMapValue;
	//域名集合
	CMap<CString, LPCTSTR, vector<CString>, vector<CString>&> m_FieldMapSect;
	//文件是否解析完成
	BOOL m_bParseComplete;
	//驱动启动类型在文件中的位置
	DWORD m_nStartTypePos;
	//当前文件所在位置
	DWORD m_nCurFilePos;
	//当前行的前一行位置指针
	DWORD m_nPrevLinePos;
	//Inf文件路径
	CString m_pStrInfFileName;
};

#endif // !defined(AFX_INFPARSER_H__C2C032E4_255F_4BE6_A16F_1A1FA9AC20C3__INCLUDED_)
