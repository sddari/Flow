#ifndef SDATAMANAGE_H
#define SDATAMANAGE_H

#include <QDataStream>




class SDataManage
{
public:
    SDataManage();
private:    
public:
    bool Save(QString _str);
    bool Load(QString _str);

    SItem *GetItem(int _nSelect);
    SLine *GetLine(int _nSelect);
    
    int InsertItem(SItem item);
    int InsertLine(SLine item);

    void DeleteItem(int _nSelect);
};

#endif // SDATAMANAGE_H
