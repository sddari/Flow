#include "sdatamanage.h"


SItem *SDataManage::GetItem(int _nSelect)
{
    if (_nSelect < 0 && _nSelect > m_vecItems.size() )  return 0;
    return &m_vecItems[_nSelect];
}

SLine *SDataManage::GetLine(int _nSelect)
{
    if (_nSelect < 0 && _nSelect > m_vecLines.size() )  return 0;
    return &m_vecLines[_nSelect];
}

int SDataManage::InsertItem(SItem item)
{
    m_vecItems.push_back(item);
    return (m_vecItems.size()-1);
}

int SDataManage::InsertLine(SLine line)
{
    m_vecLines.push_back(line);
    return (m_vecLines.size()-1);
}

int SDataManage::InsertLine(QPoint _ptStart,QPoint _ptEnd)
{
    m_vecLines.push_back(line);
    return (m_vecLines.size()-1);
}

void SDataManage::DeleteItem(int _nSelect)
{
    m_vecItems.erase(m_vecItems.begin() + _nSelect);
}
