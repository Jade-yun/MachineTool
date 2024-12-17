#include "port_setting.h"

//若没有对应端口号的，则索引定为99
uint8_t outportInterlockIndex[OUT_INTERLOCK_NUM][4]={0};                //互锁设置,存储的实际端口号

/*************************************************************************
**  函数名：  readSigSetPara()
**	输入参数：无
**	输出参数：无
**	函数功能：读取端口自定义的所有信息
**  作者：    PXC
**  开发日期：2024/11/1
**************************************************************************/
void readSigSetPara()
{
#if 1
    //X
    QStringList tempList11=getIniValues(3,"Port_X");
    QStringList tempList12=getIniValues(99,"Port_X_Name_Modify");
    QStringList tempList13=getIniValues(99,"Port_X_Number_Modify");             //仅为显示

    QStringList tempList17=getIniValues(3,"ResPort_X");
    QStringList tempList18=getIniValues(99,"ResPort_X_Name_Modify");
    QStringList tempList19=getIniValues(99,"Port_X_functionset");

    for(int i=0;i<INPUT_TOTAL_NUM;i++)
    {
        if(i<INPUT_NUM)
        {
            m_Port_X[i].definePort=QString("X%1").arg(i+1);
            m_Port_X[i].isReserve=false;
        }
        else
        {
            m_Port_X[i].definePort=QString("EX%1").arg(i-INPUT_NUM+1);
            m_Port_X[i].isReserve=true;
        }

        m_Port_X[i].defineName=tempList11[i];
        if(tempList12[i].compare("0")==0)       //如果存的是0，则表示名称没有修改，为默认名称
        {
            m_Port_X[i].modifyName=tempList11[i];
        }
        else
        {
            m_Port_X[i].modifyName=tempList12[i];
        }
        m_Port_X[i].modifyPort=tempList13[i];
        //由于如果没改的话，配置文件里面存的是0，需要将0转化为实际的端口号
        m_Port_X[i].portNum=i+1;
        if(tempList13[i].compare("0")==0)       //如果存的是0，则表示端口号没有修改
        {
            m_Port_X[i].actualPortNum=i+1;
        }
        else
        {
            m_Port_X[i].actualPortNum=tempList13[i].mid(1).toUInt();
        }
        //读取输入端口做预留功能时的名称
        m_Port_X[i].ResDefineName=tempList17[i];
        if(tempList18[i].compare("0")==0)
        {
            m_Port_X[i].ResModifyName=tempList17[i];
        }
        else
        {
            m_Port_X[i].ResModifyName=tempList18[i];
        }
        //读取每个端口是否设置了指定功能
        m_Port_X[i].functionSet = (uint8_t)tempList19[i].toUInt();
    }

    //Y
    tempList11=getIniValues(3,"Port_Y");
    tempList12=getIniValues(99,"Port_Y_Name_Modify");
    tempList13=getIniValues(99,"Port_Y_Number_Modify");             //仅为显示

    QStringList tempList14=getIniValues(3,"ResPort_Y");
    QStringList tempList15=getIniValues(99,"ResPort_Y_Name_Modify");
    QStringList tempList16=getIniValues(99,"Port_Y_functionset");

    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        if(i<OUTPUT_NUM)
        {
            m_Port_Y[i].definePort=QString("Y%1").arg(i+1);
            m_Port_Y[i].isReserve=false;
        }
        else
        {
            m_Port_Y[i].definePort=QString("EY%1").arg(i-OUTPUT_NUM+1);
            m_Port_Y[i].isReserve=true;
        }

        m_Port_Y[i].defineName=tempList11[i];
        if(tempList12[i].compare("0")==0)       //如果存的是0，则表示名称没有修改，为默认名称
        {
            m_Port_Y[i].modifyName=tempList11[i];
        }
        else
        {
            m_Port_Y[i].modifyName=tempList12[i];
        }
        m_Port_Y[i].modifyPort=tempList13[i];
        m_Port_Y[i].portNum=i+1;
        if(tempList13[i].compare("0")==0)       //如果存的是0，则表示端口号没有修改
        {
            m_Port_Y[i].actualPortNum=i+1;
        }
        else
        {
            m_Port_Y[i].actualPortNum=tempList13[i].mid(1).toUInt();
        }
        //读取输出端口做预留功能时的名称
        m_Port_Y[i].ResDefineName=tempList14[i];
        if(tempList15[i].compare("0")==0)
        {
            m_Port_Y[i].ResModifyName=tempList14[i];
        }
        else
        {
            m_Port_Y[i].ResModifyName=tempList15[i];
        }
        //读取每个端口是否设置了指定功能
        m_Port_Y[i].functionSet = (uint8_t)tempList16[i].toUInt();
    }


    //名称自定义
    ::readNameDefine();

    uint8_t tempIndex=0;
    //输出类型
    tempList11=getIniValues(1,"OutPortType");
    for(int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        tempIndex=tempList11[i].toUInt();
        if(tempIndex==0)
        {
            m_OutPortType[i][0]=0;
        }
        else
        {
            m_OutPortType[i][0]=tempIndex/*m_Port_Y[tempIndex-1].portNum*/;
        }

    }
    //互锁设置
    tempList11=getIniValues(1,"OutportInterlock");
    for(int i=0;i<OUT_INTERLOCK_NUM;i++)
    {
        for(int j=0;j<4;j++)
        {
            tempIndex=tempList11[4*i+j].toUInt();
            if(tempIndex==0)
            {
                outportInterlockIndex[i][j]=0;
            }
            else
            {
                outportInterlockIndex[i][j]=tempIndex;
            }
        }
    }
    //预留关联
    tempList11=getIniValues(1,"OutputRelevancy");
    for(int i=0;i<OUT_PORT_RELEVANCY_NUM;i++)
    {
        for(int j=0;j<2;j++)
        {
            tempIndex=tempList11[2*i+j].toUInt();
            if(tempIndex==0)
            {
                m_OutportRelevancy[i][j]=0;
            }
            else
            {
                m_OutportRelevancy[i][j]=tempIndex;
            }
        }
    }
    //预留出类型
    tempList11=getIniValues(1,"OutputReleteOut");
    for(int i=0;i<OUT_PORT_RELEOUT_NUM;i++)
    {
        tempIndex=tempList11[i].toUInt();
        if(tempIndex==0)
        {
            m_OutportReleteOut[i][0]=0;
        }
        else
        {
            m_OutportReleteOut[i][0]=tempIndex;
        }
    }
    //高级功能对应的端口号的定义
    tempList11=getIniValues(1,"SeniorFunc");
    uint8_t index=0;
    m_SeniorFuncPort.knifeOrigin1CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.knifeOrigin2CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.chuckOriginCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.stackSaveIn1CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.stackSaveIn2CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.stackSaveOutCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmIn1CheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmIn2CheckPort=tempList11[index++].toUInt();

    m_SeniorFuncPort.emergencyStopCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.pauseStopCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.pressureCheckPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.remoteAutoPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.remoteStopPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.bottomOilLimitPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.oilerSwtPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.lubPumpPort=tempList11[index++].toUInt();

    m_SeniorFuncPort.processSave1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.processSave2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.emergencySnapMotorEnablePort=tempList11[index++].toUInt();
    m_SeniorFuncPort.emergencyStopOutPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.autoLightPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmLightPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.alarmBuzzerPort=tempList11[index++].toUInt();
    m_SeniorFuncPort.pauseLightPort=tempList11[index++].toUInt();

    m_SeniorFuncPort.processFinish1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.processFinish2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.locateFinish1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.locateFinish2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.startProduct1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.startProduct2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.res1[0]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res1[1]=tempList11[index++].toUInt();

    m_SeniorFuncPort.mainAxisRotate1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.mainAxisRotate2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.mainAxisLocate1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.mainAxisLocate2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.biowAir1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.biowAir2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.res2[0]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res2[1]=tempList11[index++].toUInt();

    m_SeniorFuncPort.manualChuckIn1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.manualChuckIn2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.autoDoorCot1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.autoDoorCot2Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[0]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[1]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[2]=tempList11[index++].toUInt();
    m_SeniorFuncPort.res3[3]=tempList11[index++].toUInt();
#endif
}

/*************************************************************************
**  函数名：  readIniPara()
**	输入参数：无
**	输出参数：无
**	函数功能：读取整个工程中所有配置文件的信息，将参数读至所有的结构体
**  作者：    PXC
**  开发日期：2024/11/1
**************************************************************************/
void readIniPara()
{
    getOutPortType();
    getOutportInterlock();
    getOutportRelevancy();
    getReleteOut();
    getKeyFunc();
    getLedFunc();
    getSeniorFunc();
    getMachineSave();
    getStackSave();
    getClawSave();
    getOnlineSafe();
    getProductSet();
    getProductSenior();
    getInternet();
    getAxisPar();
    getHandwheelPar();
    getServoPar();
    getDegreeParS();
    getSaveArea();
    getServoCommPar();
    getStackInfo();
    getStackFunc();
    getManualAxis();

    getSystemSet();
    ::readPasswdFromConfig();
}
