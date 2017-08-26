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
	//���ļ�
	BOOL Open(LPCTSTR lpFileName);
	//�ر��ļ�
	void Close();
	//�����ļ�,�ڵ��������������в���֮ǰ�������ȵ��ô˺��������ļ�
	BOOL Parse();
	//�ж��Ƿ�������,ֻ���ڽ��������ܽ��к�������
	BOOL IsParseComplete();
	//��ȡ����
	CString GetSectionNameFromIndex(UINT nSecIndex);
	//��ȡ�����µ�����Field
	//e.g: [Section]
	//	   Field1=Value1
	//	   Field2=Value2
	//return: Field1, Field2
	vector<CString> GetFiledFromSection(CString strSectionName);
	//��Field��ȡֵ
	//e.g: [Section]
	//	   Field1=Value1
	//return: Value1
	CString GetValueStringFromField(CString strFieldName);
	//ȡ���ļ����ж��ٽ�
	UINT GetSectionCount();
	//����ĳ���ַ��ָ��ַ�����ֻ����һ�ηָ�ֳ�2����
	BOOL SplitStrByCh(CString strSrc, TCHAR ch, vector<CString>& arOut);
	//����ĳ���ַ��ָ��ַ���������ѭ���ָ�
	void SplitStrByChEx(CString strSrc, TCHAR ch, vector<CString>& arOut);
	//�޸�������ʽ
	void SetValueToStartTypeField(UINT nStartType = 3);

private:
	//ȥ��ǰ��ո�
	void TrimAll(CString& str);
	//ȥ��ǰ���ĳ��ƥ���ַ�
	void TrimChar(CString& str, TCHAR ch);
	//�Ƿ��ǿ���
	BOOL IsNewLine(CString str);
	//�Ƿ���ע����
	BOOL IsComment(CString str);
	//��ȡ����
	void GetSectionName(CString& strSection);
	//��������
	void Reset();

private:
	CStdioFile m_stdioFile;
	//�ļ��Ƿ��
	BOOL m_bOpenFile;
	//����
	UINT m_nSection;
	//��������
	CArray<CString, LPCTSTR> m_pSectNameArray;
	//���Ӧ��ֵ
	CMap<CString, LPCTSTR, CString, LPCTSTR> m_pFieldMapValue;
	//��������
	CMap<CString, LPCTSTR, vector<CString>, vector<CString>&> m_FieldMapSect;
	//�ļ��Ƿ�������
	BOOL m_bParseComplete;
	//���������������ļ��е�λ��
	DWORD m_nStartTypePos;
	//��ǰ�ļ�����λ��
	DWORD m_nCurFilePos;
	//��ǰ�е�ǰһ��λ��ָ��
	DWORD m_nPrevLinePos;
	//Inf�ļ�·��
	CString m_pStrInfFileName;
};

#endif // !defined(AFX_INFPARSER_H__C2C032E4_255F_4BE6_A16F_1A1FA9AC20C3__INCLUDED_)
