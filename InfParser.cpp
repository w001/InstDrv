// InfParser.cpp: implementation of the CInfParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InstDrv.h"
#include "InfParser.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInfParser::CInfParser()
: m_nSection(0)
, m_bOpenFile(FALSE)
, m_bParseComplete(FALSE)
, m_nStartTypePos(0)
, m_nCurFilePos(0)
, m_nPrevLinePos(0)
{
	m_pSectNameArray.RemoveAll();
	m_FieldMapSect.RemoveAll();
	m_pFieldMapValue.RemoveAll();
	m_pStrInfFileName = _T("");
}

CInfParser::~CInfParser()
{
	Reset();
	m_nStartTypePos = 0;
	m_pStrInfFileName = _T("");
}

void CInfParser::Reset()
{
	if (m_bOpenFile)
	{
		m_stdioFile.Close();
		m_bOpenFile = FALSE;
	}
	m_pSectNameArray.RemoveAll();
	m_FieldMapSect.RemoveAll();
	m_pFieldMapValue.RemoveAll();
	m_nSection = 0;
	m_bParseComplete = FALSE;
	m_nCurFilePos = 0;
	m_nPrevLinePos = 0;
}

BOOL CInfParser::Open(LPCTSTR lpFileName)
{
	if (m_stdioFile.Open(lpFileName, CFile::modeReadWrite | CFile::typeText))
	{
		m_pStrInfFileName = lpFileName;
		m_bOpenFile = TRUE;
		return TRUE;
	}
	return FALSE;
}

void CInfParser::Close()
{
	if (m_bOpenFile)
	{
		Reset();
	}
}

void CInfParser::TrimAll(CString& str)
{
	str.TrimLeft();
	str.TrimRight();
}

void CInfParser::TrimChar(CString& str, TCHAR ch)
{
	str.TrimLeft(ch);
	str.TrimRight(ch);
}

BOOL CInfParser::IsComment(CString str)
{
	TrimAll(str);
	if (str.GetAt(0) == ';')
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CInfParser::IsNewLine(CString str)
{
	TrimAll(str);
	if (str.IsEmpty())
	{
		return TRUE;
	}
	return FALSE;
}

CString CInfParser::GetSectionNameFromIndex(UINT nSecIndex)
{
	return m_pSectNameArray.GetAt(nSecIndex);
}

void CInfParser::GetSectionName(CString& strSection)
{
	TrimAll(strSection);
	TrimChar(strSection, '[');
	TrimChar(strSection, ']');
}

UINT CInfParser::GetSectionCount()
{
	return m_nSection;
}

BOOL CInfParser::IsParseComplete()
{
	return m_bParseComplete;
}

BOOL CInfParser::Parse()
{
	CString strContent;
	while(m_stdioFile.ReadString(strContent))
	{
		TrimAll(strContent);
		//换行
		if (IsNewLine(strContent))
		{
			continue;
		}
		//注释行
		if (IsComment(strContent))
		{
			continue;
		}
		//节行
		if (strContent.GetAt(0) == '[')
		{
			GetSectionName(strContent);
			m_nSection++;
			m_pSectNameArray.Add(strContent);
		}
		//数据行,取出最后添加的节名
		else
		{
			m_nPrevLinePos = m_nCurFilePos;
			m_nCurFilePos = m_stdioFile.GetPosition();
			CString strSecName = m_pSectNameArray.GetAt(m_nSection - 1);
			TrimAll(strContent);
			vector<CString> arTemp;
			SplitStrByCh(strContent, '=', arTemp);
			if (arTemp.size() != 0)
			{
				if (arTemp[0] == "StartType")
				{
					m_nStartTypePos = m_nPrevLinePos;
				}
			}
			//取出左边第一个是域名
			vector<CString> arField;
			if (!m_FieldMapSect.Lookup(strSecName, arField))
			{
				if (arTemp.size() == 0)
				{
					arField.push_back(strContent);
				}
				else
				{
					arField.push_back(arTemp[0]);
				}	
			}
			else
			{
				if (arTemp.size() == 0)
				{
					arField.push_back(strContent);
				}
				else
				{
					arField.push_back(arTemp[0]);
				}	
			}
			m_FieldMapSect.SetAt(strSecName, arField);
			
			if (arTemp.size() == 0)
			{
				m_pFieldMapValue.SetAt(strContent, "");
			}
			else
			{
				m_pFieldMapValue.SetAt(arTemp[0], arTemp[1]);
			}		
		}
	}
	m_bParseComplete = TRUE;
	return m_bParseComplete;
}

vector<CString> CInfParser::GetFiledFromSection(CString strSectionName)
{
	vector<CString> arFieldName;
	if (!m_FieldMapSect.IsEmpty())
	{	
		if (m_FieldMapSect.Lookup(strSectionName, arFieldName))
		{
			return arFieldName;
		}
	}
	return arFieldName;
}

CString CInfParser::GetValueStringFromField(CString strFieldName)
{
	if (!m_pFieldMapValue.IsEmpty())
	{
		CString strValue;
		if (m_pFieldMapValue.Lookup(strFieldName, strValue))
		{
			return strValue;
		}
	}
	return "";
}

BOOL CInfParser::SplitStrByCh(CString strSrc, TCHAR ch, vector<CString>& arOut)
{
	TrimAll(strSrc);
	int index = strSrc.Find(ch);
	if (index == -1)
	{
		return FALSE;
	}
	CString strLeft = strSrc.Left(index);
	TrimAll(strLeft);
	arOut.push_back(strLeft);
	CString strRight = strSrc.Right(strSrc.GetLength() - index - 1);
	TrimAll(strRight);
	arOut.push_back(strRight);
	return TRUE;
}

void CInfParser::SplitStrByChEx(CString strSrc, TCHAR ch, vector<CString>& arOut)
{
	do 
	{
		TrimAll(strSrc);
		int index = strSrc.Find(ch);
		if (index == -1)
		{
			arOut.push_back(strSrc);
			return;
		}
		CString strLeft = strSrc.Left(index);
		TrimAll(strLeft);
		arOut.push_back(strLeft);
		CString strRight = strSrc.Right(strSrc.GetLength() - index - 1);
		strSrc = strRight;
	} while(TRUE);
}

void CInfParser::SetValueToStartTypeField(UINT nStartType /* = 3 */)
{
	if (!m_bOpenFile)
	{
		if (m_stdioFile.Open(m_pStrInfFileName, CFile::modeReadWrite | CFile::typeText))
		{
			m_bOpenFile = TRUE;
		}
	}
	m_stdioFile.Seek(m_nStartTypePos, CFile::begin);
	CString strStartType;
	m_stdioFile.ReadString(strStartType);
	TrimAll(strStartType);
	vector<CString> arStartType;
	SplitStrByCh(strStartType, '=', arStartType);
	if (arStartType.size() == 0)
	{
		if (m_bOpenFile)
		{
			m_stdioFile.Close();
			m_bOpenFile = FALSE;
		}
		return;
	}
	strStartType = arStartType[1];
	TrimAll(strStartType);
	vector<CString> arTypeValue;
	if (!SplitStrByCh(strStartType, ';', arTypeValue))
	{
		if (atoi((char*)strStartType.AllocSysString()) == nStartType)
		{
			if (m_bOpenFile)
			{
				m_stdioFile.Close();
				m_bOpenFile = FALSE;
			}
			return;
		}
		m_stdioFile.Seek(m_nStartTypePos, CFile::begin);
		CString strNewValue;
		m_stdioFile.ReadString(strNewValue);
		CString strNewType;
		strNewType.Format(_T("%d"), nStartType);
		strNewValue.Replace(strStartType, strNewType);
		m_stdioFile.Seek(m_nStartTypePos, CFile::begin);
		m_stdioFile.WriteString(strNewValue);
		if (m_bOpenFile)
		{
			m_stdioFile.Close();
			m_bOpenFile = FALSE;
		}
		return;
	}
	if (atoi((char*)arTypeValue[0].AllocSysString()) == nStartType)
	{
		if (m_bOpenFile)
		{
			m_stdioFile.Close();
			m_bOpenFile = FALSE;
		}
		return;
	}
	CString strComment;
	switch(nStartType)
	{
	case 0:
		strComment = _T("SERVICE_BOOT_START");
		break;

	case 2:
		strComment = _T("SERVICE_AUTO_START");
		break;

	case 3:
		strComment = _T("SERVICE_DEMAND_START");
		break;

	default:
		return;
	}
	m_stdioFile.Seek(m_nStartTypePos, CFile::begin);
	CString strNewValue;
	m_stdioFile.ReadString(strNewValue);
	CString strNewType;
	strNewType.Format(_T("%d"), nStartType);
	strNewValue.Replace(arTypeValue[0], strNewType);
	strNewValue.Replace(arTypeValue[1], strComment);
	m_stdioFile.Seek(m_nStartTypePos, CFile::begin);
	m_stdioFile.WriteString(strNewValue);
	if (m_bOpenFile)
	{
		m_stdioFile.Close();
		m_bOpenFile = FALSE;
	}
}


