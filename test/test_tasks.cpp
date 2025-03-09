#include <gtest/gtest.h>
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"

extern "C" {
    extern void vSystemSetCriticalityLevel();
    extern List_t xReadyTasksListEDFVD;
    extern float xUtilization11, xUtilization21, xUtilization22;
    extern uint8_t uxCaseEDFVD;
}

class SystemSetCriticalityLevelTest : public ::testing::Test {
protected:
    void SetUp() override {
        prvInitialiseTaskLists();
        xUtilization11 = 0;
        xUtilization21 = 0;
        xUtilization22 = 0;
    }

    void TearDown() override {
        prvInitialiseTaskLists();
    }

    void AddTaskToReadyList(TCB_t* pxTCB, eCriticality eCrit, float xTaskPeriod, float xWCET1, float xWCET2) {
        pxTCB->eCriticality = eCrit;
        pxTCB->xWCET1 = xWCET1;
        pxTCB->xWCET2 = xWCET2;
        pxTCB->xTaskPeriod = xTaskPeriod;
        listSET_LIST_ITEM_OWNER(&pxTCB->xStateListItem, pxTCB);
        vListInsertEnd(&xReadyTasksListEDFVD, &pxTCB->xStateListItem);
    }
};

TEST_F(SystemSetCriticalityLevelTest, Case1_Success) {
    TCB_t task1, task2, task3, task4;
    AddTaskToReadyList(&task1, configTASK_CRTICALITY_LOW, 5,3,3);
    AddTaskToReadyList(&task2, configTASK_CRTICALITY_LOW, 10,8,8);
    AddTaskToReadyList(&task3, configTASK_CRTICALITY_HIGH, 30,20,30);
    AddTaskToReadyList(&task4, configTASK_CRTICALITY_HIGH, 10,5,7);
    
    vSystemSetCriticalityLevel();
    EXPECT_EQ(uxCaseEDFVD, configEDF_VD_CASE_2);
}

TEST_F(SystemSetCriticalityLevelTest, Case2_Success) {
    TCB_t task1, task2;
    AddTaskToReadyList(&task1, configTASK_CRTICALITY_LOW, 2.0, 0.0, 10.0);
    AddTaskToReadyList(&task2, configTASK_CRTICALITY_HIGH, 2.0, 3.0, 10.0);
    
    vSystemSetCriticalityLevel();
    EXPECT_EQ(uxCaseEDFVD, configEDF_VD_CASE_2);
}

TEST_F(SystemSetCriticalityLevelTest, SchedulingFailure) {
    TCB_t task1, task2;
    AddTaskToReadyList(&task1, configTASK_CRTICALITY_LOW, 5.0, 0.0, 10.0);
    AddTaskToReadyList(&task2, configTASK_CRTICALITY_HIGH, 5.0, 6.0, 10.0);
    
    EXPECT_DEATH(vSystemSetCriticalityLevel(), "Not possibile to schedule");
}
