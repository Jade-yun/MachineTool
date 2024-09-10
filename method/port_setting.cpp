#include "port_setting.h"

//若没有对应端口号的，则索引定为99
uint8_t outputTypeIndex[OUT_PORT_TYPE_NUM]={0};                    //输出类型对应的下标集合
uint8_t outportInterlockIndex[OUT_INTERLOCK_NUM][4]={0};                //互锁设置
uint8_t outputRelevancyIndex[OUT_PORT_RELEVANCY_NUM][2]={0};                //预留关联
uint8_t outputReleteOutIndex[OUT_PORT_RELEOUT_NUM]={0};                    //预留出类型
uint8_t seniorFuncIndex[SENIOR_NUM]={0};                    //预留出类型

D_PortDefineStruct m_Port_X[INPUT_TOTAL_NUM];                                  //输入自定义
D_PortDefineStruct m_Port_Y[OUTPUT_TOTAL_NUM];                                  //输出自定义

void getCmdIni()
{
#if 0
    //X
    QStringList tempList11=getIniValues(3,"Port_X");
    QStringList tempList12=getIniValues(99,"Port_X_Name_Modify");
    QStringList tempList13=getIniValues(99,"Port_X_Number_Modify");             //仅为显示

    for(int i=0;i<INPUT_TOTAL_NUM;i++)
    {
        m_Port_X[i].definePort=QString("X%1").arg(i+1);
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
        if(tempList13[i].compare("0")==0)       //如果存的是0，则表示端口号没有修改
        {
            m_Port_X[i].portNum=i+1;
        }
        else
        {
            m_Port_X[i].portNum=tempList13[i].mid(1).toUInt();
        }
        if(i>=INPUT_NUM)
            m_Port_X[i].isReserve=true;
        else
            m_Port_X[i].isReserve=false;
    }

    //Y
    tempList11=getIniValues(3,"Port_Y");
    tempList12=getIniValues(99,"Port_Y_Name_Modify");
    tempList13=getIniValues(99,"Port_Y_Number_Modify");             //仅为显示
    for(int i=0;i<OUTPUT_TOTAL_NUM;i++)
    {
        m_Port_Y[i].definePort=QString("Y%1").arg(i+1);
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
        if(tempList13[i].compare("0")==0)       //如果存的是0，则表示端口号没有修改
        {
            m_Port_Y[i].portNum=i+1;
        }
        else
        {
            m_Port_Y[i].portNum=tempList13[i].mid(1).toUInt();
        }
        if(i>=OUTPUT_NUM)
            m_Port_Y[i].isReserve=true;
        else
            m_Port_Y[i].isReserve=false;
    }
    //名称自定义
    tempList11=getIniValues(3,"DefineName");
    tempList12=getIniValues(99,"DefineName_Modify");
    for(int i=0;i<DEFINE_NAME_NUM;i++)
    {
        m_NameDefine[i].defineName=tempList11[i];
        if(tempList12[i].compare("0")==0)       //如果存的是0，则表示名称没有修改，为默认名称
        {
            m_NameDefine[i].modifyName=tempList11[i];
        }
        else
        {
            m_NameDefine[i].modifyName=tempList12[i];
        }
    }



    uint8_t tempIndex=0;
    //输出类型
    tempList11=getIniValues(1,"OutPortType");
    for(int i=0;i<OUT_PORT_TYPE_NUM;i++)
    {
        tempIndex=tempList11[i].toUInt();
        if(tempIndex==0)
        {
            outputTypeIndex[i]=99;
            m_OutPortType[i][0]=0;
        }
        else
        {
            outputTypeIndex[i]=tempIndex-1;
            m_OutPortType[i][0]=m_Port_Y[tempIndex-1].portNum;
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
                outportInterlockIndex[i][j]=99;
                m_OutportInterlock[i][j]=0;
            }
            else
            {
                outportInterlockIndex[i][j]=tempIndex-1;
                switch (j)
                {
                case 0:
                    m_OutportInterlock[i][j]=m_Port_Y[tempIndex-1].portNum;
                    break;
                case 1:
                    m_OutportInterlock[i][j]=m_Port_X[tempIndex-1].portNum;
                    break;
                case 2:
                    m_OutportInterlock[i][j]=m_Port_Y[tempIndex-1].portNum;
                    break;
                case 3:
                    m_OutportInterlock[i][j]=m_Port_X[tempIndex-1].portNum;
                    break;
                }
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
                outputRelevancyIndex[i][j]=99;
                m_OutportRelevancy[i][j]=0;
            }
            else
            {
                outputRelevancyIndex[i][j]=tempIndex-1;
                m_OutportRelevancy[i][j]=m_Port_Y[tempIndex-1].portNum;
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
            outputReleteOutIndex[i]=99;
            m_OutportReleteOut[i][0]=0;
        }
        else
        {
            outputReleteOutIndex[i]=tempIndex-1;
            m_OutportReleteOut[i][0]=m_Port_Y[tempIndex-1].portNum;
        }
    }
    //高级功能对应的端口号的定义
    tempList11=getIniValues(1,"SeniorFunc");
    for(int i=0;i<SENIOR_NUM;i++)
    {
        tempIndex=tempList11[i].toUInt();
        if(tempIndex==0)
            seniorFuncIndex[i]=99;
        else
            seniorFuncIndex[i]=tempIndex-1;
    }

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
    m_SeniorFuncPort.startFinish1Port=tempList11[index++].toUInt();
    m_SeniorFuncPort.startFinish2Port=tempList11[index++].toUInt();
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

