#ifndef TYPEDEF_H
#define TYPEDEF_H



typedef enum
{
    New,
    Modify,
} DB_Update_Catgroy;

typedef enum
{
    AllEquipmentTable,
    ConsumableMaterialTable,
} Sqltable;

typedef enum
{
    Calibration_Equipment,
    Verify_Equipment,
    AllEquipments,
} CalOrVer;

#endif // TYPEDEF_H
